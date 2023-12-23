#include "Utils.hpp"

namespace Nexus::Utils
{
    const glm::vec4 ColorFromRGBA(float r, float g, float b, float a)
    {
        return glm::vec4(
            1.0f / 255.0f * r,
            1.0f / 255.0f * g,
            1.0f / 255.0f * b,
            1.0f / 255.0f * a);
    }
}