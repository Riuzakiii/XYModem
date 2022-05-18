#include "gtest/gtest.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include "Devices/SerialHandler.h"
#include "serial/serial.h"
#include "XYModem.h"
#include "Tools.h"

namespace xymodem
{
class TestXYModemHelper : public ::testing::Test
{
public:
    TestXYModemHelper ()
        : deviceHandler (std::make_shared<SerialHandler>(serialDevice)),
          xModem (deviceHandler),
          yModem (deviceHandler)
    {
    }

    serial::Serial serialDevice;
    std::shared_ptr<SerialHandler> deviceHandler;
    XModemSender xModem;
    YModemSender yModem;
};

TEST_F (TestXYModemHelper, TestMakeDataPacket)
{
    std::string expectedData;
    for (int i = 0; i < xyModemConst::packetDataSize; ++i)
    {
        if (i == (xyModemConst::packetDataSize - 1))
        {
            expectedData += "0xff";
        }
        else
        {
            expectedData += "0xff,";
        }
    }

    std::string data;

    const auto dataPacket = xModem.makeDataPacket (data, 5, false);
    std::string id = "0x02,0x05,0xfa";
    std::string crc = "0xc0,0x84";
    EXPECT_EQ (tools::dispByteArray<xyModemConst::packetTotalSize> (dataPacket),
               fmt::format ("{:s},{:s},{:s}", id, expectedData, crc));
}

TEST_F (TestXYModemHelper, TestFlushLocalBuffer)
{
    deviceHandler->inputBuffer.emplace_back (static_cast<uint8_t> (0xff));
    deviceHandler->inputBuffer.emplace_back (static_cast<uint8_t> (0xff));
    deviceHandler->flushLocalBuffer ();
    EXPECT_EQ (deviceHandler->inputBuffer.empty (), true);
}

TEST_F (TestXYModemHelper, TestReadNextByte)
{
    deviceHandler->inputBuffer.emplace_back (static_cast<uint8_t> (0xa5));
    EXPECT_EQ (deviceHandler->readNextByte (), 0xa5);
}

TEST_F (TestXYModemHelper, TestMakeHeaderPacket)
{
    const auto headerPacket = yModem.makeHeaderPacket ("test", 0, 0);
    std::string id = "0x02,0x00,0xff";
    std::string header = "0x74,0x65,0x73,0x74,0x00,0x30,0x20,0x30";
    constexpr auto sizeHeaderBytes = 8;
    std::array<uint8_t, xyModemConst::packetDataSize - sizeHeaderBytes> packet =
        {0x00};
    std::string crc = "0xa3,0x72";
    EXPECT_EQ (
        tools::dispByteArray<xyModemConst::packetTotalSize> (headerPacket),
        fmt::format ("{:s},{:s},{:#04x},{:s}",
                     id,
                     header,
                     fmt::join (packet, ","),
                     crc));
}

TEST_F (TestXYModemHelper, TestMakeLastPacket)
{
    const auto lastPacket = yModem.makeLastPacket ();
    std::string id = "0x02,0x00,0xff";
    std::array<uint8_t, xyModemConst::packetDataSize> packet = {0x00};
    std::string crc = "0x00,0x00";
    EXPECT_EQ (
        tools::dispByteArray<xyModemConst::packetTotalSize> (lastPacket),
        fmt::format ("{:s},{:#04x},{:s}", id, fmt::join (packet, ","), crc));
}
} // namespace xymodem
