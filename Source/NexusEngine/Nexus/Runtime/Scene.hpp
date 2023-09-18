#pragma once

#include "Nexus/Memory.hpp"
#include "Nexus/Runtime/ECS/Entity.hpp"

#include <vector>

namespace Nexus
{
    class Scene
    {
    public:
        Scene(const std::string &name)
        {
            m_Name = name;
        }

        const std::string &GetName() const { return m_Name; }
        std::string &GetName() { return m_Name; }

        void Serialize(const std::string &sceneDirectory);
        void Deserialize(const std::string &filepath);

        void AddEntity(const Nexus::Entity entity)
        {
            m_Entities.push_back(entity);
        }

        void AddEmptyEntity()
        {
            Nexus::Entity entity(std::string("Entity"), m_Entities.size());
            TransformComponent *component = new TransformComponent();
            entity.AddComponent(component);
            m_Entities.push_back(entity);
        }

        void SetClearColor(const glm::vec4 &clearColor) { m_ClearColor = clearColor; }
        const glm::vec4 &GetClearColor() const { return m_ClearColor; }
        glm::vec4 &GetClearColor() { return m_ClearColor; }

        std::vector<Nexus::Entity> &GetEntities() { return m_Entities; }

        const size_t GetEntityCount() { return m_Entities.size(); }

    private:
        void LoadEntity(nlohmann::json json, int id);
        void LoadComponent(Entity &entity, nlohmann::json json);
        std::string m_Name;
        std::vector<Nexus::Entity> m_Entities;
        glm::vec4 m_ClearColor{0.39f, 0.58f, 0.92f, 1.0f};
    };
}