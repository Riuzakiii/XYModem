#include "Tools.h"
#include "XYModem.h"
#include "crc_cpp.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <array>
#include <random>

TEST (TestTools, TestDispByteArray)
{
    EXPECT_EQ (xymodem::tools::dispByteArray<8> (
                   { 255, 255, 255, 255, 255, 255, 255, 255 }),
               "0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff");
}

TEST (TestTools, TestIncreasePacketNum)
{
    uint8_t test1 = 255;
    uint8_t test2 = 43;

    xymodem::tools::increasePacketNum (test1);
    xymodem::tools::increasePacketNum (test2);

    EXPECT_EQ (test1, 0);
    EXPECT_EQ (test2, 44);
}

TEST (TestTools, TestDecreasePacketNum)
{
    uint8_t test1 = 0;
    uint8_t test2 = 43;

    xymodem::tools::decreasePacketNum (test1);
    xymodem::tools::decreasePacketNum (test2);

    EXPECT_EQ (test1, 255);
    EXPECT_EQ (test2, 42);
}

std::array<uint8_t, xymodem::payloadSize1K> generateRandomPacket()
{
    std::array<uint8_t, xymodem::payloadSize1K> data{};

    std::random_device randomDevice;
    std::mt19937 gen (randomDevice());
    std::uniform_int_distribution<int> distrib (0, 255);
    std::generate (data.begin(),
                   data.end(),
                   [&distrib, &gen]
                   { return static_cast<uint8_t> (distrib (gen)); });

    return data;
}

TEST (TestTools, TestCRC16)
{
    constexpr auto tries = 10;

    for (int i = 0; i < tries; ++i)
    {
        auto data = generateRandomPacket();
        crc_cpp::crc16_xmodem crc;
        for (auto dataByte : data)
        {
            crc.update (dataByte);
        }

        EXPECT_EQ (xymodem::tools::compute_crc16xmodem (data), crc.final());
    }
}
