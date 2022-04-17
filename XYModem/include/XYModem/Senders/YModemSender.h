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
#include "XModemSender.h"

namespace xymodem
{
/** Simple YModem 1K Sender. Tested with Tera Term YModem receiver. Will be able
 * to support YModem 1K batch, but it needs to be implemented.
 * @note there are differences with the YModem protocol
 * reference.
 * 1. The header packet ends with a CRC, which works with TeraTerm but is not in
 * the YModem protocol reference.
 * @see XModem
 * @see FileTransferProtocol
 */
class YModemSender : private FileTransferProtocol
{
public:
    /** Initialise the YModem sender.
     * @param pathToLogFile pathToLogFile must include the filename and
     * @param logLevel 0(trace), 1(debug), 2(info), 3(warn), 4(error), 5(level
     * critical), 6(off)
     * extension, and should be absolute.
     */
    YModemSender (std::shared_ptr<DeviceHandler> deviceHandler_, std::shared_ptr<Logger> logger = std::make_shared<Logger>());

    /** Begin the YModem transmission.
     *  @param files The files to transmit
     *  @param updateCallback updateCallback will be called after each packet
     * sent.
     *  @param yieldCallback when yieldCallback returns False, the transmission
     * will abort.
     *  @param logHex if True, the content of the packets sent will be logged in
     * hexadecimal.
     *  @throw XYModemExceptions::Timeout
     *  @throw XYModemExceptions::CouldNotOpenFile
     *  @throw XYModemExceptions::TransmissionAborted
     *  @see XModem
     */
    void transmit (
        const std::vector<std::shared_ptr<File>>& files,
        std::function<void (float)> updateCallback = [] (float) {},
        std::function<bool ()> yieldCallback = [] () { return false; },
        const bool logHex = false);

protected:
private:
    /** Make the first packet containing filename, filesize, file last
     * modification date
     * @param _fileName Filename with extension
     * @param _fileSize Filesize in bytes
     * @param _lastModificationDate In seconds from epoch
     * @param logHex if True, the content of the packets sent will be logged in
     * hexadecimal.
     */
    Packet makeHeaderPacket (const std::string& _fileName,
                             const int64& _fileSize,
                             const int64& _lastModificationDate,
                             const bool logHex = false);
    /** Make the last packet
     * @param logHex if True, the content of the packets sent will be logged in
     * hexadecimal.
     */
    Packet makeLastPacket (const bool logHex = false);

    /** Write packet to the device
     * @param packet The packet to send to the device
     */
    void writePacket (Packet packet);

    virtual void executeState (const unsigned int currentState,
                               bool logHex) override;
    void executeSendHeader (bool logHex);


    XModemSender xModem;
    std::vector<std::shared_ptr<File>> files;
    
    // YModem state machine constants
    [[maybe_unused]] static constexpr std::string_view retries =
        "YModem number of retries";
    [[maybe_unused]] static constexpr unsigned int waitingStart = 0;
    [[maybe_unused]] static constexpr unsigned int sendingHeader = 1;
    [[maybe_unused]] static constexpr unsigned int retryingHeader = 2;
    [[maybe_unused]] static constexpr unsigned int xModemTransmission = 3;
    [[maybe_unused]] static constexpr unsigned int undefined = 4;
    [[maybe_unused]] static constexpr unsigned int abort = 5;
    [[maybe_unused]] static constexpr unsigned int transmissionFinished = 6;
    // clang-format off
    [[maybe_unused]] inline static std::array<transition, 11> stateTransitions =
        {{{waitingStart,sendingHeader,xyModemConst::C,[] (GuardConditions) { return true; }},
          {sendingHeader,xModemTransmission,xyModemConst::ACK,[] (GuardConditions) { return true; }},
          {sendingHeader,retryingHeader,xyModemConst::NAK,[] (GuardConditions) { return true; }},
          {retryingHeader,retryingHeader,xyModemConst::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) <= xyModemConst::maxRetries; }},
          {retryingHeader,abort,xyModemConst::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) > xyModemConst::maxRetries; }},
          {xModemTransmission,sendingHeader,xyModemConst::C,[] (GuardConditions) { return true; }},
          {undefined,sendingHeader,xyModemConst::C,[] (GuardConditions) { return true; }},
          {waitingStart,abort,xyModemConst::CAN,[] (GuardConditions) { return true; }},
          {sendingHeader,abort, xyModemConst::CAN, [] (GuardConditions) { return true; }},
          {xModemTransmission,abort,xyModemConst::CAN, [] (GuardConditions) { return true; }},
          {undefined, abort, xyModemConst::CAN, [] (GuardConditions) {return true;}}
          }};
    //clang-format on

    FRIEND_TEST (TestXYModemHelper, TestMakeHeaderPacket);
    FRIEND_TEST (TestXYModemHelper, TestMakeLastPacket);
    FRIEND_TEST (YModemTest, TestBeginYModem);
    FRIEND_TEST (YModemTest, TestBeginYModem);
    FRIEND_TEST (YModemTest, TestBeginXModemTransmission);
    FRIEND_TEST (YModemTest, TestRetryingHeader);
    FRIEND_TEST (YModemTest, TestReretryingHeader);
    FRIEND_TEST (YModemTest, TestTooManyRetries);
    FRIEND_TEST (YModemTest, TestUnknownCharacterReceived);
    FRIEND_TEST (YModemTest, TestWaitingStartButCAN);
    FRIEND_TEST (YModemTest, TestSendingHeaderButCAN);
    FRIEND_TEST (YModemTest, TestRetryingHeaderButCAN);
    FRIEND_TEST (YModemTest, TestXModemTransmissionButCAN);
};
}
