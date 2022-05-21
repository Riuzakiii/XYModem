#pragma once
#include "gtest/gtest_prod.h"
#include <array>
#include <string>
#include <algorithm>
#include <cassert>
#include <functional>
#include <string_view>
#include <exception>
#include <utility>
#include "XYModemConst.h"
#include "Tools.h"
#include "Devices/DeviceHandler.h"
#include "FileTransferProtocol.h"

namespace xymodem
{
/** XModem 1K sender, tested with Tera term receiver.
 * @todo end of File
 * @todo setFileInfos
 */
template<std::size_t payloadSize = xymodem::payloadSize1K>
class XModemSender : private FileTransferProtocol
{
public:
    /** Initialise the XModem sender.
     * @param pathToLogFile pathToLogFile must include the filename and
     * extension. w_string on Windows
     * @param logLevel 0(trace), 1(debug), 2(info), 3(warn), 4(error), 5(level
     * critical), 6(off)
     */
    XModemSender (std::shared_ptr<DeviceHandler> deviceHandler_, std::shared_ptr<Logger> logger = std::make_shared<Logger>());

    /** Begin the XModem transmission.
     *  @param fileAbsolutePath The absolute path to the file.
     *  @param updateCallback updateCallback will be called after each packet
     * sent.
     *  @param yieldCallback when yieldCallback returns False, the transmission
     * will abort.
     *  @param logHex if True, the content of the packets sent will be logged in
     * hexadecimal.
     *  @throw XYModemExceptions::Timeout
     *  @throw XYModemExceptions::CouldNotOpenFile
     *  @throw XYModemExceptions::TransmissionAborted
     */
    void transmit (
        const std::shared_ptr<File>& file,
        std::function<void (float)> updateCallback = [] (float) {},
        std::function<bool ()> yieldCallback = [] { return false; },
        const bool logHex = false);

protected:
private:
    /** Create a packet with the given data taken from the file.
     * @param data data that will be sent to the device
     * @param packetNum the number of the packet sent (0-255, goes back to 0
     * after 255)
     * @param logHex if True, the content of the packets sent will be logged in
     * hexadecimal.
     */
    std::array<uint8_t, payloadSize + xymodem::totalExtraSize> makeDataPacket (const std::string& data,
                           const uint8_t& packetNum,
                           const bool logHex = false);

    /** Write packet to the device
     * @param packet The packet to send to the device
     */
    void writePacket (std::array<uint8_t, payloadSize + xymodem::totalExtraSize> packet);

    virtual void executeState (const unsigned int currentState,
                               bool logHex) override;
    void executeSendPacket (bool logHex);

    uint8_t packetNum = 1;
    std::intmax_t lastFullPacket = 0;
    std::intmax_t lastPacketRemaining = 0;
    std::intmax_t totalPackets = 0;

    std::shared_ptr<File> file;

    // XModem state machine constants

    [[maybe_unused]] static constexpr std::string_view retries =
        "XModem number of retries";
    [[maybe_unused]] static constexpr std::string_view packetsLeft =
        "Number of packets left";
    [[maybe_unused]] static constexpr unsigned int waitingStart = 0;
    [[maybe_unused]] static constexpr unsigned int sendingPacket = 1;
    [[maybe_unused]] static constexpr unsigned int retryingPacket = 2;
    [[maybe_unused]] static constexpr unsigned int endOfTransmission = 3;
    [[maybe_unused]] static constexpr unsigned int retryingEOT = 4;
    [[maybe_unused]] static constexpr unsigned int transmissionFinished = 5;
    [[maybe_unused]] static constexpr unsigned int undefined = 6;
    [[maybe_unused]] static constexpr unsigned int abort = 7;
    // clang-format off
    [[maybe_unused]] inline static std::array<transition, 20> stateTransitions =
        {{{waitingStart, sendingPacket, xymodem::C, [] (GuardConditions) { return true; }},
          {sendingPacket, sendingPacket,xymodem::ACK,[] (GuardConditions t_guards){ return t_guards.get (packetsLeft) > 0; }},
          {sendingPacket, retryingPacket, xymodem::NAK, [] (GuardConditions) { return true; }},
          {sendingPacket, endOfTransmission, xymodem::ACK, [] (GuardConditions t_guards) { return t_guards.get (packetsLeft) == 0; }},
          {endOfTransmission, transmissionFinished, xymodem::ACK, [] (GuardConditions) { return true; }},
          {endOfTransmission, retryingEOT, xymodem::NAK, [] (GuardConditions) { return true; }},
          {retryingEOT, transmissionFinished, xymodem::ACK, [] (GuardConditions) { return true; }},
          {retryingEOT, retryingEOT, xymodem::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) <= xymodem::maxRetries; }},
          {retryingEOT,abort,xymodem::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) > xymodem::maxRetries; }},
          {retryingPacket, retryingPacket,xymodem::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) <= xymodem::maxRetries; }},
          {retryingPacket,sendingPacket,xymodem::ACK,[] (GuardConditions) { return true; }},
          {retryingPacket,abort,xymodem::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) > xymodem::maxRetries; }},
          {undefined,sendingPacket,xymodem::C,[] (GuardConditions t_guards){ return t_guards.get (packetsLeft) > 0; }},
          {undefined,retryingPacket,xymodem::NAK,[] (GuardConditions) { return true; }},
          {undefined,abort,xymodem::CAN,[] (GuardConditions) { return true; }},
          {waitingStart,abort,xymodem::CAN,[] (GuardConditions) { return true; }},
          {sendingPacket,abort,xymodem::CAN,[] (GuardConditions) { return true; }},
          {endOfTransmission,abort,xymodem::CAN,[] (GuardConditions) { return true; }},
          {retryingPacket,abort, xymodem::CAN,[] (GuardConditions) { return true; }},
          {retryingEOT, abort, xymodem::CAN, [] (GuardConditions) {return true;}}
          }};
    //clang-format on
    FRIEND_TEST (TestXYModemHelper, TestMakeDataPacket);
    FRIEND_TEST (XModemTest, TestBeginXModem);
    FRIEND_TEST (XModemTest, TestSendNextPacket);
    FRIEND_TEST (XModemTest, TestSendNextPacketButNoPacketsLeft);
    FRIEND_TEST (XModemTest, TestRetryingButReachedRetriesLimit);
    FRIEND_TEST (XModemTest, TestRetryingPacket);
    FRIEND_TEST (XModemTest, TestReretryingPacket);
    FRIEND_TEST (XModemTest, TestTransmissionFinished);
    FRIEND_TEST (XModemTest, TestTransmissionFinishedAfterEOTRetry);
    FRIEND_TEST (XModemTest, TestUnknownCharacterReceived);
    FRIEND_TEST (XModemTest, TestEOTButNAKReceived);
    FRIEND_TEST (XModemTest, TestEOTButNAKReceivedAndNoRetriesLeft);
    FRIEND_TEST (XModemTest, TestSendingPacketButCAN);
    FRIEND_TEST (XModemTest, TestSendEOTButCAN);
    FRIEND_TEST (XModemTest, TestRetryingPacketButCAN);
    FRIEND_TEST (XModemTest, TestRetryingEOTButCAN);
};

}
