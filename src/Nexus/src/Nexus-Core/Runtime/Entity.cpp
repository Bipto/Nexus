#include "Entity.hpp"

namespace Nexus
{
Entity::Entity(GUID id, const std::string &name) : m_ID(id), m_Name(name)
{
}

void Entity::SetID(GUID id)
{
    m_ID = id;
}

GUID Entity::GetID() const
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

void Entity::Serialize(YAML::Emitter &out) const
{
    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << m_ID.Value;
    out << YAML::Key << "Name" << YAML::Value << m_Name;
    out << YAML::EndMap;
}

bool Entity::Deserialize(YAML::Node &data)
{
    return false;
}
} // namespace Nexus
