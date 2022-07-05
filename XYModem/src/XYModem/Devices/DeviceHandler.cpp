// Copyright 2022 Riuzakiii

#include "Devices/DeviceHandler.h"
#include <cstdint> // for uint8_t
#include <iomanip>
#include <ios>
#include <sstream>

namespace xymodem
{
/**
 * Device handler
 */

std::string DeviceHandler::showBuffer()
{
    std::stringstream result;

    for (auto it = inputBuffer.begin(); it != inputBuffer.end(); ++it)
    {
        result << "0x" << std::setfill ('0') << std::setw (2) << std::hex << std::noshowbase << static_cast<int> (*it);
        if (it != (inputBuffer.end() - 1))
        {
            result << ",";
        }
    }
    return result.str();
}

uint8_t DeviceHandler::readNextByte()
{
    uint8_t byte = inputBuffer.front();
    inputBuffer.erase (inputBuffer.begin());
    return byte;
}

void DeviceHandler::flushLocalBuffer() { inputBuffer.clear(); }

void DeviceHandler::flushAllInputBuffers()
{
    flushDeviceInputBuffer();
    flushLocalBuffer();
}
size_t DeviceHandler::getInputBufferSize() { return inputBuffer.size(); }

bool DeviceHandler::isInputBufferEmpty() const { return inputBuffer.empty(); }

uint8_t DeviceHandler::getInputBufferFront() const { return inputBuffer.front(); }
} // namespace xymodem
