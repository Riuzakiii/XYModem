#include "Devices/SerialHandler.h"
#include "Tools.h"
#include "XYModem.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include "serial/serial.h"
#include "gtest/gtest.h"

namespace xymodem
{
class TestXYModemHelper : public ::testing::Test
{
public:
    TestXYModemHelper() : deviceHandler (std::make_shared<SerialHandler> (serialDevice)), xModem (deviceHandler), yModem (deviceHandler) {}

    [[nodiscard]] const auto& getXModem() const { return xModem; }
    [[nodiscard]] auto& getXModem() { return xModem; }

    [[nodiscard]] const auto& getYModem() const { return yModem; }
    [[nodiscard]] auto& getYModem() { return yModem; }

    [[nodiscard]] const auto& getDeviceHandler() const { return deviceHandler; }
    [[nodiscard]] auto& getDeviceHandler() { return deviceHandler; }

private:
    serial::Serial serialDevice;
    std::shared_ptr<SerialHandler> deviceHandler;
    XModemSender<xymodem::payloadSize1K> xModem;
    YModemSender<xymodem::payloadSize1K> yModem;
};

TEST_F (TestXYModemHelper, TestMakeDataPacket)
{
    std::string expectedData;
    for (std::size_t i = 0; i < xymodem::payloadSize1K; ++i)
    {
        if (i == (xymodem::payloadSize1K - 1))
        {
            expectedData += "0xff";
        }
        else
        {
            expectedData += "0xff,";
        }
    }

    std::string data;

    const auto dataPacket = getXModem().makeDataPacket (data, 5, false);
    std::string packetId = "0x02,0x05,0xfa";
    std::string crc = "0xc0,0x84";
    EXPECT_EQ (xymodem::tools::dispByteArray (dataPacket), fmt::format ("{:s},{:s},{:s}", packetId, expectedData, crc));
}

TEST_F (TestXYModemHelper, TestFlushLocalBuffer)
{
    getDeviceHandler()->inputBuffer.emplace_back (static_cast<uint8_t> (0xff));
    getDeviceHandler()->inputBuffer.emplace_back (static_cast<uint8_t> (0xff));
    getDeviceHandler()->flushLocalBuffer();
    EXPECT_EQ (getDeviceHandler()->inputBuffer.empty(), true);
}

TEST_F (TestXYModemHelper, TestReadNextByte)
{
    getDeviceHandler()->inputBuffer.emplace_back (static_cast<uint8_t> (0xa5));
    EXPECT_EQ (getDeviceHandler()->readNextByte(), 0xa5);
}

TEST_F (TestXYModemHelper, TestMakeHeaderPacket)
{
    const auto headerPacket = getYModem().makeHeaderPacket ("test", 0, 0);
    std::string packetId = "0x02,0x00,0xff";
    std::string header = "0x74,0x65,0x73,0x74,0x00,0x30,0x20,0x30";
    constexpr auto sizeHeaderBytes = 8;
    std::array<uint8_t, xymodem::payloadSize1K - sizeHeaderBytes> packet = {0x00};
    std::string crc = "0xa3,0x72";
    EXPECT_EQ (xymodem::tools::dispByteArray (headerPacket), fmt::format ("{:s},{:s},{:#04x},{:s}", packetId, header, fmt::join (packet, ","), crc));
}

TEST_F (TestXYModemHelper, TestMakeLastPacket)
{
    const auto lastPacket = getYModem().makeLastPacket();
    std::string packetId = "0x02,0x00,0xff";
    std::array<uint8_t, xymodem::payloadSize1K> packet = {0x00};
    std::string crc = "0x00,0x00";
    EXPECT_EQ (xymodem::tools::dispByteArray (lastPacket), fmt::format ("{:s},{:#04x},{:s}", packetId, fmt::join (packet, ","), crc));
}
} // namespace xymodem
