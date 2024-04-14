#pragma once

#include "glm/glm.hpp"

#include <string>

namespace Nexus
{
    class Scene
    {
    public:
        Scene() = delete;
        Scene(const std::string &name);

        const std::string &GetName() const { return m_Name; }
        const glm::vec4 &GetClearColour() const { return m_ClearColour; }

        void SetName(const std::string &name) { m_Name = name; }
        void SetClearColour(const glm::vec4 &clearColour) { m_ClearColour = clearColour; }

        void Serialize(const std::string &filepath);

    private:
        std::string m_Name;
        glm::vec4 m_ClearColour = {1.0f, 1.0f, 1.0f, 1.0f};
    };
}