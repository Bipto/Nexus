#pragma once

#include "glm/glm.hpp"

namespace Nexus
{
    class AudioSource
    {
    public:
        virtual glm::vec3 &GetPosition() = 0;
        virtual void SetPosition(const glm::vec3 &position) = 0;
    };
}