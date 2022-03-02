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
        crc_cpp::crc16_xmodem crc;

        for (auto c : data)
        {
            crc.update (c);
        }

        return crc.final ();
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
