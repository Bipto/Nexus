#pragma once

#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Scripting
{
	class Script
	{
	  public:
		NX_API Script() = default;
		virtual ~Script()
		{
		}

		virtual void OnLoad()
		{
		}

		virtual void OnUpdate(TimeSpan time)
		{
		}

		virtual void OnRender(TimeSpan time)
		{
		}

		virtual void OnUnload()
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

	extern "C" inline NX_API std::map<std::string, std::function<Script *()>> &GetScriptRegistry()
	{
		return ScriptRegistry::GetRegistry();
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