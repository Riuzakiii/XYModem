#include "Devices/SerialHandler.h"
#ifdef _WIN32
#include <chrono>
#include <thread>
using namespace std::literals::chrono_literals;
#endif

namespace xymodem
{
SerialHandler::SerialHandler (serial::Serial& serialDevice_)
    : serialDevice (serialDevice_)
{
}

size_t SerialHandler::write (const uint8_t* data, size_t size)
{
    return serialDevice.write (data, size);
}

void SerialHandler::readAll()
{
    /** Solves a small problem : C character can be received during writing, but
     * we don't care about characters before ACK if there is one
     */
    std::vector<uint8_t> tempBuffer;
    serialDevice.read (tempBuffer, serialDevice.available());
    inputBuffer = tempBuffer;
}
void SerialHandler::flushDeviceInputBuffer()
{
    std::vector<uint8_t> tempBuffer;
    serialDevice.read (tempBuffer, serialDevice.available());
}

size_t SerialHandler::available() const { return serialDevice.available(); }

bool SerialHandler::waitReadable() const
{
#ifndef _WIN32
    return serialDevice.waitReadable();
#else
    constexpr auto timeout = 30s;
    constexpr auto sleepForMax = 10ms;
    auto sleepFor = 2ms;
    auto startTime = std::chrono::steady_clock::now();
    while ((std::chrono::steady_clock::now() - startTime) < timeout)
    {
        if (serialDevice.available() > 0)
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
