#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Utils
{
	class ScriptProjectGenerator
	{
	  public:
		NX_API		ScriptProjectGenerator() = default;
		NX_API void Generate(const std::string &templatePath, const std::string &projectName, const std::string &projectDirectory);
	};
}	 // namespace Nexus::Utils