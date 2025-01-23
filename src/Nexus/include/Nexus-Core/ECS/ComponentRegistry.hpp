#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Registry.hpp"

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"

#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Utils/StringUtils.hpp"

namespace Nexus::ECS
{
	struct ComponentSerializers
	{
		std::function<std::string(void *obj)>										Serializer;
		std::function<void(GUID guid, Registry &registry, const std::string &data)> Deserializer;
	};

	using CreateComponentFunc = std::function<void(Registry &registry, const Entity &entity)>;
	using RenderComponentFunc = std::function<void(void *data, Nexus::Ref<Nexus::Project> project)>;

	inline std::map<std::string, ComponentSerializers> m_RegisteredSerializers;
	inline std::map<std::string, CreateComponentFunc>  m_RegisteredComponentCreators;
	inline std::map<std::string, RenderComponentFunc>  m_RegisteredComponentRenderFunctions;
	inline std::map<std::string, std::string>		   m_ComponentDisplayNames;

	inline const std::map<std::string, CreateComponentFunc> &GetAvailableComponents()
	{
		return m_RegisteredComponentCreators;
	}

	template<typename T>
	inline void RegisterComponent(const char *displayName)
	{
		const std::type_info &typeInfo = typeid(T);
		const char			 *typeName = typeInfo.name();

		ComponentSerializers serializers {.Serializer = [](void *obj) -> std::string
										  {
											  T					*actualObj = static_cast<T *>(obj);
											  std::ostringstream oss;
											  oss << *actualObj;
											  return oss.str();
										  },
										  .Deserializer =
											  [](GUID guid, Registry &registry, const std::string &data)
										  {
											  T					 obj {};
											  std::istringstream iss(data);
											  iss >> obj;

											  registry.AddComponent(guid, obj);
										  }};
		m_RegisteredSerializers[typeName] = serializers;

		m_RegisteredComponentCreators[typeName] = [](Registry &registry, const Entity &entity) { registry.AddComponent<T>(entity.ID, T {}); };
		m_ComponentDisplayNames[typeName]		= displayName;
	}

	template<typename T>
	inline void RegisterComponentRenderFunc(RenderComponentFunc renderFunc)
	{
		const std::type_info &typeInfo				   = typeid(T);
		const char			 *typeName				   = typeInfo.name();
		m_RegisteredComponentRenderFunctions[typeName] = renderFunc;
	}

	template<typename T>
	inline void RegisterComponentWithRenderFunc(const char *displayName, RenderComponentFunc renderFunc)
	{
		RegisterComponent<T>(displayName);
		RegisterComponentRenderFunc<T>(renderFunc);
	}

	inline std::string SerializeComponent(Registry &registry, ComponentPtr component)
	{
		const char *typeName = component.typeName;

		if (m_RegisteredSerializers.find(typeName) != m_RegisteredSerializers.end())
		{
			void *obj = registry.GetRawComponent(component.typeName, component.componentIndex);

			const ComponentSerializers &serializer = m_RegisteredSerializers.at(typeName);
			return serializer.Serializer(obj);
		}
		throw std::runtime_error("Type is not registed for serialization");
	}

	inline void DeserializeComponent(Registry &registry, GUID guid, const std::string &name, const std::string &data)
	{
		if (m_RegisteredSerializers.find(name.c_str()) != m_RegisteredSerializers.end())
		{
			const ComponentSerializers &serializer = m_RegisteredSerializers.at(name.c_str());
			serializer.Deserializer(guid, registry, data);
			return;
		}

		throw std::runtime_error("Type is not registered for deserialization");
	}

	inline std::string GetDisplayNameFromTypeName(const std::string &typeName)
	{
		if (m_ComponentDisplayNames.find(typeName) != m_ComponentDisplayNames.end())
		{
			return m_ComponentDisplayNames[typeName];
		}

		throw std::runtime_error("Display name could not be found");
	}

	inline std::string GetTypeNameFromDisplayName(const std::string &displayName)
	{
		for (const auto &[typeName, matchedDisplayName] : m_ComponentDisplayNames)
		{
			if (typeName == displayName)
			{
				return typeName;
			}
		}

		throw std::runtime_error("Type does not have a display name");
	}

	inline void CreateComponent(const char *typeName, Registry &registry, const Entity &entity)
	{
		if (m_RegisteredComponentCreators.find(typeName) != m_RegisteredComponentCreators.end())
		{
			m_RegisteredComponentCreators[typeName](registry, entity);
			return;
		}

		throw std::runtime_error("Type is not registered for creation");
	}

	inline void RenderComponent(Registry &registry, ComponentPtr component, Nexus::Ref<Nexus::Project> project)
	{
		if (m_RegisteredComponentRenderFunctions.find(component.typeName) != m_RegisteredComponentRenderFunctions.end())
		{
			void *obj = registry.GetRawComponent(component.typeName, component.componentIndex);
			m_RegisteredComponentRenderFunctions[component.typeName](obj, project);
			return;
		}

		throw std::runtime_error("Type is not registered for rendering");
	}

	inline std::string GetDisplayName(ComponentPtr component)
	{
		if (m_ComponentDisplayNames.find(component.typeName) != m_ComponentDisplayNames.end())
		{
			return m_ComponentDisplayNames[component.typeName];
		}

		throw std::runtime_error("Component does not have a display name");
	}

#define REGISTER_COMPONENT(Comp, DisplayName)	   Nexus::ECS::RegisterComponent<Comp>(DisplayName)
#define REGISTER_COMPONENT_RENDER_FUNC(Comp, Func) Nexus::ECS::RegisterComponentRenderFunc<Comp>(Func)
#define REGISTER_COMPONENT_WITH_RENDER_FUNC(Comp, DisplayName, Func) Nexus::ECS::RegisterComponentWithRenderFunc<Comp>(DisplayName, Func);

}	 // namespace Nexus::ECS