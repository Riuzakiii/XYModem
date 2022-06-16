// Copyright 2022 Riuzakiii

#include "Devices/SerialHandler.h"
#include "XYModem.h"
#include "serial/serial.h"
#include "gtest/gtest.h"

namespace xymodem
{
class YModemTest : public testing::Test
{
public:
    YModemTest() : deviceHandler (std::make_shared<SerialHandler> (serialDevice)), yModem (deviceHandler) {}

    [[nodiscard]] auto getYModem() const -> const auto& { return yModem; }
    [[nodiscard]] auto getYModem() -> auto& { return yModem; }

private:
    serial::Serial serialDevice;
    std::shared_ptr<SerialHandler> deviceHandler;
    YModemSender<xymodem::payloadSize1K> yModem;
};

TEST_F (YModemTest, TestBeginYModem)
{
    EXPECT_EQ (getYModem().getNextState (xymodem::C, YModemSender<>::waitingStart, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::sendingHeader);
}

TEST_F (YModemTest, TestBeginXModemTransmission)
{
    EXPECT_EQ (getYModem().getNextState (xymodem::ACK, YModemSender<>::sendingHeader, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::xModemTransmission);
}

TEST_F (YModemTest, TestRetryingHeader)
{
    EXPECT_EQ (getYModem().getNextState (xymodem::NAK, YModemSender<>::sendingHeader, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::retryingHeader);
}

TEST_F (YModemTest, TestReretryingHeader)
{
    getYModem().guards.set (YModemSender<>::retries, 2);
    EXPECT_EQ (getYModem().getNextState (xymodem::NAK, YModemSender<>::retryingHeader, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::retryingHeader);
}

TEST_F (YModemTest, TestTooManyRetries)
{
    getYModem().guards.set (YModemSender<>::retries, 11);
    EXPECT_EQ (getYModem().getNextState (xymodem::NAK, YModemSender<>::retryingHeader, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::abort);
}

TEST_F (YModemTest, TestUnknownCharacterReceived)
{
    EXPECT_EQ (getYModem().getNextState (23, YModemSender<>::sendingHeader, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::undefined);
    EXPECT_EQ (getYModem().getNextState (23, YModemSender<>::retryingHeader, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::undefined);
}

TEST_F (YModemTest, TestWaitingStartButCAN)
{
    EXPECT_EQ (getYModem().getNextState (xymodem::CAN, YModemSender<>::waitingStart, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::abort);
}

TEST_F (YModemTest, TestSendingHeaderButCAN)
{
    EXPECT_EQ (getYModem().getNextState (xymodem::CAN, YModemSender<>::sendingHeader, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::abort);
}

TEST_F (YModemTest, TestRetryingHeaderButCAN)
{
    EXPECT_EQ (getYModem().getNextState (xymodem::CAN, YModemSender<>::sendingHeader, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::abort);
}

TEST_F (YModemTest, TestXModemTransmissionButCAN)
{
    EXPECT_EQ (getYModem().getNextState (xymodem::CAN, YModemSender<>::xModemTransmission, YModemSender<>::undefined, YModemSender<>::stateTransitions),
               YModemSender<>::abort);
}
} // namespace xymodem
