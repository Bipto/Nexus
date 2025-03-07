#pragma once

#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/Runtime.hpp"

#if defined(WIN32)
	#define NX_SCRIPT_EXPORT __declspec(dllexport)
#else
	#define NX_SCRIPT_EXPORT
#endif

namespace Nexus::ECS
{
	// forward declaration
	class ComponentRegistry;
}	 // namespace Nexus::ECS

namespace Nexus::Scripting
{
	class NativeScript
	{
	  public:
		NativeScript() = default;
		virtual ~NativeScript()
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

	class NativeScriptRegistry
	{
	  public:
		static std::map<std::string, std::function<Nexus::Scripting::NativeScript *()>> &GetRegistry()
		{
			static std::map<std::string, std::function<Nexus::Scripting::NativeScript *()>> registry;
			return registry;
		}
	};

	extern "C" inline NX_SCRIPT_EXPORT std::map<std::string, std::function<Nexus::Scripting::NativeScript *()>> &GetScriptRegistry()
	{
		return NativeScriptRegistry::GetRegistry();
	}

	extern "C" inline NX_SCRIPT_EXPORT ECS::ComponentRegistry &GetComponentRegistry()
	{
		return ECS::ComponentRegistry::GetRegistry();
	}

	extern "C" inline NX_SCRIPT_EXPORT void ShareEngineState(Nexus::Application *app,
															 ImGuiContext		*context,
															 ImGuiMemAllocFunc	 allocFunc,
															 ImGuiMemFreeFunc	 freeFunc)
	{
		Nexus::SetApplication(app);
		ImGui::SetAllocatorFunctions(allocFunc, freeFunc, nullptr);
		ImGui::SetCurrentContext(context);
		ImGuizmo::SetImGuiContext(context);
	}

}	 // namespace Nexus::Scripting

#define NX_REGISTER_SCRIPT(ClassType)                                                                                                                \
	struct ClassType##Register                                                                                                                       \
	{                                                                                                                                                \
		ClassType##Register()                                                                                                                        \
		{                                                                                                                                            \
			Nexus::Scripting::NativeScriptRegistry::GetRegistry()[#ClassType] = []() { return new ClassType(); };                                    \
		}                                                                                                                                            \
	};                                                                                                                                               \
	static ClassType##Register instance##ClassType##Register;