#pragma once
#ifdef TESTING_ENABLED
#include "gtest/gtest_prod.h"
#endif
#include "Devices/DeviceHandler.h"
#include "Files/File.h"
#include "Loggers/Logger.h"
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>

namespace xymodem
{
class GuardConditions;
using guard_test = std::function<bool (GuardConditions)>;
using transition = const std::tuple<unsigned int,
                                    unsigned int,
                                    uint8_t,
                                    guard_test>; //(current state, next
                                                 // state, event ,guard)

/**
 * GuardConditions : prevent using guard conditions that have not been manually
 * initialized.
 */
class GuardConditions
{
public:
    GuardConditions() = default;

    /**
     * @brief Get the value of an existing guard condition.
     * @param guardName
     * @return guardType
     */
    [[nodiscard]] std::intmax_t get (std::string_view guardName) const;
    /**
     * @brief Set the value of an existing guard condition.
     * @param guardName
     * @param value
     */
    void set (std::string_view guardName, const std::intmax_t value);
    /**
     * @brief Add a new guard, will be set to zero by default.
     * @param guardName
     * @param value
     */
    void addGuard (std::string_view guardName, const std::intmax_t value = 0);
    /**
     * @brief Add new guards. If the key already exists, does nothing.
     * @param newGuards
     */
    void addGuards (
        const std::vector<std::pair<std::string_view, std::intmax_t>>&
            newGuards);
    /**
     * @brief Set all values to zero.
     */
    void clear();
    /**
     * @brief Incremement by 1
     * @param guardName
     */
    void inc (std::string_view guardName);
    /**
     * @brief Decrement by 1
     * @param guardName
     */
    void dec (std::string_view guardName);

private:
    std::unordered_map<std::string_view, std::intmax_t> guardConditions;
};

/**
 * FileTransferProtocol : base class for finite state machine based file
 * transfer protocols
 */
class FileTransferProtocol
{
public:
    /**
     * @param pathToLogFile Path to logfile, wstring on windows
     * @param logLevel 0(trace), 1(debug), 2(info), 3(warn), 4(error), 5(level
     * critical), 6(off)
     */

    FileTransferProtocol (
        std::shared_ptr<DeviceHandler> device,
        const unsigned int& currentState,
        std::shared_ptr<Logger> logger = std::make_shared<Logger>());
    virtual ~FileTransferProtocol() = default;

protected:
    /** Make a transition in the XModem state machine. From a given
     * signal(character) and the current state, gives the next state.
     * @param characterReceived Character received from the device
     * @param currentState The state in which XModem is currently in.
     * @return The new state, which can then be used to execute the init
     * functions of the state.
     */
    template <size_t N>
    [[nodiscard]] unsigned int
        getNextState (uint8_t characterReceived,
                      unsigned int t_currentState,
                      unsigned int undefined,
                      std::array<transition, N> transitions)
    {
        for (const auto& transition : transitions)
        {
            if (std::get<currentStatePtr> (transition) == t_currentState &&
                std::get<characterReceivedPtr> (transition) ==
                    characterReceived &&
                std::get<guardsTestsPtr> (transition) (guards))
            {
                return std::get<nextStatePtr> (transition);
            }
        }
        return undefined;
    }

    /** Execute some actions when entering a new state.
     *  @param currentState The current state
     *  @param logHex if True, the content of the packets sent will be logged in
     * hexadecimal.
     */
    virtual void executeState (const unsigned int currentState, bool logHex);

    std::shared_ptr<DeviceHandler> deviceHandler;
    std::shared_ptr<Logger> logger;
    unsigned int currentState;
    std::function<void (float)> updateCallback = [] (float) {};
    std::function<bool()> yieldCallback = []() { return false; };
    GuardConditions guards;
    bool isTransmissionFinished = false;
    bool transmissionShouldFinish = false;

    static constexpr int currentStatePtr = 0;
    static constexpr int nextStatePtr = 1;
    static constexpr int guardsTestsPtr = 3;
    static constexpr int characterReceivedPtr = 2;

private:
#ifdef TESTING_ENABLED
    FRIEND_TEST (TestXYModemHelper, TestSetFileInfos);
#endif
};
} // namespace xymodem
