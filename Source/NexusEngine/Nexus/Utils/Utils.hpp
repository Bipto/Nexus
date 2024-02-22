#pragma once

#include "Nexus/Graphics/SamplerState.hpp"
#include "glm/glm.hpp"

namespace Nexus::Utils
{
    const glm::vec4 ColorFromRGBA(float r, float g, float b, float a);
    const glm::vec4 ColorFromBorderColor(Nexus::Graphics::BorderColor color);
}