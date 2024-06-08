#pragma once

#include "Nexus/Graphics/SamplerState.hpp"
#include "glm/glm.hpp"

namespace Nexus::Utils
{
    glm::vec4 ColorFromRGBA(float r, float g, float b, float a);
    glm::vec4 ColorFromBorderColor(Nexus::Graphics::BorderColor color);

    template <typename T>
    T ReMapRange(T oldMin, T oldMax, T newMin, T newMax, T value)
    {
        return newMin + (value - oldMin) * (newMax - newMin) / (oldMax - newMin);
    }
}