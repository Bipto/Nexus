#pragma once

#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Scripting
{
	class Script
	{
	  public:
		Script() = default;
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

		virtual void OnTick(TimeSpan time)
		{
		}

		virtual void OnUnload()
		{
		}

		void Instantiate(Project *project, GUID guid)
		{
			m_Project = project;
			m_GUID	  = guid;
		}

	  private:
		Project *m_Project = nullptr;
		GUID	 m_GUID	   = Nexus::GUID(0);
	};

	class ScriptRegistry
	{
	  public:
		static std::map<std::string, std::function<Nexus::Scripting::Script *()>> &GetRegistry()
		{
			static std::map<std::string, std::function<Nexus::Scripting::Script *()>> registry;
			return registry;
		}
	};

	extern "C" inline NX_API std::map<std::string, std::function<Nexus::Scripting::Script *()>> &GetScriptRegistry()
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