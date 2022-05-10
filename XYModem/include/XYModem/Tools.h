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
        unsigned int crc16ccitt = 0x1021;
        uint16_t remainder = 0;
        uint8_t pop = 0;

        for(int i = 0; i < (data.size() * 8 + 16); ++i)
        {
            pop = (remainder >> 15) & 1U;
            remainder <<=1;
            
            if(i >= data.size() * 8)
            {
                remainder += 0 & 1U;
            }
            else
            {
                remainder += (data[i/8] >> (7 - i%8)) & 1U; 
            }
            
            if(pop)
            {
                remainder ^= crc16ccitt;
            }
        }

        return remainder;
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
