#include "XYModem.h"

namespace xymodem
{
/**
 * XModem sender
 */
XModem::XModem (DeviceHandler& deviceHandler_,
                const logType pathToLogFile,
                const int logLevel)
    : FileTransferProtocol (
          deviceHandler_, waitingStart, pathToLogFile, logLevel)
{
    guards.addGuards ({{retries, 0}, {packetsLeft, 0}});
};

XModem::XModem (DeviceHandler& deviceHandler_,
                std::shared_ptr<spdlog::logger> logger)
    : FileTransferProtocol (deviceHandler_, waitingStart, std::move (logger))
{
    guards.addGuards ({{retries, 0}, {packetsLeft, 0}});
};

Packet XModem::makeDataPacket (const std::string& data_,
                               const uint8_t& t_packetNum,
                               const bool logHex)
{
    Packet packet = {0x00};
    packetData data;
    // packets except header and last packet are filled with 0xFF
    for (auto& byte : data)
    {
        byte = 0xFF;
    }

    // Making header (SOH/STX, packetnum(0-255), 255-packetnum)
    const std::array<uint8_t, xyModemConst::packetHeaderSize> header = {
        xyModemConst::STX,
        t_packetNum,
        static_cast<unsigned char> (~t_packetNum)};

    // Copying the file data to data
    std::copy (data_.begin (), data_.end (), data.begin ());

    // Computing CRC
    const auto crc = tools::compute_crc16xmodem (data);
    const auto crc_hi = static_cast<uint8_t> (crc >> 8);
    const auto crc_lo = static_cast<uint8_t> (crc);

    // Copying the header, data and crc to the packet
    std::copy (header.begin (), header.end (), packet.begin ());
    std::copy (data.begin (), data.end (), packet.begin () + header.size ());
    *(packet.end () - 2) = crc_hi;
    *(packet.end () - 1) = crc_lo;

    if (logHex)
        logger->debug (tools::dispByteArray<packet.size ()> (packet));

    return packet;
}


void XModem::writePacket (Packet packet)
{
    deviceHandler.flushAllInputBuffers ();
    deviceHandler.write (packet.data (), packet.size ());
}


void XModem::executeSendPacket (const bool logHex)
{
    if (guards.get (packetsLeft) == 1)
    {

        logger->debug (packetNum);
        const auto packet =
            makeDataPacket (getNextFileBlock (dataFile,
                                              (lastPacketRemaining == 0)
                                                  ? xyModemConst::packetDataSize
                                                  : lastPacketRemaining),
                            packetNum,
                            logHex);
        writePacket (packet);
        logger->debug ("Sent last packet");
    }
    else
    {
        const auto packet = makeDataPacket (
            getNextFileBlock (dataFile, xyModemConst::packetDataSize),
            packetNum,
            logHex);
        writePacket (packet);
        logger->debug ("Packets left {}", guards.get (packetsLeft));
    }
    tools::increasePacketNum (packetNum);
    guards.dec (packetsLeft);
    assert (guards.get (packetsLeft) >= 0);
    updateCallback (
        static_cast<float> (totalPackets - guards.get (packetsLeft)) /
        static_cast<float> (totalPackets));
    logger->debug (
        "Sent {}\\% packets",
        static_cast<float> (totalPackets - guards.get (packetsLeft)) /
            static_cast<float> (totalPackets) * 100);
}

void XModem::executeState (const unsigned int state, bool logHex)
{
    switch (state)
    {
    case sendingPacket:
        guards.set (retries, 0);
        executeSendPacket (logHex);
        break;
    case retryingPacket:
        tools::decreasePacketNum (packetNum);
        guards.inc (packetsLeft);
        guards.inc (retries);
        executeSendPacket (logHex);
        break;
    case endOfTransmission:
    {
        guards.set (retries, 0);
        const uint8_t buffer[1] = {xyModemConst::EOT};
        deviceHandler.flushAllInputBuffers ();
        deviceHandler.write (buffer, 1);
        logger->debug ("Sent EOT");
        break;
    }
    case retryingEOT:
    {
        guards.inc (retries);
        const uint8_t buffer[1] = {xyModemConst::EOT};
        deviceHandler.flushAllInputBuffers ();
        deviceHandler.write (buffer, 1);
        logger->debug ("Re-sent EOT");
        break;
    }
    case transmissionFinished:
        isTransmissionFinished = true;
        logger->info ("XModem transmission finished successfully !");
        break;
    case abort:
        isTransmissionFinished = true;
        logger->warn ("XModem transmission aborted, perhaps too many retries");
        throw XYModemExceptions::TransmissionAborted ();
        break;
    default:
        logger->info ("Nothing to do");
        break;
    }
}

void XModem::transmit (const ghc::filesystem::path& fileAbsolutePath,
                       std::function<void (float)> updateCallback_,
                       std::function<bool ()> yieldCallback_,
                       const bool logHex)
{
    guards.set (retries, 0);
    updateCallback = std::move (updateCallback_);
    yieldCallback = std::move (yieldCallback_);

    //// Prepare all required information
    dataFile.open (fileAbsolutePath, std::ios::binary);
    if (!dataFile.is_open ())
    {
        logger->error ("Could not open file located at " +
                       fileAbsolutePath.string ());
        throw (XYModemExceptions::CouldNotOpenFile (fileAbsolutePath));
    }
    setFileInfos (fileAbsolutePath);
    ////

    //// Compute the number of packets to send
    const auto divNumPacket =
        std::lldiv (fileInfos.fileSize,
                    static_cast<std::intmax_t> (xyModemConst::packetDataSize));

    lastFullPacket = divNumPacket.quot;     // Number of fullsize packets
    lastPacketRemaining = divNumPacket.rem; // Remaining bytes
    logger->debug ("lastfullpacket {}", lastFullPacket);
    logger->debug ("remaining {}", lastPacketRemaining);
    guards.set (packetsLeft, lastFullPacket);
    if (lastPacketRemaining != 0)
        guards.inc (packetsLeft); // In case the file is less
                                  // than
                                  // xyModemConst::packetDataSize
                                  // bytes
    totalPackets = guards.get (packetsLeft);
    ////

    // Initial flush of buffer to remove data from previous uses, and put back
    // the file iterator at the beginning for insurance
    dataFile.seekg (0, std::ios::beg);
    deviceHandler.flushAllInputBuffers ();

    logger->info ("Beginning transmission");
    auto timerStart = std::chrono::steady_clock::now ();
    while (!isTransmissionFinished)
    {
        if (!yieldCallback ())
        {
            if (deviceHandler.waitReadable ())
            {
                deviceHandler.readAll ();
                logger->debug ("Device input buffer size {}",
                               deviceHandler.available ());
                logger->debug ("Local buffer size {}",
                               deviceHandler.getInputBufferSize ());
                logger->debug ("Printing input buffer");
                deviceHandler.showBuffer ();
                logger->debug ("Buffer front : {}",
                               deviceHandler.getInputBufferFront ());

                const auto characterReceived = deviceHandler.readNextByte ();
                currentState = getNextState (characterReceived,
                                             currentState,
                                             undefined,
                                             stateTransitions);
                logger->debug ("Current state {}", currentState);
                executeState (currentState, logHex);

                timerStart =
                    std::chrono::steady_clock::now (); // reset the timeout
            }
            else
            {
                logger->warn ("Timeout ... Transmission aborted");
                throw XYModemExceptions::Timeout (
                    xyModemConst::timeout.count ());
                break;
            }
        }
        else
        {
            isTransmissionFinished = true;
        }
    }
    dataFile.close ();
}

// ====================================================================

YModem::YModem (DeviceHandler& deviceHandler_,
                const logType pathToLogFile,
                const int logLevel)
    : FileTransferProtocol (
          deviceHandler_, waitingStart, pathToLogFile, logLevel),
      xModem (deviceHandler_, logger)
{
    guards.addGuard (retries, 0);
};

Packet YModem::makeHeaderPacket (const std::string& fileName_,
                                 const int64& fileSize_,
                                 const int64& lastModificationDate_,
                                 const bool logHex)
{
    assert (fileSize_ >= 0);
    assert (lastModificationDate_ >= 0);
    Packet packet = {0x00};
    packetData data = {0x00};
    auto dataIterator = data.begin (); // NOLINT(readability-qualified-auto)

    // Making header

    const std::array<uint8_t, xyModemConst::packetHeaderSize> header = {
        xyModemConst::STX, 0x00, 0xFF};

    // Adding the required information in data (filename, file length,
    // modification data since UNIX epoch in seconds)

    std::copy (fileName_.begin (), fileName_.end (), dataIterator);
    dataIterator += fileName_.size ();
    *(dataIterator++) = xyModemConst::fileNameSep;
    const auto fileSizeStr = std::to_string (fileSize_);
    std::copy (fileSizeStr.begin (), fileSizeStr.end (), dataIterator);
    dataIterator += fileSizeStr.size ();
    *(dataIterator++) = xyModemConst::headerFieldsSep;
    const auto lastModificationDateStr = std::to_string (lastModificationDate_);
    std::copy (lastModificationDateStr.begin (),
               lastModificationDateStr.end (),
               dataIterator);

    // Computing CRC
    const auto crc = tools::compute_crc16xmodem (data);
    const auto crc_hi = static_cast<uint8_t> (crc >> 8);
    const auto crc_lo = static_cast<uint8_t> (crc);

    // Copying the header, data and crc to the packet
    std::copy (header.begin (), header.end (), packet.begin ());
    std::copy (data.begin (), data.end (), packet.begin () + header.size ());
    *(packet.end () - 2) = crc_hi;
    *(packet.end () - 1) = crc_lo;

    if (logHex)
        logger->debug (tools::dispByteArray<packet.size ()> (packet));
    return packet;
}

Packet YModem::makeLastPacket (const bool logHex)
{
    Packet packet = {0x00};
    packetData data = {0x00}; // Last packet filed with 0s

    // Making the header(SOH/STX, 0x00, 0xFF)
    const std::array<uint8_t, xyModemConst::packetHeaderSize> header = {
        xyModemConst::STX, 0x00, 0xFF};

    // Copying the header, data and crc to the packet
    std::copy (header.begin (), header.end (), packet.begin ());
    std::copy (data.begin (), data.end (), packet.begin () + header.size ());
    if (logHex)
        logger->debug (tools::dispByteArray<packet.size ()> (packet));

    return packet;
}

void YModem::writePacket (Packet packet)
{
    deviceHandler.flushAllInputBuffers ();
    deviceHandler.write (packet.data (), packet.size ());
}

void YModem::executeSendHeader (bool logHex)
{
    if (filesAbsolutePaths.empty ())
    {
        const auto packet = makeLastPacket (logHex);
        writePacket (packet);
        transmissionShouldFinish = true;
    }
    else
    {
        setFileInfos (filesAbsolutePaths.back ());
        const auto initialPacket =
            makeHeaderPacket (fileInfos.fileName,
                              fileInfos.fileSize,
                              fileInfos.lastModificationDate,
                              logHex);
        deviceHandler.flushAllInputBuffers ();
        writePacket (initialPacket);
        logger->info ("Wrote header packet");
    }
}
void YModem::executeState (const unsigned int t_currentState, bool logHex)
{
    switch (t_currentState)
    {
    case sendingHeader:
        guards.set (retries, 0);
        executeSendHeader (logHex);
        break;
    case retryingHeader:
        guards.inc (retries);
        executeSendHeader (logHex);
        break;
    case xModemTransmission:
        if (transmissionShouldFinish)
        {
            isTransmissionFinished = true;
            logger->info ("YModem transmission finished !");
        }
        else
        {
            guards.set (retries, 0);
            logger->info ("Beginning XModem transmission");
            xModem.transmit (filesAbsolutePaths.back (),
                             updateCallback,
                             yieldCallback,
                             logHex);
            filesAbsolutePaths.pop_back ();
            logger->info ("Finished XModem transmission !");
        }
        break;
    case transmissionFinished:
        isTransmissionFinished = true;
        logger->info ("YModem transmission finished successfully !");
        break;
    case abort:
        isTransmissionFinished = true;
        logger->warn ("YModem transmission aborted, perhaps too many retries");
        throw XYModemExceptions::TransmissionAborted ();
        break;
    default:
        logger->info ("Nothing to do");
        break;
    }
}


void YModem::transmit (
    const std::vector<ghc::filesystem::path>& filesAbsolutePaths_,
    std::function<void (float)> updateCallback_,
    std::function<bool ()> yieldCallback_,
    const bool logHex)
{
    assert (!filesAbsolutePaths_.empty ());

    // Preparing for new transfer
    guards.clear ();
    filesAbsolutePaths = filesAbsolutePaths_;
    updateCallback = std::move (updateCallback_);
    yieldCallback = std::move (yieldCallback_);
    deviceHandler.flushAllInputBuffers ();
    //

    logger->info ("Beginning transmission");
    while (!isTransmissionFinished)
    {
        if (!yieldCallback ())
        {
            if (deviceHandler.available () > 0)
            {
                deviceHandler.readAll ();
                logger->debug ("Device input buffer size {}",
                               deviceHandler.available ());
                logger->debug ("Local buffer size {}",
                               deviceHandler.getInputBufferSize ());
                logger->debug ("Printing input buffer");
                deviceHandler.showBuffer ();
                logger->debug ("Buffer front : {}",
                               deviceHandler.getInputBufferFront ());
                const auto characterReceived = deviceHandler.readNextByte ();
                currentState = getNextState (characterReceived,
                                             currentState,
                                             undefined,
                                             stateTransitions);
                logger->debug ("Current state {}", currentState);
                executeState (currentState, logHex);
                if (currentState != xModemTransmission)
                {
                    if (!deviceHandler.waitReadable ())
                    {
                        logger->warn ("Timeout ... Transmission aborted");
                        throw XYModemExceptions::Timeout (
                            xyModemConst::timeout.count ());
                    }
                }
            }
        }
        else
        {
            isTransmissionFinished = true;
        }
    }
    dataFile.close ();
}
} // namespace xymodem
