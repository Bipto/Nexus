#pragma once

#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/Runtime.hpp"

namespace Nexus::ECS
{
	// forward declaration
	class ComponentRegistry;
}	 // namespace Nexus::ECS

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

		Project *GetProject() const
		{
			return m_Project;
		}

		Scene *GetCurrentScene() const
		{
			if (!m_Project)
			{
				return nullptr;
			}

			return m_Project->GetLoadedScene();
		}

		GUID GetEntityID() const
		{
			return m_GUID;
		}

		template<typename T>
		inline T *GetComponent(size_t index = 0)
		{
			Scene *currentScene = GetCurrentScene();
			return currentScene->Registry.GetComponent<T>(GetEntityID(), index);
		}

		virtual void OnUnload()
		{
		}

		inline void Instantiate(Project *project, GUID guid)
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

	extern "C" inline NX_API ECS::ComponentRegistry &GetComponentRegistry()
	{
		return ECS::ComponentRegistry::GetRegistry();
	}

	extern "C" inline NX_API void ShareEngineState(Nexus::Application *app,
												   ImGuiContext		  *context,
												   ImGuiMemAllocFunc   allocFunc,
												   ImGuiMemFreeFunc	   freeFunc)
	{
		/* Nexus::SetApplication(app);
		ImGui::SetAllocatorFunctions(allocFunc, freeFunc, nullptr);
		ImGui::SetCurrentContext(context); */
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