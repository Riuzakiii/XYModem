#include "gtest/gtest.h"
#include "Devices/RingBuffer.h"
#include "Devices/FakeSerialHandler.h"
#include <vector>
#include <algorithm>

TEST(TestFakeSerialHandler, TestConnection)
{
    xymodem::RingBuffer<uint8_t, 1024> deviceBuffer1;
    xymodem::RingBuffer<uint8_t, 1024> deviceBuffer2;

    xymodem::FakeSerialHandler<1024> device1(deviceBuffer1, deviceBuffer2);
    xymodem::FakeSerialHandler<1024> device2(deviceBuffer2, deviceBuffer1);

    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6};

    device1.write(data.data(), data.size());

    auto result = device2.read(data.size());

    EXPECT_TRUE(std::equal(data.begin(), data.end(), result.begin()));

}


