#include "gtest/gtest.h"
#include "serial/serial.h"
#include "Devices/SerialHandler.h"
#include "XYModem.h"

namespace xymodem
{
class XModemTest : public ::testing::Test
{
public:
    XModemTest () : deviceHandler (std::make_shared<SerialHandler>(serialDevice)), xModem (deviceHandler) {}

    serial::Serial serialDevice;
    std::shared_ptr<SerialHandler> deviceHandler;
    XModemSender<xymodem::payloadSize1K> xModem;
};

TEST_F (XModemTest, TestBeginXModem)
{
    EXPECT_EQ (xModem.getNextState (xymodem::C,
                                    XModemSender<>::waitingStart,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::sendingPacket);
}

TEST_F (XModemTest, TestSendNextPacket)
{
    xModem.guards.set (XModemSender<>::packetsLeft, 2);
    EXPECT_EQ (xModem.getNextState (xymodem::ACK,
                                    XModemSender<>::sendingPacket,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::sendingPacket);
}

TEST_F (XModemTest, TestSendNextPacketButNoPacketsLeft)
{
    xModem.guards.set (XModemSender<>::packetsLeft, 0);
    EXPECT_EQ (xModem.getNextState (xymodem::ACK,
                                    XModemSender<>::sendingPacket,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::endOfTransmission);
}

TEST_F (XModemTest, TestRetryingButReachedRetriesLimit)
{
    xModem.guards.set (XModemSender<>::retries, 11);
    EXPECT_EQ (xModem.getNextState (xymodem::NAK,
                                    XModemSender<>::retryingPacket,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestRetryingPacket)
{
    xModem.guards.set (XModemSender<>::retries, 0);
    EXPECT_EQ (xModem.getNextState (xymodem::NAK,
                                    XModemSender<>::sendingPacket,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::retryingPacket);
}
TEST_F (XModemTest, TestReretryingPacket)
{
    xModem.guards.set (XModemSender<>::retries, 5);
    EXPECT_EQ (xModem.getNextState (xymodem::NAK,
                                    XModemSender<>::retryingPacket,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::retryingPacket);
}

TEST_F (XModemTest, TestEOTButNAKReceived)
{
    xModem.guards.set (XModemSender<>::retries, 0);
    EXPECT_EQ (xModem.getNextState (xymodem::NAK,
                                    XModemSender<>::endOfTransmission,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::retryingEOT);
}

TEST_F (XModemTest, TestEOTButNAKReceivedAndNoRetriesLeft)
{
    xModem.guards.set (XModemSender<>::retries, 11);
    EXPECT_EQ (xModem.getNextState (xymodem::NAK,
                                    XModemSender<>::retryingEOT,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestUnknownCharacterReceived)
{
    EXPECT_EQ (xModem.getNextState (23,
                                    XModemSender<>::sendingPacket,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::undefined);
}

TEST_F (XModemTest, TestTransmissionFinished)
{
    EXPECT_EQ (xModem.getNextState (xymodem::ACK,
                                    XModemSender<>::endOfTransmission,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::transmissionFinished);
}

TEST_F (XModemTest, TestTransmissionFinishedAfterEOTRetry)
{
    EXPECT_EQ (xModem.getNextState (xymodem::ACK,
                                    XModemSender<>::retryingEOT,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::transmissionFinished);
}

TEST_F (XModemTest, TestSendingPacketButCAN)
{
    EXPECT_EQ (xModem.getNextState (xymodem::CAN,
                                    XModemSender<>::sendingPacket,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestSendEOTButCAN)
{
    EXPECT_EQ (xModem.getNextState (xymodem::CAN,
                                    XModemSender<>::endOfTransmission,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestRetryingPacketButCAN)
{
    EXPECT_EQ (xModem.getNextState (xymodem::CAN,
                                    XModemSender<>::retryingPacket,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}

TEST_F (XModemTest, TestRetryingEOTButCAN)
{
    EXPECT_EQ (xModem.getNextState (xymodem::CAN,
                                    XModemSender<>::retryingEOT,
                                    XModemSender<>::undefined,
                                    XModemSender<>::stateTransitions),
               XModemSender<>::abort);
}
} // namespace xymodem
