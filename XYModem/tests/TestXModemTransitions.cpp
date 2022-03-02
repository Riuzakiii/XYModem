#include "gtest/gtest.h"
#include "serial/serial.h"
#include "Devices/SerialHandler.h"
#include "XYModem.h"

namespace xymodem
{
class XModemTest : public ::testing::Test
{
public:
    XModemTest () : serialHandler (serialDevice), xModem (serialHandler) {}

    serial::Serial serialDevice;
    SerialHandler serialHandler;
    XModem xModem;
};

TEST_F (XModemTest, TestBeginXModem)
{
    EXPECT_EQ (xModem.getNextState (xyModemConst::C,
                                    XModem::waitingStart,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::sendingPacket);
}

TEST_F (XModemTest, TestSendNextPacket)
{
    xModem.guards.set (XModem::packetsLeft, 2);
    EXPECT_EQ (xModem.getNextState (xyModemConst::ACK,
                                    XModem::sendingPacket,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::sendingPacket);
}

TEST_F (XModemTest, TestSendNextPacketButNoPacketsLeft)
{
    xModem.guards.set (XModem::packetsLeft, 0);
    EXPECT_EQ (xModem.getNextState (xyModemConst::ACK,
                                    XModem::sendingPacket,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::endOfTransmission);
}

TEST_F (XModemTest, TestRetryingButReachedRetriesLimit)
{
    xModem.guards.set (XModem::retries, 11);
    EXPECT_EQ (xModem.getNextState (xyModemConst::NAK,
                                    XModem::retryingPacket,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::abort);
}

TEST_F (XModemTest, TestRetryingPacket)
{
    xModem.guards.set (XModem::retries, 0);
    EXPECT_EQ (xModem.getNextState (xyModemConst::NAK,
                                    XModem::sendingPacket,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::retryingPacket);
}
TEST_F (XModemTest, TestReretryingPacket)
{
    xModem.guards.set (XModem::retries, 5);
    EXPECT_EQ (xModem.getNextState (xyModemConst::NAK,
                                    XModem::retryingPacket,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::retryingPacket);
}

TEST_F (XModemTest, TestEOTButNAKReceived)
{
    xModem.guards.set (XModem::retries, 0);
    EXPECT_EQ (xModem.getNextState (xyModemConst::NAK,
                                    XModem::endOfTransmission,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::retryingEOT);
}

TEST_F (XModemTest, TestEOTButNAKReceivedAndNoRetriesLeft)
{
    xModem.guards.set (XModem::retries, 11);
    EXPECT_EQ (xModem.getNextState (xyModemConst::NAK,
                                    XModem::retryingEOT,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::abort);
}

TEST_F (XModemTest, TestUnknownCharacterReceived)
{
    EXPECT_EQ (xModem.getNextState (23,
                                    XModem::sendingPacket,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::undefined);
}

TEST_F (XModemTest, TestTransmissionFinished)
{
    EXPECT_EQ (xModem.getNextState (xyModemConst::ACK,
                                    XModem::endOfTransmission,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::transmissionFinished);
}

TEST_F (XModemTest, TestTransmissionFinishedAfterEOTRetry)
{
    EXPECT_EQ (xModem.getNextState (xyModemConst::ACK,
                                    XModem::retryingEOT,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::transmissionFinished);
}

TEST_F (XModemTest, TestSendingPacketButCAN)
{
    EXPECT_EQ (xModem.getNextState (xyModemConst::CAN,
                                    XModem::sendingPacket,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::abort);
}

TEST_F (XModemTest, TestSendEOTButCAN)
{
    EXPECT_EQ (xModem.getNextState (xyModemConst::CAN,
                                    XModem::endOfTransmission,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::abort);
}

TEST_F (XModemTest, TestRetryingPacketButCAN)
{
    EXPECT_EQ (xModem.getNextState (xyModemConst::CAN,
                                    XModem::retryingPacket,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::abort);
}

TEST_F (XModemTest, TestRetryingEOTButCAN)
{
    EXPECT_EQ (xModem.getNextState (xyModemConst::CAN,
                                    XModem::retryingEOT,
                                    XModem::undefined,
                                    XModem::stateTransitions),
               XModem::abort);
}
} // namespace xymodem
