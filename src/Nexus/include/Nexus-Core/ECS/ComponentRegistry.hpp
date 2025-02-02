#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"

#include "Nexus-Core/ECS/Registry.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/Utils/StringUtils.hpp"

#include "yaml-cpp/yaml.h"

namespace Nexus
{
	// forward declaration
	class Project;
	struct Entity;
}	 // namespace Nexus

namespace Nexus::ECS
{
	using CreateComponentFunc	 = std::function<void(Registry &registry, const Entity &entity)>;
	using RenderComponentFunc	 = std::function<void(void *data, Nexus::Ref<Nexus::Project> project)>;
	using StringSerializerFunc	 = std::function<std::string(void *obj)>;
	using StringDeserializerFunc = std::function<void(GUID guid, Registry &registry, const std::string &data, size_t entityHierarchyIndex)>;
	using YamlSerializerFunc	 = std::function<YAML::Node(void *obj)>;
	using YamlDeserializerFunc	 = std::function<void(GUID guid, Registry &registry, const YAML::Node &node, size_t entityHierarchyIndex)>;

	struct ComponentStorage
	{
		StringSerializerFunc   StringSerializer	  = {};
		StringDeserializerFunc StringDeserializer = {};
		YamlSerializerFunc	   YamlSerializer	  = {};
		YamlDeserializerFunc   YamlDeserializer	  = {};
		CreateComponentFunc	   CreationFunction	  = {};
		RenderComponentFunc	   RenderFunc		  = {};
		std::string			   DisplayName		  = {};
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
			storage.StringSerializer = [](void *obj) -> std::string
			{
				T				  *actualObj = static_cast<T *>(obj);
				std::ostringstream oss;
				oss << *actualObj;
				return oss.str();
			};
			storage.StringDeserializer = [](GUID guid, Registry &registry, const std::string &data, size_t entityHierarchyIndex)
			{
				T				   obj {};
				std::istringstream iss(data);
				iss >> obj;
				registry.AddComponent(guid, obj, entityHierarchyIndex);
			};
			storage.YamlSerializer = [](void *obj) -> YAML::Node
			{
				T		  *actualObj = static_cast<T *>(obj);
				YAML::Node node = YAML::convert<T>::encode(*actualObj);
				return node;
			};
			storage.YamlDeserializer = [](GUID guid, Registry &registry, const YAML::Node &node, size_t entityHierarchyIndex)
			{
				T obj = node.as<T>();
				registry.AddComponent(guid, obj, entityHierarchyIndex);
			};
			storage.CreationFunction = [](Registry &registry, const Entity &entity) { registry.AddComponent<T>(entity.ID, T {}); };
			storage.DisplayName		 = displayName;
			storage.RenderFunc		 = renderFunc;

			m_RegisteredComponents[typeName] = storage;
		}

		std::string SerializeComponentToString(Registry &registry, ComponentPtr component)
		{
			const char *typeName = component.typeName;

			if (m_RegisteredComponents.find(typeName) != m_RegisteredComponents.end())
			{
				auto &componentStorage = m_RegisteredComponents.at(typeName);
				void *obj			   = registry.GetRawComponent(component.typeName, component.componentIndex);
				return componentStorage.StringSerializer(obj);
			}

			throw std::runtime_error("Type is not registed for serialization");
		}

		void DeserializeComponentFromString(Registry		  &registry,
											GUID			   guid,
											const std::string &displayName,
											const std::string &data,
											size_t			   entityHierarchyIndex)
		{
			std::string typeName = GetTypeNameFromDisplayName(displayName);

			if (m_RegisteredComponents.find(typeName.c_str()) != m_RegisteredComponents.end())
			{
				auto &storage = m_RegisteredComponents.at(typeName.c_str());
				storage.StringDeserializer(guid, registry, data, entityHierarchyIndex);
				return;
			}

			throw std::runtime_error("Type is not registered for deserialization");
		}

		YAML::Node SerializeComponentToYaml(Registry &registry, ComponentPtr component)
		{
			const char *typeName = component.typeName;

			if (m_RegisteredComponents.find(typeName) != m_RegisteredComponents.end())
			{
				auto &componentStorage = m_RegisteredComponents.at(typeName);
				void *obj			   = registry.GetRawComponent(component.typeName, component.componentIndex);
				return componentStorage.YamlSerializer(obj);
			}

			throw std::runtime_error("Type is not registered for serialization");
		}

		void DeserializeComponentFromYaml(Registry			&registry,
										  GUID				 guid,
										  const std::string &displayName,
										  const YAML::Node	&node,
										  size_t			 entityHierarchyIndex)
		{
			std::string typeName = GetTypeNameFromDisplayName(displayName);

			if (m_RegisteredComponents.find(typeName.c_str()) != m_RegisteredComponents.end())
			{
				auto &storage = m_RegisteredComponents.at(typeName.c_str());
				storage.YamlDeserializer(guid, registry, node, entityHierarchyIndex);
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