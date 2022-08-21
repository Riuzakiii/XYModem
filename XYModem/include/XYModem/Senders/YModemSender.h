#pragma once
#include "Devices/DeviceHandler.h"
#include "FileTransferProtocol.h"
#include "Tools.h"
#include "XModemSender.h"
#include "XYModemConst.h"
#include "gtest/gtest_prod.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <functional>
#include <string>
#include <string_view>
#include <utility>

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
template <std::size_t payloadSize = xymodem::payloadSize1K>
class YModemSender : private FileTransferProtocol
{
public:
    /** Initialise the YModem sender.
     * @param pathToLogFile pathToLogFile must include the filename and
     * @param logLevel 0(trace), 1(debug), 2(info), 3(warn), 4(error), 5(level
     * critical), 6(off)
     * extension, and should be absolute.
     */
    explicit YModemSender (const std::shared_ptr<DeviceHandler>& deviceHandler_, const std::shared_ptr<Logger>& logger = std::make_shared<Logger>());

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
        std::function<bool()> yieldCallback = []() { return false; },
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
    std::array<uint8_t, payloadSize + xymodem::totalExtraSize>
    makeHeaderPacket (const std::string& _fileName, const int64& _fileSize, const int64& _lastModificationDate, const bool logHex = false);
    /** Make the last packet
     * @param logHex if True, the content of the packets sent will be logged in
     * hexadecimal.
     */
    std::array<uint8_t, payloadSize + xymodem::totalExtraSize> makeLastPacket (const bool logHex = false);

    /** Write packet to the device
     * @param packet The packet to send to the device
     */
    void writePacket (std::array<uint8_t, payloadSize + xymodem::totalExtraSize> packet);

    virtual void executeState (const unsigned int currentState, bool logHex) override;
    void executeSendHeader (bool logHex);

    XModemSender<> xModem;
    std::vector<std::shared_ptr<File>> files;

    // YModem state machine constants
    [[maybe_unused]] static constexpr std::string_view retries = "YModem number of retries";
    [[maybe_unused]] static constexpr unsigned int waitingStart = 0;
    [[maybe_unused]] static constexpr unsigned int sendingHeader = 1;
    [[maybe_unused]] static constexpr unsigned int retryingHeader = 2;
    [[maybe_unused]] static constexpr unsigned int xModemTransmission = 3;
    [[maybe_unused]] static constexpr unsigned int undefined = 4;
    [[maybe_unused]] static constexpr unsigned int abort = 5;
    [[maybe_unused]] static constexpr unsigned int transmissionFinished = 6;

    static bool noConditions (const GuardConditions&) { return true; }
    static bool checkCanRetry (const GuardConditions& t_guards) { return t_guards.get (retries) <= xymodem::maxRetries; }
    static bool checkCannotRetry (const GuardConditions& t_guards) { return t_guards.get (retries) > xymodem::maxRetries; }

    // clang-format off
    [[maybe_unused]] static inline std::array<transition<uint8_t>, 11> stateTransitions
        {{{waitingStart,sendingHeader,xymodem::C, noConditions},
          {sendingHeader,xModemTransmission,xymodem::ACK, noConditions},
          {sendingHeader,retryingHeader,xymodem::NAK, noConditions},
          {retryingHeader,retryingHeader,xymodem::NAK, checkCanRetry},
          {retryingHeader,abort,xymodem::NAK, checkCannotRetry},
          {xModemTransmission,sendingHeader,xymodem::C, noConditions},
          {undefined,sendingHeader,xymodem::C, noConditions},
          {waitingStart,abort,xymodem::CAN, noConditions},
          {sendingHeader,abort, xymodem::CAN, noConditions},
          {xModemTransmission,abort,xymodem::CAN, noConditions},
          {undefined, abort, xymodem::CAN, noConditions}
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

#include "../../src/XYModem/Senders/YModemSender.hpp"
