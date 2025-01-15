#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Registry.hpp"

namespace Nexus::ECS
{
	struct ComponentSerializers
	{
		std::function<std::string(void *)>		   Serializer;
		std::function<void *(const std::string &)> Deserializer;
	};

	using CreateComponentFunc = std::function<void(Registry &registry, const Entity &entity)>;

	std::map<std::type_index, ComponentSerializers> m_RegisteredSerializers;
	std::map<const char *, CreateComponentFunc>		m_RegisteredComponentCreators;

	template<typename T>
	void RegisterComponent(const char *typeName)
	{
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
		m_RegisteredSerializers[typeid(T)] = serializers;

		m_RegisteredComponentCreators[typeName] = [](Registry &registry, const Entity &entity) { registry.AddComponent<T>(entity, T {}); };
	}

	template<typename T>
	std::string SerializeComponent(T &obj)
	{
		std::type_index typeIndex(typeid(T));
		if (m_RegisteredSerializers.find(typeIndex) != m_RegisteredSerializers.end())
		{
			const ComponentSerializers &serializer = m_RegisteredSerializers.at(typeIndex);
			return serializer.Serializer(&obj);
		}
		throw std::runtime_error("Type is not registed for serialization");
	}

	template<typename T>
	T DeserializeComponent(const std::string &data)
	{
		std::type_index typeIndex(typeid(T));
		if (m_RegisteredSerializers.find(typeIndex) != m_RegisteredSerializers.end())
		{
			const ComponentSerializers &registeredComponent = m_RegisteredSerializers.at(typeIndex);
			void					   *objPtr				= registeredComponent.Deserializer(data);
			T						   *actualObj			= static_cast<T *>(objPtr);
			T							result				= *actualObj;
			delete actualObj;
			return result;
		}

		throw std::runtime_error("Type is not registered for deserialization");
	}

	void CreateComponent(const char *typeName, Registry &registry, const Entity &entity)
	{
		if (m_RegisteredComponentCreators.find(typeName) != m_RegisteredComponentCreators.end())
		{
			m_RegisteredComponentCreators[typeName](registry, entity);
		}
	}

#define REGISTER_COMPONENT(T) Nexus::ECS::RegisterComponent<T>(#T)

}	 // namespace Nexus::ECS