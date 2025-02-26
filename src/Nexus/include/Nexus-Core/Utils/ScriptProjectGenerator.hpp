#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Utils
{
	class NX_API ScriptProjectGenerator
	{
	  public:
		ScriptProjectGenerator() = default;
		void Generate(const std::string &templatePath, const std::string &projectName, const std::string &projectDirectory);
	};
}	 // namespace Nexus::Utils