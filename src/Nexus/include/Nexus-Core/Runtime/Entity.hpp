#pragma once

#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "yaml-cpp/yaml.h"

namespace Nexus
{
	class Project;

	namespace ECS
	{
		class Registry;
	}

	struct Entity
	{
	  public:
		void Serialize(YAML::Emitter &out, ECS::Registry &registry, Project *project) const;

		GUID		ID	 = {};
		std::string Name = "Unnamed Entity";
	};
}	 // namespace Nexus