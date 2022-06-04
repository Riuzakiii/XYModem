#include <gtest/gtest.h>

int main (int argc, char** argv)
{
    testing::InitGoogleTest (&argc, argv);
    const auto testResult = RUN_ALL_TESTS();
    return testResult;
}
