#include "Nexus-Core/Runtime/Entity.hpp"

namespace Nexus
{
	void Entity::Serialize(YAML::Emitter &out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << ID.Value;
		out << YAML::Key << "Name" << YAML::Value << Name;
		out << YAML::EndMap;
	}

	bool Entity::Deserialize(YAML::Node &data)
	{
		return false;
	}
}	 // namespace Nexus
