#pragma once

#include <string>

namespace Nexus
{
    class Entity
    {
        public:
            Entity(const std::string& name, const std::string& id) 
                : m_Name(name), m_EntityID(id)
                {}

            const std::string& GetID() { return m_EntityID; }
            const std::string& GetName() const { return m_Name; }
            bool IsActive() { return m_Active; }

        private:
            std::string m_EntityID = "0";
            std::string m_Name = "Entity";
            bool m_Active = true;
    };
}