#include "Devices/RingBuffer.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <vector>

TEST (TestRingBuffer, BasicTest)
{
    xymodem::RingBuffer<int, 10> buffer;
    std::vector<int> data = {1, 2, 3, 4, 5, 6};

    buffer.write (data);
    auto result = buffer.read (data.size());

    EXPECT_TRUE (std::equal (data.begin(), data.end(), result.begin()));
}

TEST (TestRingBuffer, TestSeveralWrites)
{
    xymodem::RingBuffer<int, 10> buffer;
    std::vector<int> data = {1, 2, 3, 4, 5, 6};
    std::vector<int> data1 = {7, 7, 7, 7, 7, 7};

    buffer.write (data);
    auto result = buffer.read (data.size());
    buffer.write (data1);
    result = buffer.read (data1.size());

    EXPECT_TRUE (std::equal (data1.begin(), data1.end(), result.begin()));
}

TEST (TestRingBuffer, TestOverflow)
{
    xymodem::RingBuffer<int, 10> buffer;
    std::vector<int> data = {1, 2, 3, 4, 5, 6};
    std::vector<int> data1 = {7, 7, 7, 7, 7, 7};

    buffer.write (data);
    buffer.write (data1);
    auto result = buffer.read (10);

    EXPECT_TRUE (std::equal (data.begin(), data.end(), result.begin()));
}
