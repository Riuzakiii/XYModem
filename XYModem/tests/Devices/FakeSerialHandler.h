#pragma once
#include "Devices/DeviceHandler.h"
#include "RingBuffer.h"
#include <chrono>
#include <thread>
using namespace std::literals::chrono_literals;

namespace xymodem
{

template<int bufferSize>
class FakeSerialHandler : public DeviceHandler
{
public:
    FakeSerialHandler(RingBuffer<uint8_t, bufferSize>& inputBuffer, RingBuffer<uint8_t, bufferSize>& outputBuffer);

    size_t write (const uint8_t* data, size_t size) override;

    void readAll() override;

    [[nodiscard]] std::vector<uint8_t> read(int n);

    void flushDeviceInputBuffer() override;

    [[nodiscard]] size_t available() const override;

    bool waitReadable() const override;
private:
    RingBuffer<uint8_t, bufferSize>& inputBuffer;
    RingBuffer<uint8_t, bufferSize>& outputBuffer;

};

template<int bufferSize>
FakeSerialHandler<bufferSize>::FakeSerialHandler(RingBuffer<uint8_t, bufferSize>& inputBuffer, RingBuffer<uint8_t, bufferSize>& outputBuffer) : inputBuffer(inputBuffer), outputBuffer(outputBuffer)
{

}

template<int bufferSize>
size_t FakeSerialHandler<bufferSize>::write (const uint8_t* data, size_t size)
{
   size_t written = 0;
   for(size_t i = 0; i < size; ++i)
   {
        if(outputBuffer.write({data[i]}) != 1)
        {
            break;
        }
   }
   return written+1;
}

template<int bufferSize>
void FakeSerialHandler<bufferSize>::readAll()
{
    auto read = inputBuffer.read();
}

template<int bufferSize>
std::vector<uint8_t> FakeSerialHandler<bufferSize>::read(int n)
{
    return inputBuffer.read(n);
}

template<int bufferSize>
void FakeSerialHandler<bufferSize>::flushDeviceInputBuffer()
{

}

template<int bufferSize>
[[nodiscard]] size_t FakeSerialHandler<bufferSize>::available() const
{
    return inputBuffer.available();
}

template<int bufferSize>
bool FakeSerialHandler<bufferSize>::waitReadable() const
{
    constexpr auto timeout = 30s;
    constexpr auto sleepForMax = 10ms;
    auto sleepFor = 2ms;
    auto startTime = std::chrono::steady_clock::now();
    while ((std::chrono::steady_clock::now() - startTime) < timeout)
    {
        if (inputBuffer.available() > 0)
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
}

}
