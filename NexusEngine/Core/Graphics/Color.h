#pragma once

#include <inttypes.h>

namespace Nexus
{
    struct Color
    {
    public:
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            m_Color = (uint32_t(r) << 24) | (uint32_t(g) << 16) | (uint32_t(b) << 8) | uint32_t(a);
        }

        uint8_t GetRedChannel() const { return uint8_t(m_Color >> 24); }
        uint8_t GetGreenChannel() const { return uint8_t(m_Color >> 16); }
        uint8_t GetBlueChannel() const { return uint8_t(m_Color >> 8); }
        uint8_t GetAlphaChannel() const { return uint8_t(m_Color); }

    private:
        uint32_t m_Color = 0;
    };
}