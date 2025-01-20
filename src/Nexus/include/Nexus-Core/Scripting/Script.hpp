#pragma once

#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Scripting
{
	NX_API class Script
	{
	  public:
		NX_API Script() = default;
		virtual NX_API ~Script()
		{
		}

		virtual void NX_API OnLoad()
		{
		}

		virtual void NX_API OnUpdate(TimeSpan time)
		{
		}

		virtual void NX_API OnRender(TimeSpan time)
		{
		}

		virtual void NX_API OnUnload()
		{
		}
	};

	NX_API inline std::map<std::string, std::function<Script *()>> RegisteredScriptCreationFunctions = {};

	extern "C" NX_API void *GetRegisteredScriptCreationFunctions()
	{
		return &RegisteredScriptCreationFunctions;
	}

}	 // namespace Nexus::Scripting
#define NX_REGISTER_SCRIPT(T, Name) Nexus::Scripting::RegisteredScriptCreationFunctions[Name] = []() { return new T(); };