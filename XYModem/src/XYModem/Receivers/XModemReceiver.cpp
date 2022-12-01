#include "Receivers/XModemReceiver.h"

namespace xymodem
{
XModemReceiver::XModemReceiver (std::shared_ptr<DeviceHandler> device, std::shared_ptr<Logger> logger) :
    FileTransferProtocol (device, sendStartWithCRCSignal, logger), device (device), logger (logger)
{
    guards.addGuards ({
        {retriesStart, 0},
        {retries,      0}
    });
}

void XModemReceiver::executeState (const unsigned int currentState, bool logHex)
{
    switch (currentState)
    {
    case sendStartWithCRCSignal:
        device->write (&xymodem::C, 1);
        time = std::chrono::system_clock::now();
        logger->info ("Sending C");
        guards.inc (retriesStart);
        break;
    case waitForPacket:
        break;
    case processPacket:
        logger->info ("Processing packet");
        file->append (device->read (payloadSize1K + totalExtraSize));
        file->close();
        break;
    default:
        isTransmissionFinished = true;
        break;
    }
}

void XModemReceiver::receive (const std::shared_ptr<File>& destination)
{
    file = destination;

    //// Prepare all required information
    file->create();
    file->open();
    if (!file->isOpened())
    {
        logger->error ("Could not open file located named " + file->getFilename());
        throw (XYModemExceptions::CouldNotOpenFile (file->getFilename()));
    }
    file->erase();
    ////

    logger->info ("Beginning reception");

    while (!isTransmissionFinished)
    {
        eventType currentEvent = noEvent;

        auto now = std::chrono::system_clock::now();
        auto timeoutdiff = std::chrono::duration_cast<std::chrono::seconds> (now - time);

        if (currentState == sendStartWithCRCSignal)
        {
            if (timeoutdiff > 3s)
            {
                currentEvent = timeout3s;
                time = now;
            }
        }

        if (device->available() >= (payloadSize1K + totalExtraSize))
        {
            currentEvent = receivedPayload;
        }
        else if (device->available() > 0 && device->available() < (payloadSize1K + totalExtraSize))
        {
            currentEvent = receveidSomething;
        }

        if (currentEvent != noEvent)
        {
            logger->debug ("Device input buffer size" + std::to_string (deviceHandler->available()));
            currentState = getNextState (currentEvent, currentState, undefined, stateTransitions);
            logger->debug ("Current state " + std::to_string (currentState));
            executeState (currentState, true);
        }

        if (currentState != sendStartWithCRCSignal)
        {
            if (!device->waitReadable())
            {
                logger->warn ("Timeout in receiver ... Transmission aborted");
                throw XYModemExceptions::Timeout (xymodem::timeout.count());
            }
        }
    }
}

} // namespace xymodem

//        auto now = std::chrono::system_clock::now();
//        eventType currentEvent = noEvent;

//        auto timeoutdiff = std::chrono::duration_cast<std::chrono::seconds>(now - time);

//        if (currentState == sendStartWithCRCSignal)
//        {
//            if( timeoutdiff > 3s)
//            {
//                currentEvent = timeout3s;
//                time = now;
//            }
//        }

//        if(currentEvent == noEvent)
//        {
//            if(device->available() > 0)
//            {
//                if(device->available() >= (payloadSize1K + totalExtraSize))
//                {
//                    currentEvent = receivedPayload;
//                }
//                else
//                {
//                    currentEvent = receveidSomething;
//                }
//            }

//        }

//        if(currentEvent != noEvent)
//        {
//            logger->debug ("Device input buffer size" + std::to_string (deviceHandler->available()));
//            currentState = getNextState (currentEvent, currentState, undefined, stateTransitions);
//            logger->debug ("Current state " + std::to_string (currentState));
//            executeState (currentState, true);

//            if(currentState != sendStartWithCRCSignal)
//            {
//                if(!device->waitReadable())
//                {
//                    logger->warn ("Timeout ... Transmission aborted");
//                    throw XYModemExceptions::Timeout (xymodem::timeout.count());
//                }

//            }
//        }
