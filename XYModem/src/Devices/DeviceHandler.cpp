#include "Devices/DeviceHandler.h"

namespace xymodem
{
/**
 * Device handler
 */

std::string DeviceHandler::showBuffer ()
{
    std::string result;
    for (const auto& byte : inputBuffer)
    {
        result += fmt::format ("{:>5}", byte);
    }
    return result;
}

uint8_t DeviceHandler::readNextByte ()
{
    uint8_t byte = inputBuffer.front ();
    inputBuffer.erase (inputBuffer.begin ());
    return byte;
}

void DeviceHandler::flushLocalBuffer () { inputBuffer.clear (); }

void DeviceHandler::flushAllInputBuffers ()
{
    flushDeviceInputBuffer ();
    flushLocalBuffer ();
}
size_t DeviceHandler::getInputBufferSize () { return inputBuffer.size (); }

bool DeviceHandler::isInputBufferEmpty () const { return inputBuffer.empty (); }

uint8_t DeviceHandler::getInputBufferFront () const
{
    return inputBuffer.front ();
}
} // namespace xymodem
