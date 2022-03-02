#include "gtest/gtest.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include "XYModem.h"
#include "Tools.h"


TEST (TestTools, TestDispByteArray)
{
    EXPECT_EQ (
        tools::dispByteArray<8> ({255, 255, 255, 255, 255, 255, 255, 255}),
        "0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff");
}

TEST (TestTools, TestIncreasePacketNum)
{
    uint8_t test1 = 255;
    uint8_t test2 = 43;

    tools::increasePacketNum (test1);
    tools::increasePacketNum (test2);

    EXPECT_EQ (test1, 0);
    EXPECT_EQ (test2, 44);
}

TEST (TestTools, TestDecreasePacketNum)
{
    uint8_t test1 = 0;
    uint8_t test2 = 43;

    tools::decreasePacketNum (test1);
    tools::decreasePacketNum (test2);

    EXPECT_EQ (test1, 255);
    EXPECT_EQ (test2, 42);
}
