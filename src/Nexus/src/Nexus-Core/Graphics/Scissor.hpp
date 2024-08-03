#pragma once

#include "Nexus-Core/Graphics/Rectangle.hpp"

namespace Nexus::Graphics
{
    struct Scissor
    {
        uint32_t X = 0;
        uint32_t Y = 0;
        uint32_t Width = 0;
        uint32_t Height = 0;

        Scissor() = default;

        explicit Scissor(const Rectangle<float> &rect)
        {
            X = rect.GetLeft();
            Y = rect.GetTop();
            Width = rect.GetWidth();
            Height = rect.GetHeight();
        }
    };
}