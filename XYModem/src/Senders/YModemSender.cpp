﻿#include "Senders/YModemSender.h"

// ====================================================================

namespace xymodem
{

template<int payloadSize>
YModemSender<payloadSize>::YModemSender (const std::shared_ptr<DeviceHandler>& deviceHandler_, const std::shared_ptr<Logger>& logger)
    : FileTransferProtocol (
          deviceHandler_, waitingStart, logger),
      xModem (deviceHandler_, logger)
{
    static_assert(payloadSize == xyModemConst::payloadSize1K || payloadSize == xyModemConst::payloadSize128);
    guards.addGuard (retries, 0);
}

template<int payloadSize>
std::array<uint8_t, payloadSize + xyModemConst::totalExtraSize> YModemSender<payloadSize>::makeHeaderPacket (const std::string& fileName_,
                                 const int64& fileSize_,
                                 const int64& lastModificationDate_,
                                 const bool logHex)
{
    assert (fileSize_ >= 0);
    assert (lastModificationDate_ >= 0);
    std::array<uint8_t, payloadSize + xyModemConst::totalExtraSize> packet = {0x00};
    std::array<uint8_t, payloadSize> data = {0x00};
    auto dataIterator = data.begin (); // NOLINT(readability-qualified-auto)

    // Making header
    static uint8_t payloadType = 0;
    if constexpr (payloadSize == xyModemConst::payloadSize1K)
    {
        payloadType = xyModemConst::STX;
    }
    else
    {
        payloadType = xyModemConst::SOH;
    }
    const std::array<uint8_t, xyModemConst::packetHeaderSize> header = {
        payloadType, 0x00, 0xFF};

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
    const auto crc = xymodem::tools::compute_crc16xmodem<payloadSize>(data);
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

template<int payloadSize>
std::array<uint8_t, payloadSize + xyModemConst::totalExtraSize> YModemSender<payloadSize>::makeLastPacket (const bool logHex)
{
    std::array<uint8_t, payloadSize + xyModemConst::totalExtraSize> packet = { 0x00 };
    std::array<uint8_t, payloadSize> data = { 0x00 }; // Last packet filed with 0s

    // Making the header(SOH/STX, 0x00, 0xFF)
    static uint8_t payloadType = 0;
    if constexpr (payloadSize == xyModemConst::payloadSize1K)
    {
        payloadType = xyModemConst::STX;
    }
    else
    {
        payloadType = xyModemConst::SOH;
    }
    const std::array<uint8_t, xyModemConst::packetHeaderSize> header = {
        payloadType, 0x00, 0xFF};

    // Copying the header, data and crc to the packet
    std::copy (header.begin (), header.end (), packet.begin ());
    std::copy (data.begin (), data.end (), packet.begin () + header.size ());
    if (logHex)
        logger->debug (tools::dispByteArray<packet.size ()> (packet));

    return packet;
}

template<int payloadSize>
void YModemSender<payloadSize>::writePacket (std::array<uint8_t, payloadSize + xyModemConst::totalExtraSize> packet)
{
    deviceHandler->flushAllInputBuffers ();
    deviceHandler->write (packet.data (), packet.size ());
}

template<int payloadSize>
void YModemSender<payloadSize>::executeSendHeader (bool logHex)
{
    if (files.empty ())
    {
        const auto packet = makeLastPacket (logHex);
        writePacket (packet);
        transmissionShouldFinish = true;
    }
    else
    {
        auto file = files.back();
        const auto initialPacket =
            makeHeaderPacket (file->getFilename(),
                              file->getFilesize(),
                              file->getLastModificationDate(),
                              logHex);
        deviceHandler->flushAllInputBuffers ();
        writePacket (initialPacket);
        logger->info ("Wrote header packet");
    }
}

template<int payloadSize>
void YModemSender<payloadSize>::executeState (const unsigned int t_currentState, bool logHex)
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
            xModem.transmit (files.back (),
                             updateCallback,
                             yieldCallback,
                             logHex);
            files.pop_back ();
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

template<int payloadSize>
void YModemSender<payloadSize>::transmit (
    const std::vector<std::shared_ptr<File>>& files_,
    std::function<void (float)> updateCallback_,
    std::function<bool ()> yieldCallback_,
    const bool logHex)
{
    assert (!files_.empty ());

    // Preparing for new transfer
    guards.clear ();
    files = files_;
    updateCallback = std::move (updateCallback_);
    yieldCallback = std::move (yieldCallback_);
    deviceHandler->flushAllInputBuffers ();
    //

    logger->info ("Beginning transmission");
    while (!isTransmissionFinished)
    {
        if (!yieldCallback ())
        {
            if (deviceHandler->available () > 0)
            {
                deviceHandler->readAll ();
                logger->debug ("Device input buffer size " + std::to_string(deviceHandler->available ()));
                logger->debug ("Local buffer size " + std::to_string(deviceHandler->getInputBufferSize ()));
                logger->debug ("Printing input buffer");
                deviceHandler->showBuffer ();
                logger->debug ("Buffer front : "  + std::to_string(deviceHandler->getInputBufferFront ()));
                const auto characterReceived = deviceHandler->readNextByte ();
                currentState = getNextState (characterReceived,
                                             currentState,
                                             undefined,
                                             stateTransitions);
                logger->debug ("Current state " + std::to_string(currentState));
                executeState (currentState, logHex);
                if (currentState != xModemTransmission)
                {
                    if (!deviceHandler->waitReadable ())
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
}

template class YModemSender<xyModemConst::payloadSize1K>;
template class YModemSender<xyModemConst::payloadSize128>;

} //namespace xymodem
