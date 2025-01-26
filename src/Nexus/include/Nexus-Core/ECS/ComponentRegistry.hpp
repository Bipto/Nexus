#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"

#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Utils/StringUtils.hpp"

namespace Nexus::ECS
{
	using CreateComponentFunc = std::function<void(Registry &registry, const Entity &entity)>;
	using RenderComponentFunc = std::function<void(void *data, Nexus::Ref<Nexus::Project> project)>;

	struct ComponentStorage
	{
		std::function<std::string(void *obj)>										Serializer		 = {};
		std::function<void(GUID guid, Registry &registry, const std::string &data)> Deserializer	 = {};
		CreateComponentFunc															CreationFunction = {};
		RenderComponentFunc															RenderFunc		 = {};
		std::string																	DisplayName		 = {};
	};

	class ComponentRegistry
	{
	  public:
		static ComponentRegistry &GetRegistry()
		{
			static ComponentRegistry registry;
			return registry;
		}

		template<typename T>
		void RegisterComponent(const char *displayName, RenderComponentFunc renderFunc)
		{
			const std::type_info &typeInfo = typeid(T);
			const char			 *typeName = typeInfo.name();

			ComponentStorage storage = {};
			storage.Serializer		 = [](void *obj) -> std::string
			{
				T				  *actualObj = static_cast<T *>(obj);
				std::ostringstream oss;
				oss << *actualObj;
				return oss.str();
			};
			storage.Deserializer = [](GUID guid, Registry &registry, const std::string &data)
			{
				T				   obj {};
				std::istringstream iss(data);
				iss >> obj;

				registry.AddComponent(guid, obj);
			};
			storage.CreationFunction = [](Registry &registry, const Entity &entity) { registry.AddComponent<T>(entity.ID, T {}); };
			storage.DisplayName		 = displayName;
			storage.RenderFunc		 = renderFunc;

			m_RegisteredComponents[typeName] = storage;
		}

		std::string SerializeComponent(Registry &registry, ComponentPtr component)
		{
			const char *typeName = component.typeName;

			if (m_RegisteredComponents.find(typeName) != m_RegisteredComponents.end())
			{
				auto &componentStorage = m_RegisteredComponents.at(typeName);
				void *obj			   = registry.GetRawComponent(component.typeName, component.componentIndex);
				return componentStorage.Serializer(obj);
			}

			throw std::runtime_error("Type is not registed for serialization");
		}

		void DeserializeComponent(Registry &registry, GUID guid, const std::string &displayName, const std::string &data)
		{
			std::string typeName = GetTypeNameFromDisplayName(displayName);

			if (m_RegisteredComponents.find(typeName.c_str()) != m_RegisteredComponents.end())
			{
				auto &storage = m_RegisteredComponents.at(typeName.c_str());
				storage.Deserializer(guid, registry, data);
				return;
			}

			throw std::runtime_error("Type is not registered for deserialization");
		}

		std::string GetDisplayNameFromTypeName(const std::string &typeName)
		{
			if (m_RegisteredComponents.find(typeName) != m_RegisteredComponents.end())
			{
				return m_RegisteredComponents.at(typeName).DisplayName;
			}

			throw std::runtime_error("Display name could not be found");
		}

		std::string GetTypeNameFromDisplayName(const std::string &displayName)
		{
			for (auto &[typeName, storage] : m_RegisteredComponents)
			{
				if (storage.DisplayName == displayName)
				{
					return typeName;
				}
			}

			throw std::runtime_error("Type does not have a display name");
		}

		void CreateComponent(const char *typeName, Registry &registry, const Entity &entity)
		{
			if (m_RegisteredComponents.find(typeName) != m_RegisteredComponents.end())
			{
				auto &storage = m_RegisteredComponents.at(typeName);
				storage.CreationFunction(registry, entity);
				return;
			}

			throw std::runtime_error("Type is not registered for creation");
		}

		void RenderComponent(Registry &registry, ComponentPtr component, Nexus::Ref<Nexus::Project> project)
		{
			if (m_RegisteredComponents.find(component.typeName) != m_RegisteredComponents.end())
			{
				auto &storage = m_RegisteredComponents.at(component.typeName);
				void *obj	  = registry.GetRawComponent(component.typeName, component.componentIndex);
				storage.RenderFunc(obj, project);
				return;
			}

			throw std::runtime_error("Type is not registered for rendering");
		}

		std::string GetDisplayName(ComponentPtr component)
		{
			if (m_RegisteredComponents.find(component.typeName) != m_RegisteredComponents.end())
			{
				auto &storage = m_RegisteredComponents.at(component.typeName);
				return storage.DisplayName;
			}

			throw std::runtime_error("Component does not have a display name");
		}

		std::map<std::string, ComponentStorage> &GetRegisteredComponents()
		{
			return m_RegisteredComponents;
		}

	  private:
		ComponentRegistry()
		{
		}

	  private:
		std::map<std::string, ComponentStorage> m_RegisteredComponents = {};
	};

#define NX_REGISTER_COMPONENT_WITH_CUSTOM_NAME(Comp, DisplayName, RenderFunc)                                                                        \
	struct Comp##Register                                                                                                                            \
	{                                                                                                                                                \
		Comp##Register()                                                                                                                             \
		{                                                                                                                                            \
			Nexus::ECS::ComponentRegistry &registry = Nexus::ECS::ComponentRegistry::GetRegistry();                                                  \
			registry.RegisterComponent<Comp>(DisplayName, RenderFunc);                                                                               \
		}                                                                                                                                            \
	};                                                                                                                                               \
	static Comp##Register instance##Comp##Register;

#define NX_REGISTER_COMPONENT(Comp, RenderFunc) NX_REGISTER_COMPONENT_WITH_CUSTOM_NAME(Comp, #Comp, RenderFunc)

}	 // namespace Nexus::ECS