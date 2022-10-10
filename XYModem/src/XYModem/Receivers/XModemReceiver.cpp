#include "Receivers/XModemReceiver.h"

namespace xymodem
{
XModemReceiver::XModemReceiver(std::shared_ptr<DeviceHandler> device, std::shared_ptr<Logger> logger) : FileTransferProtocol(device, sendStartWithCRCSignal, logger)
{

}

void XModemReceiver::executeState (const unsigned int currentState, bool logHex)
{

}

void XModemReceiver::receive (std::shared_ptr<File> destination)
{

}

}
