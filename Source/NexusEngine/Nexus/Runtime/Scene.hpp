#pragma once

#include "Nexus/Types.hpp"

#include "glm/glm.hpp"

#include "Entity.hpp"

#include "Nexus/nxpch.hpp"

namespace Nexus
{
    class Scene
    {
    public:
        Scene(const std::string &name = "Untitled Scene");

        const std::string &GetName() const { return m_Name; }
        const glm::vec4 &GetClearColour() const { return m_ClearColour; }

        void SetName(const std::string &name) { m_Name = name; }
        void SetClearColour(const glm::vec4 &clearColour) { m_ClearColour = clearColour; }

        void Serialize(const std::string &filepath);

        void AddEmptyEntity();
        std::vector<Entity> &GetEntities();

    public:
        static Scene *Deserialize(const std::string &filepath);

    private:
        std::string m_Name;
        glm::vec4 m_ClearColour = {1.0f, 1.0f, 1.0f, 1.0f};

        std::vector<Entity> m_Entities;
    };
}