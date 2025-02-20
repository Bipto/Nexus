#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"

#include "Nexus-Core/ECS/Registry.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/Utils/StringUtils.hpp"

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"
#include "yaml-cpp/yaml.h"

namespace Nexus
{
	// forward declaration
	class Project;
	struct Entity;
}	 // namespace Nexus

namespace Nexus::ECS
{
	template<typename T>
	struct DataTable
	{
		std::string Key	  = {};
		T			Value = {};
	};

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