#pragma once
#include "gtest/gtest_prod.h"
#include <array>
#include <string>
#include <algorithm>
#include <cassert>
#include <memory>
#include <tuple>
#include <variant>
#include <functional>
#include <string_view>
#include <exception>
#include <utility>
#include "spdlog/spdlog.h"
#include "crc_cpp.h"
//#include <filesystem> Unfortunately only supported on most recent OS versions
#include "ghc/filesystem.hpp" //Workaround, when filesystem is more widely used just change ghc::filesystem for std::filesystem
#include "XYModemConst.h"
#include "Devices/DeviceHandler.h"
#include "FileTransferProtocol.h"
#include "Tools.h"

namespace XYModemExceptions
{
struct Timeout : public std::exception
{
    long long timeoutSeconds = 0;

    Timeout (long long timeoutSeconds) : timeoutSeconds (timeoutSeconds){};

    virtual const char* what () const throw () { return "Timeout"; }
};
struct CouldNotOpenFile : public std::exception
{
    std::string absolutePath;
    std::string response;

    CouldNotOpenFile (std::string absolutePath) : absolutePath (absolutePath){};

    virtual const char* what () const throw () { return "Could not open file"; }
};

struct TransmissionAborted : public std::exception
{
    virtual const char* what () const throw ()
    {
        return "Transmission aborted.";
    }
};
} // namespace XYModemExceptions

namespace xymodem
{
/** XModem 1K sender, tested with Tera term receiver.
 * @todo end of File
 * @todo setFileInfos
 */
class XModem : private FileTransferProtocol
{
public:
    /** Initialise the XModem sender.
     * @param pathToLogFile pathToLogFile must include the filename and
     * extension. w_string on Windows
     * @param logLevel 0(trace), 1(debug), 2(info), 3(warn), 4(error), 5(level
     * critical), 6(off)
     */
    XModem (DeviceHandler& deviceHandler_,
            const logType pathToLogFile = logType (),
            const int logLevel = 2);

    XModem (DeviceHandler& deviceHandler_,
            std::shared_ptr<spdlog::logger> logger);

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
        const ghc::filesystem::path& fileAbsolutePath,
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
    Packet makeDataPacket (const std::string& data,
                           const uint8_t& packetNum,
                           const bool logHex = false);

    /** Write packet to the device
     * @param packet The packet to send to the device
     */
    void writePacket (Packet packet);

    virtual void executeState (const unsigned int currentState,
                               bool logHex) override;
    void executeSendPacket (bool logHex);

    uint8_t packetNum = 1;
    std::intmax_t lastFullPacket = 0;
    std::intmax_t lastPacketRemaining = 0;
    std::intmax_t totalPackets = 0;

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
        {{{waitingStart, sendingPacket, xyModemConst::C, [] (GuardConditions) { return true; }},
          {sendingPacket, sendingPacket,xyModemConst::ACK,[] (GuardConditions t_guards){ return t_guards.get (packetsLeft) > 0; }},
          {sendingPacket, retryingPacket, xyModemConst::NAK, [] (GuardConditions) { return true; }},
          {sendingPacket, endOfTransmission, xyModemConst::ACK, [] (GuardConditions t_guards) { return t_guards.get (packetsLeft) == 0; }},
          {endOfTransmission, transmissionFinished, xyModemConst::ACK, [] (GuardConditions) { return true; }},
          {endOfTransmission, retryingEOT, xyModemConst::NAK, [] (GuardConditions) { return true; }},
          {retryingEOT, transmissionFinished, xyModemConst::ACK, [] (GuardConditions) { return true; }},
          {retryingEOT, retryingEOT, xyModemConst::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) <= xyModemConst::maxRetries; }},
          {retryingEOT,abort,xyModemConst::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) > xyModemConst::maxRetries; }},
          {retryingPacket, retryingPacket,xyModemConst::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) <= xyModemConst::maxRetries; }},
          {retryingPacket,sendingPacket,xyModemConst::ACK,[] (GuardConditions) { return true; }},
          {retryingPacket,abort,xyModemConst::NAK,[] (GuardConditions t_guards){ return t_guards.get (retries) > xyModemConst::maxRetries; }},
          {undefined,sendingPacket,xyModemConst::C,[] (GuardConditions t_guards){ return t_guards.get (packetsLeft) > 0; }},
          {undefined,retryingPacket,xyModemConst::NAK,[] (GuardConditions) { return true; }},
          {undefined,abort,xyModemConst::CAN,[] (GuardConditions) { return true; }},
          {waitingStart,abort,xyModemConst::CAN,[] (GuardConditions) { return true; }},
          {sendingPacket,abort,xyModemConst::CAN,[] (GuardConditions) { return true; }},
          {endOfTransmission,abort,xyModemConst::CAN,[] (GuardConditions) { return true; }},
          {retryingPacket,abort, xyModemConst::CAN,[] (GuardConditions) { return true; }},
          {retryingEOT, abort, xyModemConst::CAN, [] (GuardConditions) {return true;}}
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

/** Simple YModem 1K Sender. Tested with Tera Term YModem receiver. Will be able
 * to support YModem 1K batch, but it needs to be implemented.
 * @note there are differences with the YModem protocol
 * reference.
 * 1. The header packet ends with a CRC, which works with TeraTerm but is not in
 * the YModem protocol reference.
 * @see XModem
 * @see FileTransferProtocol
 */
class YModem : private FileTransferProtocol
{
public:
    /** Initialise the YModem sender.
     * @param pathToLogFile pathToLogFile must include the filename and
     * @param logLevel 0(trace), 1(debug), 2(info), 3(warn), 4(error), 5(level
     * critical), 6(off)
     * extension, and should be absolute.
     */
    YModem (DeviceHandler& deviceHandler_,
            const logType pathToLogFile = logType (),
            const int logLevel = 2);

    /** Begin the YModem transmission.
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
     *  @see XModem
     */
    void transmit (
        const std::vector<ghc::filesystem::path>& filesAbsolutePaths,
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


    XModem xModem;
    std::vector<ghc::filesystem::path> filesAbsolutePaths;
    
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
} // namespace
