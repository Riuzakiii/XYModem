﻿#include "Senders/XModemSender.h"

namespace xymodem
{
/**
 * XModem sender
 */
XModemSender::XModemSender (std::shared_ptr<DeviceHandler> deviceHandler_, std::shared_ptr<Logger> logger)
    : FileTransferProtocol (
          deviceHandler_, waitingStart, logger)
{
    guards.addGuards ({{retries, 0}, {packetsLeft, 0}});
};

Packet XModemSender::makeDataPacket (const std::string& data_,
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


void XModemSender::writePacket (Packet packet)
{
    deviceHandler->flushAllInputBuffers ();
    deviceHandler->write (packet.data (), packet.size ());
}


void XModemSender::executeSendPacket (const bool logHex)
{
    if (guards.get (packetsLeft) == 1)
    {

        logger->debug (std::to_string(packetNum));
        const auto packet =
            makeDataPacket (file->getNextFileBlock (
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
            file->getNextFileBlock (xyModemConst::packetDataSize),
            packetNum,
            logHex);
        writePacket (packet);
        logger->debug ("Packets left " + std::to_string(guards.get (packetsLeft)));
    }
    tools::increasePacketNum (packetNum);
    guards.dec (packetsLeft);
    assert (guards.get (packetsLeft) >= 0);
    updateCallback (
        static_cast<float> (totalPackets - guards.get (packetsLeft)) /
        static_cast<float> (totalPackets));
    logger->debug (
        "Sent"
        + std::to_string(static_cast<float> (totalPackets - guards.get (packetsLeft)) /
            static_cast<float> (totalPackets) * 100) + "\\% packets");
}

void XModemSender::executeState (const unsigned int state, bool logHex)
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
        deviceHandler->flushAllInputBuffers ();
        deviceHandler->write (buffer, 1);
        logger->debug ("Sent EOT");
        break;
    }
    case retryingEOT:
    {
        guards.inc (retries);
        const uint8_t buffer[1] = {xyModemConst::EOT};
        deviceHandler->flushAllInputBuffers ();
        deviceHandler->write (buffer, 1);
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

void XModemSender::transmit (const std::shared_ptr<File>& file_,
                       std::function<void (float)> updateCallback_,
                       std::function<bool ()> yieldCallback_,
                       const bool logHex)
{
    file = file_;
    guards.set (retries, 0);
    updateCallback = std::move (updateCallback_);
    yieldCallback = std::move (yieldCallback_);

    //// Prepare all required information
    file->open();
    if (!file->isOpened())
    {
        logger->error ("Could not open file located named " + file->getFilename());
        throw (XYModemExceptions::CouldNotOpenFile (file->getFilename()));
    }
    ////

    //// Compute the number of packets to send
    const auto divNumPacket =
        std::lldiv (file->getFilesize(),
                    static_cast<std::intmax_t> (xyModemConst::packetDataSize));

    lastFullPacket = divNumPacket.quot;     // Number of fullsize packets
    lastPacketRemaining = divNumPacket.rem; // Remaining bytes
    logger->debug ("lastfullpacket "+ std::to_string(lastFullPacket));
    logger->debug ("remaining " + std::to_string(lastPacketRemaining));
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
    file->erase();
    deviceHandler->flushAllInputBuffers ();

    logger->info ("Beginning transmission");
    auto timerStart = std::chrono::steady_clock::now ();
    while (!isTransmissionFinished)
    {
        if (!yieldCallback ())
        {
            if (deviceHandler->waitReadable ())
            {
                deviceHandler->readAll ();
                logger->debug ("Device input buffer size" + std::to_string(deviceHandler->available ()));
                logger->debug ("Local buffer size " + std::to_string(deviceHandler->getInputBufferSize ()));
                logger->debug ("Printing input buffer");
                deviceHandler->showBuffer ();
                logger->debug ("Buffer front "+ std::to_string( deviceHandler->getInputBufferFront ()));

                const auto characterReceived = deviceHandler->readNextByte ();
                currentState = getNextState (characterReceived,
                                             currentState,
                                             undefined,
                                             stateTransitions);
                logger->debug ("Current state " + std::to_string(currentState));
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
    file->close();
}
}