#include "Utils.hpp"

namespace Nexus::Utils
{
    glm::vec4 ColorFromRGBA(float r, float g, float b, float a)
    {
        return glm::vec4(
            1.0f / 255.0f * r,
            1.0f / 255.0f * g,
            1.0f / 255.0f * b,
            1.0f / 255.0f * a);
    }

    glm::vec4 ColorFromBorderColor(Nexus::Graphics::BorderColor color)
    {
        switch (color)
        {
        case Nexus::Graphics::BorderColor::TransparentBlack:
            return {0.0f, 0.0f, 0.0f, 0.0f};
        case Nexus::Graphics::BorderColor::OpaqueBlack:
            return {0.0f, 0.0f, 0.0f, 1.0f};
        case Nexus::Graphics::BorderColor::OpaqueWhite:
            return {1.0f, 1.0f, 1.0f, 1.0f};
        default:
            throw std::runtime_error("Failed to find a valid border color");
        }
    }
}