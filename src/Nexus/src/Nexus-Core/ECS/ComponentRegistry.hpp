#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Registry.hpp"

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"

#include "Nexus-Core/Utils/StringUtils.hpp"

namespace Nexus::ECS
{
	struct ComponentSerializers
	{
		std::function<std::string(void *)>		   Serializer;
		std::function<void *(const std::string &)> Deserializer;
	};

	using CreateComponentFunc = std::function<void(Registry &registry, const Entity &entity)>;
	using RenderComponentFunc = std::function<void(void *)>;

	std::map<const char *, ComponentSerializers> m_RegisteredSerializers;
	std::map<const char *, CreateComponentFunc>	 m_RegisteredComponentCreators;
	std::map<const char *, RenderComponentFunc>	 m_RegisteredComponentRenderFunctions;
	std::map<const char *, const char *>		 m_ComponentDisplayNames;

	const std::map<const char *, CreateComponentFunc> &GetAvailableComponents()
	{
		return m_RegisteredComponentCreators;
	}

	template<typename T>
	void RegisterComponent(const char *displayName)
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
										  .Deserializer = [](const std::string &data) -> void *
										  {
											  T					*obj = new T();
											  std::istringstream iss(data);
											  iss >> *obj;
											  return obj;
										  }};
		m_RegisteredSerializers[typeName] = serializers;

		m_RegisteredComponentCreators[typeName] = [](Registry &registry, const Entity &entity) { registry.AddComponent<T>(entity, T {}); };
		m_ComponentDisplayNames[typeName]		= displayName;
	}

	template<typename T>
	void RegisterComponentRenderFunc(RenderComponentFunc renderFunc)
	{
		const std::type_info &typeInfo				   = typeid(T);
		const char			 *typeName				   = typeInfo.name();
		m_RegisteredComponentRenderFunctions[typeName] = renderFunc;
	}

	template<typename T>
	std::string SerializeComponent(T &obj)
	{
		const std::type_info &typeInfo = typeid(T);
		const char			 *typeName = typeInfo.name();

		if (m_RegisteredSerializers.find(typeName) != m_RegisteredSerializers.end())
		{
			const ComponentSerializers &serializer = m_RegisteredSerializers.at(typeName);
			return serializer.Serializer(&obj);
		}
		throw std::runtime_error("Type is not registed for serialization");
	}

	template<typename T>
	T DeserializeComponent(const std::string &data)
	{
		const std::type_info &typeInfo = typeid(T);
		const char			 *typeName = typeInfo.name();
		if (m_RegisteredSerializers.find(typeName) != m_RegisteredSerializers.end())
		{
			const ComponentSerializers &registeredComponent = m_RegisteredSerializers.at(typeName);
			void					   *objPtr				= registeredComponent.Deserializer(data);
			T						   *actualObj			= static_cast<T *>(objPtr);
			T							result				= *actualObj;
			delete actualObj;
			return result;
		}

		throw std::runtime_error("Type is not registered for deserialization");
	}

	const char *GetDisplayNameFromTypeName(const char *typeName)
	{
		if (m_ComponentDisplayNames.find(typeName) != m_ComponentDisplayNames.end())
		{
			return m_ComponentDisplayNames[typeName];
		}

		throw std::runtime_error("Display name could not be found");
	}

	const char *GetTypeNameFromDisplayName(const char *displayName)
	{
		for (const auto &[typeName, displayName] : m_ComponentDisplayNames)
		{
			if (strcmp(typeName, displayName))
			{
				return typeName;
			}
		}

		throw std::runtime_error("Type does not have a display name");
	}

	void CreateComponent(const char *typeName, Registry &registry, const Entity &entity)
	{
		if (m_RegisteredComponentCreators.find(typeName) != m_RegisteredComponentCreators.end())
		{
			m_RegisteredComponentCreators[typeName](registry, entity);
			return;
		}

		throw std::runtime_error("Type is not registered for creation");
	}

	void RenderComponent(ComponentPtr component)
	{
		if (m_RegisteredComponentRenderFunctions.find(component.typeName) != m_RegisteredComponentRenderFunctions.end())
		{
			m_RegisteredComponentRenderFunctions[component.typeName](component.data);
			return;
		}

		throw std::runtime_error("Type is not registered for rendering");
	}

	const char *GetDisplayName(ComponentPtr component)
	{
		if (m_ComponentDisplayNames.find(component.typeName) != m_ComponentDisplayNames.end())
		{
			return m_ComponentDisplayNames[component.typeName];
		}

		throw std::runtime_error("Component does not have a display name");
	}

#define REGISTER_COMPONENT(Comp, DisplayName)					Nexus::ECS::RegisterComponent<Comp>(DisplayName)
#define REGISTER_COMPONENT_RENDER_FUNC(Comp, Func)				Nexus::ECS::RegisterComponentRenderFunc<Comp>(Func)
#define REGISTER_COMPONENT_WITH_RENDER_FUNC(Comp, DisplayName, Func)                                                                                 \
	REGISTER_COMPONENT(Comp, DisplayName);                                                                                                           \
	REGISTER_COMPONENT_RENDER_FUNC(Comp, Func)

}	 // namespace Nexus::ECS