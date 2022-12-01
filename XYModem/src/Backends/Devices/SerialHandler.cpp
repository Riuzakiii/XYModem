// Copyright 2022 Riuzakiii

#include "Devices/SerialHandler.h"
#ifdef _WIN32
#include <chrono>
#include <thread>
using namespace std::literals::chrono_literals;
#endif
#include "serial/serial.h"
#include <cstdint> // for uint8_t, uint32_t
#include <vector>  // for vector

namespace xymodem
{
SerialHandler::SerialHandler (std::shared_ptr<serial::Serial>& serialDevice_) : serialDevice (serialDevice_) {}

SerialHandler::SerialHandler (uint32_t baudrate,
                              uint32_t interByteTimeout,
                              uint32_t readTimeoutConstant,
                              uint32_t readTimeoutMultiplier,
                              uint32_t writeTimeoutConstant,
                              uint32_t writeTimeoutMultiplier) :
    serialDevice (std::make_shared<serial::Serial>())
{
    serialDevice->setBaudrate (baudrate);
    serialDevice->setTimeout (interByteTimeout, readTimeoutConstant, readTimeoutMultiplier, writeTimeoutConstant, writeTimeoutMultiplier);
}

size_t SerialHandler::write (const uint8_t* data, size_t size) { return serialDevice->write (data, size); }

std::vector<uint8_t> SerialHandler::read (int n)
{
    std::vector<uint8_t> tempBuffer;
    serialDevice->read (tempBuffer, serialDevice->available());
    return tempBuffer;
}

void SerialHandler::flushInputBuffer()
{
    std::vector<uint8_t> tempBuffer;
    serialDevice->read (tempBuffer, serialDevice->available());
}

size_t SerialHandler::available() const { return serialDevice->available(); }

bool SerialHandler::waitReadable() const
{
#ifndef _WIN32
    return serialDevice->waitReadable();
#else
    constexpr auto timeout = 30s;
    constexpr auto sleepForMax = 10ms;
    auto sleepFor = 2ms;
    auto startTime = std::chrono::steady_clock::now();
    while ((std::chrono::steady_clock::now() - startTime) < timeout)
    {
        if (serialDevice->available() > 0)
        {
            return true;
        }
        if (sleepFor < sleepForMax)
        {
            sleepFor *= 2;
        }
        std::this_thread::sleep_for (sleepFor); // exponential backoff
    }
    return false;
#endif
}
} // namespace xymodem
