#include "Nexus-Core/Runtime/Entity.hpp"

#include "Nexus-Core/ECS/ComponentRegistry.hpp"
#include "Nexus-Core/ECS/Registry.hpp"

#include "Nexus-Core/Runtime/Project.hpp"

namespace Nexus
{
	void Entity::Serialize(YAML::Emitter &out, ECS::Registry &registry, Project *project) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << ID.Value;
		out << YAML::Key << "Name" << YAML::Value << Name;

		const std::vector<Nexus::ECS::ComponentPtr> components = registry.GetAllComponents(ID);

		// serialize the entity's components
		if (components.size() > 0)
		{
			out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;

			for (const Nexus::ECS::ComponentPtr &component : components)
			{
				auto	   &componentRegistry = ECS::ComponentRegistry::GetRegistry();
				std::string displayName		  = project->GetDisplayNameFromComponent(component);
				YAML::Node	componentNode	  = project->SerializeComponentToYaml(registry, component);

				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << displayName;
				out << YAML::Key << "HierarchyIndex" << YAML::Value << component.entityComponentIndex;
				out << YAML::Key << "Data" << YAML::Value << componentNode;
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;
		}

		out << YAML::EndMap;
	}

}	 // namespace Nexus
