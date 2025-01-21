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

	class ScriptRegistry
	{
	  public:
		static std::map<std::string, std::function<Script *()>> &GetRegistry()
		{
			static std::map<std::string, std::function<Script *()>> registry;
			return registry;
		}
	};

	extern "C" NX_API std::map<std::string, std::function<Script *()>> *GetScriptRegistry()
	{
		return &ScriptRegistry::GetRegistry();
	}

}	 // namespace Nexus::Scripting

#define NX_REGISTER_SCRIPT(ClassType)                                                                                                                \
	struct ClassType##Register                                                                                                                       \
	{                                                                                                                                                \
		ClassType##Register()                                                                                                                        \
		{                                                                                                                                            \
			Nexus::Scripting::ScriptRegistry::GetRegistry()[#ClassType] = []() { return new ClassType(); };                                          \
		}                                                                                                                                            \
	};                                                                                                                                               \
	static ClassType##Register instance##ClassType##Register;