// Copyright 2022 Riuzakiii

#include "gtest/gtest.h"
#include "Receivers/XModemReceiver.h"
#include "Senders/XModemSender.h"
#include "Loggers/Logger.h"
#include "Devices/RingBuffer.h"
#include "Devices/FakeSerialHandler.h"
#include "Files/DesktopFile.h"
#include <thread>

namespace xymodem
{
  class XModemReceiverTest : public ::testing::Test
  {
  public:
    XModemReceiverTest() : device1(std::make_shared<FakeSerialHandler<1024>>(deviceBuffer1, deviceBuffer2)), device2(std::make_shared<FakeSerialHandler<1024>>(deviceBuffer2, deviceBuffer1)), file(std::make_shared<DesktopFile>("/home/thiebaud/Documents/Dev/XYModem/XYModem/tests/testReceiver.txt")), fileDestination(std::make_shared<DesktopFile>("/home/thiebaud/Documents/Dev/XYModem/XYModem/tests/testReceiverDestination.txt")), logger(std::make_shared<Logger>())
    {

    }

    RingBuffer<uint8_t, 1024> deviceBuffer1;
    RingBuffer<uint8_t, 1024> deviceBuffer2;

    std::shared_ptr<FakeSerialHandler<1024>> device1;
    std::shared_ptr<FakeSerialHandler<1024>> device2;
    std::shared_ptr<DesktopFile> file;
    std::shared_ptr<DesktopFile> fileDestination;

    std::shared_ptr<Logger> logger;
    XModemReceiver receiver{device1, logger};
    XModemSender<> sender{device2, logger};
  };

TEST_F(XModemReceiverTest, TestTransmission)
{
    std::thread t1([this](){sender.transmit(file);});
    std::thread t2([this](){receiver.receive(fileDestination);});

    t1.join();
    t2.join();

    EXPECT_TRUE(true);
}
}
