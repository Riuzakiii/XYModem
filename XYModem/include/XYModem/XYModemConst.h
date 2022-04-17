#pragma once
#include <string>
#include <array>
#include <chrono>
using namespace std::literals::chrono_literals;

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

using int64 = long long;                                                                                            
namespace xyModemConst
{
[[maybe_unused]] constexpr uint8_t SOH =
    0x01; // also found as "SOH" in the documentation.
[[maybe_unused]] constexpr uint8_t STX =
    0x02; // also found as "SOH" in the documentation.
[[maybe_unused]] constexpr uint8_t EOT =
    0x04; // also found as "EOT" in the documentation.
[[maybe_unused]] constexpr uint8_t ACK =
    0x06; // also found as "ACK" in the documentation.
[[maybe_unused]] constexpr uint8_t NAK =
    0x15; // also found as "NAK" in the documentation.
[[maybe_unused]] constexpr uint8_t ETB =
    0x17; // also found as "ETB" in the documentation.
[[maybe_unused]] constexpr uint8_t CAN =
    0x18; // also found as "CAN" in the documentation.
[[maybe_unused]] constexpr uint8_t C =
    0x43; // also found as "C" in the documentation.
[[maybe_unused]] constexpr auto fileNameSep = 0x00;
[[maybe_unused]] constexpr auto headerFieldsSep = 0x20;
[[maybe_unused]] constexpr auto packetDataSize = 1024;
[[maybe_unused]] constexpr auto packetHeaderSize = 3;
[[maybe_unused]] constexpr auto packetCRCSize = 2;
[[maybe_unused]] constexpr auto packetTotalSize =
    packetHeaderSize + packetDataSize + packetCRCSize;
[[maybe_unused]] constexpr auto timeout = 30s;
[[maybe_unused]] constexpr auto maxRetries = 10;
} // namespace xyModemConst

using Packet = std::array<uint8_t, xyModemConst::packetTotalSize>;
using packetData = std::array<uint8_t, xyModemConst::packetDataSize>;
