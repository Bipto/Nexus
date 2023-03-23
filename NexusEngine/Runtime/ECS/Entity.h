#pragma once

#include <string>
#include <vector>

#include "Core/Memory.h"
#include "Components.h"

namespace Nexus
{
    class Entity
    {
        public:
            Entity(const std::string& name, int id) 
                : m_Name(name), m_EntityID(id)
                {}

            const int GetID() { return m_EntityID; }
            bool IsActive() { return m_Active; }

            const std::string& GetName() const { return m_Name; }
            void SetName(const std::string& name) { m_Name = name; }

            const std::vector<Ref<Component>>& GetComponents() { return m_Components; }
            void AddComponent(Ref<Component> component) { m_Components.push_back(component); }

        private:
            int m_EntityID = 0;
            std::string m_Name = "Entity";
            bool m_Active = true;
            std::vector<Ref<Component>> m_Components;
    };
}