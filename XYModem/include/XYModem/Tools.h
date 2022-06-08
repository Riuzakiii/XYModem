#pragma once
#include "XYModemConst.h"
#include <array>
#include <iomanip>
#include <ios>
#include <sstream>

namespace xymodem
{
namespace tools
{
    template <std::size_t size>
    inline std::string dispByteArray (std::array<uint8_t, size> data)
    {
        std::stringstream result;

        for (auto it = data.begin(); it != data.end(); ++it)
        {
            result << "0x" << std::setfill ('0') << std::setw (2) << std::hex << std::noshowbase << static_cast<int> (*it);
            if (it != (data.end() - 1))
            {
                result << ",";
            }
        }
        return result.str();
    }

    constexpr std::array<uint16_t, 256> crc16lookup (uint16_t polynomial = 0x1021)
    {
        constexpr auto lookupSize = 256;
        constexpr auto controlBitSize = 8;

        std::array<uint16_t, lookupSize> lookUpTable{0};

        for (int i = 0; i < lookupSize; ++i)
        {
            auto control = static_cast<uint8_t> (i);
            uint16_t sum = 0;
            for (int j = 0; j < controlBitSize; ++j)
            {
                if ((control >> (7 - j) & 1U) == 1U)
                {
                    control ^= (polynomial >> (controlBitSize + j + 1)) & 0xff;
                    sum ^= (polynomial << (controlBitSize - j - 1)) & 0xffff;
                }
            }
            lookUpTable.at (i) = sum;
        }
        return lookUpTable;
    }

    /** CRC is based on the polynomial long division of data by a polynomial.
     * The CRC is the remainder in this division. A Look-up table speeds up the
     * computation, trading memory for speed (an array is computed at compile
     * time, should be a bit more than 512 bytes).
     */
    template <std::size_t payloadSize = xymodem::payloadSize1K>
    [[nodiscard]] inline uint16_t compute_crc16xmodem (std::array<uint8_t, payloadSize> const& data)
    {
        constexpr uint16_t crc16ccitt = 0x1021;
        constexpr auto table = crc16lookup (crc16ccitt);

        uint16_t remainder = 0;
        for (std::size_t i = 0; i < (data.size() + 2); ++i)
        {
            uint8_t controlByte = (remainder >> 8) & 0xff; // the top byte determines the next 8
                                                           // steps in the division
            uint16_t sum = table[controlByte];
            remainder <<= 8; // prepare to add the next byte

            if (i < data.size())
            {
                remainder |= (data[i] & 0xff);
            }
            else
            {
                remainder |= remainder + (0x00 & 0xff);
            }
            remainder ^= sum;
        }
        return static_cast<uint16_t> (remainder);
    }

    inline void increasePacketNum (uint8_t& numPacket)
    {
        if (numPacket == 255)
        {
            numPacket = 0;
            return;
        }
        ++numPacket;
    }

    inline void decreasePacketNum (uint8_t& numPacket)
    {
        if (numPacket == 0)
        {
            numPacket = 255;
            return;
        }
        --numPacket;
    }

    template <typename Integer>
    std::string decimalToOctal (Integer decimalInt)
    {
        std::string result;
        while (decimalInt != 0 || result.empty())
        {
            auto remainder = decimalInt % 8;
            decimalInt /= 8;
            result.insert (result.begin(), '0' + static_cast<char> (remainder));
        }
        return result;
    }
} // namespace tools
} // namespace xymodem
