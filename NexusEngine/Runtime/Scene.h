#pragma once

#include "Core/nxpch.h"
#include "Runtime/ECS/Entity.h"

#include <vector>

namespace Nexus
{
    class Scene
    {
        public:
            Scene(const std::string& name)
            {
                m_Name = name;
            }

            const std::string& GetName() { return m_Name; }
            
            void Serialize(const std::string& sceneDirectory);
            void Deserialize(const std::string& filepath);

            void AddEntity(const Nexus::Entity entity)
            {
                m_Entities.push_back(entity);
            }

            void AddEmptyEntity()
            {
                Nexus::Entity entity(std::string("Entity"), m_Entities.size());
                m_Entities.push_back(entity);
            }

            const std::vector<Nexus::Entity>& GetEntities()
            {
                return m_Entities;
            }

            const size_t GetEntityCount() { return m_Entities.size(); }

        private:
            std::string m_Name;
            std::vector<Nexus::Entity> m_Entities;
    };
}