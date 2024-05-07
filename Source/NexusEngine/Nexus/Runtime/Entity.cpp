#include "Entity.hpp"

namespace Nexus
{
    Entity::Entity(uint64_t id, const std::string &name)
        : m_ID(id), m_Name(name)
    {
    }

    void Entity::SetID(uint64_t id)
    {
        m_ID = id;
    }

    uint64_t Entity::GetID() const
    {
        return m_ID;
    }

    void Entity::SetName(const std::string &name)
    {
        m_Name = name;
    }

    const std::string &Entity::GetName() const
    {
        return m_Name;
    }
}
