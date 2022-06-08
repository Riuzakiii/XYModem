#include "Devices/SerialHandler.h"
#include "XYModem.h"
#include "serial/serial.h"
#include "gtest/gtest.h"

namespace xymodem
{
class XModemTest : public ::testing::Test
{
public:
    XModemTest() : deviceHandler (std::make_shared<SerialHandler> (serialDevice)), xModem (deviceHandler) {}

    [[nodiscard]] const auto& getXModem() const { return xModem; }
    [[nodiscard]] auto& getXModem() { return xModem; }

    [[nodiscard]] const auto& getDeviceHandler() const { return deviceHandler; }
    [[nodiscard]] auto& getDeviceHandler() { return deviceHandler; }

private:
    serial::Serial serialDevice;
    std::shared_ptr<SerialHandler> deviceHandler;
    XModemSender<xymodem::payloadSize1K> xModem;
};

TEST_F (XModemTest, TestBeginXModem)
{
    EXPECT_EQ (getXModem().getNextState (xymodem::C, XModemSender<>::waitingStart, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::sendingPacket);
}

TEST_F (XModemTest, TestSendNextPacket)
{
    getXModem().guards.set (XModemSender<>::packetsLeft, 2);
    EXPECT_EQ (getXModem().getNextState (xymodem::ACK, XModemSender<>::sendingPacket, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::sendingPacket);
}

TEST_F (XModemTest, TestSendNextPacketButNoPacketsLeft)
{
    getXModem().guards.set (XModemSender<>::packetsLeft, 0);
    EXPECT_EQ (getXModem().getNextState (xymodem::ACK, XModemSender<>::sendingPacket, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::endOfTransmission);
}

TEST_F (XModemTest, TestRetryingButReachedRetriesLimit)
{
    getXModem().guards.set (XModemSender<>::retries, 11);
    EXPECT_EQ (getXModem().getNextState (xymodem::NAK, XModemSender<>::retryingPacket, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestRetryingPacket)
{
    getXModem().guards.set (XModemSender<>::retries, 0);
    EXPECT_EQ (getXModem().getNextState (xymodem::NAK, XModemSender<>::sendingPacket, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::retryingPacket);
}
TEST_F (XModemTest, TestReretryingPacket)
{
    getXModem().guards.set (XModemSender<>::retries, 5);
    EXPECT_EQ (getXModem().getNextState (xymodem::NAK, XModemSender<>::retryingPacket, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::retryingPacket);
}

TEST_F (XModemTest, TestEOTButNAKReceived)
{
    getXModem().guards.set (XModemSender<>::retries, 0);
    EXPECT_EQ (getXModem().getNextState (xymodem::NAK, XModemSender<>::endOfTransmission, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::retryingEOT);
}

TEST_F (XModemTest, TestEOTButNAKReceivedAndNoRetriesLeft)
{
    getXModem().guards.set (XModemSender<>::retries, 11);
    EXPECT_EQ (getXModem().getNextState (xymodem::NAK, XModemSender<>::retryingEOT, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestUnknownCharacterReceived)
{
    EXPECT_EQ (getXModem().getNextState (23, XModemSender<>::sendingPacket, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::undefined);
}

TEST_F (XModemTest, TestTransmissionFinished)
{
    EXPECT_EQ (getXModem().getNextState (xymodem::ACK, XModemSender<>::endOfTransmission, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::transmissionFinished);
}

TEST_F (XModemTest, TestTransmissionFinishedAfterEOTRetry)
{
    EXPECT_EQ (getXModem().getNextState (xymodem::ACK, XModemSender<>::retryingEOT, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::transmissionFinished);
}

TEST_F (XModemTest, TestSendingPacketButCAN)
{
    EXPECT_EQ (getXModem().getNextState (xymodem::CAN, XModemSender<>::sendingPacket, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestSendEOTButCAN)
{
    EXPECT_EQ (getXModem().getNextState (xymodem::CAN, XModemSender<>::endOfTransmission, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestRetryingPacketButCAN)
{
    EXPECT_EQ (getXModem().getNextState (xymodem::CAN, XModemSender<>::retryingPacket, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestRetryingEOTButCAN)
{
    EXPECT_EQ (getXModem().getNextState (xymodem::CAN, XModemSender<>::retryingEOT, XModemSender<>::undefined, XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}
} // namespace xymodem
