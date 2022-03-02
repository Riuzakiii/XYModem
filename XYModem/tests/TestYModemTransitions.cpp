#include "gtest/gtest.h"
#include "serial/serial.h"
#include "Devices/SerialHandler.h"
#include "XYModem.h"

namespace xymodem
{
class YModemTest : public testing::Test
{
public:
    YModemTest () : serialHandler (serialDevice), yModem (serialHandler){};

    serial::Serial serialDevice;
    SerialHandler serialHandler;
    YModem yModem;
};

TEST_F (YModemTest, TestBeginYModem)
{
    EXPECT_EQ (yModem.getNextState (xyModemConst::C,
                                    YModem::waitingStart,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::sendingHeader);
}

TEST_F (YModemTest, TestBeginXModemTransmission)
{
    EXPECT_EQ (yModem.getNextState (xyModemConst::ACK,
                                    YModem::sendingHeader,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::xModemTransmission);
}

TEST_F (YModemTest, TestRetryingHeader)
{
    EXPECT_EQ (yModem.getNextState (xyModemConst::NAK,
                                    YModem::sendingHeader,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::retryingHeader);
}

TEST_F (YModemTest, TestReretryingHeader)
{
    yModem.guards.set (YModem::retries, 2);
    EXPECT_EQ (yModem.getNextState (xyModemConst::NAK,
                                    YModem::retryingHeader,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::retryingHeader);
}

TEST_F (YModemTest, TestTooManyRetries)
{
    yModem.guards.set (YModem::retries, 11);
    EXPECT_EQ (yModem.getNextState (xyModemConst::NAK,
                                    YModem::retryingHeader,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::abort);
}

TEST_F (YModemTest, TestUnknownCharacterReceived)
{
    EXPECT_EQ (yModem.getNextState (23,
                                    YModem::sendingHeader,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::undefined);
    EXPECT_EQ (yModem.getNextState (23,
                                    YModem::retryingHeader,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::undefined);
}

TEST_F (YModemTest, TestWaitingStartButCAN)
{
    EXPECT_EQ (yModem.getNextState (xyModemConst::CAN,
                                    YModem::waitingStart,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::abort);
}

TEST_F (YModemTest, TestSendingHeaderButCAN)
{
    EXPECT_EQ (yModem.getNextState (xyModemConst::CAN,
                                    YModem::sendingHeader,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::abort);
}

TEST_F (YModemTest, TestRetryingHeaderButCAN)
{
    EXPECT_EQ (yModem.getNextState (xyModemConst::CAN,
                                    YModem::sendingHeader,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::abort);
}

TEST_F (YModemTest, TestXModemTransmissionButCAN)
{
    EXPECT_EQ (yModem.getNextState (xyModemConst::CAN,
                                    YModem::xModemTransmission,
                                    YModem::undefined,
                                    YModem::stateTransitions),
               YModem::abort);
}
} // namespace xymodem
