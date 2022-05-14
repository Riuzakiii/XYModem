#pragma once
#include "fmt/format.h"

namespace tools
{
    template <int size> inline std::string dispByteArray (std::array<uint8_t, size> data)
    {
        return fmt::format("{:#04x}", fmt::join(data,","));
    }

    [[nodiscard]] inline uint16_t compute_crc16xmodem (packetData const& data)
    {
        uint16_t crc16ccitt = 0x1021;
        uint16_t remainder = 0;

        for(int i = 0; i < (data.size() + 2); ++i)
        {
            uint8_t controlByte = (remainder >> 8) & 0xff;
            uint16_t sum = 0;

            for (int j = 0; j < 8; ++j)
            {
                if ((controlByte >> (7 - j) & 1U) == 1U)
                {
                    controlByte ^= (crc16ccitt >> (8 + j + 1));
                    sum ^= (crc16ccitt << (8 - j - 1));
                }
            }
            remainder <<= 8;
            
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

        return static_cast<uint16_t>(remainder);
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

}; // namespace tools
