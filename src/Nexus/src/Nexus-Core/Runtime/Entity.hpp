#pragma once

#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "yaml-cpp/yaml.h"

namespace Nexus
{
	struct Entity
	{
	  public:
		void			   Serialize(YAML::Emitter &out) const;
		bool			   Deserialize(YAML::Node &data);

		GUID		ID	 = {};
		std::string Name = "Unnamed Entity";
	};
}	 // namespace Nexus