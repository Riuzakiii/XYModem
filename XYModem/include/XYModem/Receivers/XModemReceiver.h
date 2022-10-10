#pragma once
#include "../FileTransferProtocol.h"
#include "../Files/File.h"
#include "../XYModemConst.h"
#include <array>

namespace xymodem
{

class XModemReceiver : private FileTransferProtocol
{
    using eventType = int;

public:
    XModemReceiver (std::shared_ptr<DeviceHandler> device, std::shared_ptr<Logger> logger = std::make_shared<Logger>());
    void receive (std::shared_ptr<File> destination);

private:
    void executeState (const unsigned int currentState, bool logHex) override;

    [[maybe_unused]] static constexpr eventType noEvent = -1;
    [[maybe_unused]] static constexpr eventType receveidSomething = -2;
    [[maybe_unused]] static constexpr eventType receivedPayload = -3;
    [[maybe_unused]] static constexpr eventType timeout1s = -4;
    [[maybe_unused]] static constexpr eventType timeout3s = -5;
    [[maybe_unused]] static constexpr eventType packetSuccessfullyWritten = -6;
    [[maybe_unused]] static constexpr eventType errorInPacket = -7;
    [[maybe_unused]] static constexpr std::string_view retriesStart = "XModem number of retries waiting for first packet";
    [[maybe_unused]] static constexpr std::string_view retries = "XModem number of retries";
    static constexpr int maxRetriesStart = 20;
    [[maybe_unused]] static constexpr unsigned int sendStartWithCRCSignal = 1;
    [[maybe_unused]] static constexpr unsigned int waitForPacket = 2;
    [[maybe_unused]] static constexpr unsigned int processPacket = 3;
    [[maybe_unused]] static constexpr unsigned int sendNotAcknowledged = 4;
    [[maybe_unused]] static constexpr unsigned int sendAcknowledged = 5;
    [[maybe_unused]] static constexpr unsigned int acknowledgeEndOfTransmission = 6;
    [[maybe_unused]] static constexpr unsigned int timeout = 7;

    static bool noConditions (GuardConditions) { return true; }
    static bool checkCanRetryStart (const GuardConditions& t_guards) { return t_guards.get (retriesStart) <= maxRetriesStart; }
    static bool checkCannotRetryStart (const GuardConditions& t_guards) { return t_guards.get (retriesStart) > maxRetriesStart; }
    static bool checkCanRetry (const GuardConditions& t_guards) { return t_guards.get (retries) <= xymodem::maxRetries; }
    static bool checkCannotRetry (const GuardConditions& t_guards) { return t_guards.get (retries) > xymodem::maxRetries; }

    // clang-format off
    static inline std::array<transition<eventType>, 14> transitions = {{
        {sendStartWithCRCSignal, sendStartWithCRCSignal, timeout3s, checkCanRetryStart},
        {sendStartWithCRCSignal, timeout, timeout3s, checkCannotRetryStart},
        {sendStartWithCRCSignal, waitForPacket, receveidSomething, noConditions },
        {waitForPacket, processPacket, receivedPayload, noConditions},
        {processPacket, sendAcknowledged, packetSuccessfullyWritten, noConditions},
        {sendAcknowledged, waitForPacket, noEvent, noConditions},
        {waitForPacket, timeout, timeout1s, noConditions},
        {processPacket, sendNotAcknowledged, errorInPacket, noConditions},
        {sendNotAcknowledged, timeout, noEvent, noConditions},
        {sendNotAcknowledged, waitForPacket, noEvent, checkCanRetry},
        {sendNotAcknowledged, timeout, noEvent, checkCannotRetry},
        {waitForPacket, acknowledgeEndOfTransmission, xymodem::EOT, noConditions},
        {acknowledgeEndOfTransmission, acknowledgeEndOfTransmission, xymodem::EOT, checkCanRetry},
        {acknowledgeEndOfTransmission, timeout, noEvent, checkCannotRetry}
    }};
    // clang-format on
};

} // namespace xymodem
