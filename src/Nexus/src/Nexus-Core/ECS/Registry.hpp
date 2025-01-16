#pragma once

#include "Nexus-Core/Runtime/Entity.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::ECS
{
	struct ComponentPtr
	{
		void	   *data	 = nullptr;
		const char *typeName = nullptr;
	};

	class IComponentArray
	{
	  public:
		virtual ~IComponentArray()
		{
		}

		virtual size_t		 GetComponentCount()				= 0;
		virtual ComponentPtr GetAbstractComponent(size_t index) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	  public:
		virtual ~ComponentArray()
		{
		}

		size_t GetComponentCount() final
		{
			return m_Components.size();
		}

		ComponentPtr GetAbstractComponent(size_t index) final
		{
			const std::type_info &typeInfo = typeid(T);
			ComponentPtr		  ptr {.data = &m_Components[index], .typeName = typeInfo.name()};
			return ptr;
		}

		void AddComponent(const T &component)
		{
			m_Components.push_back(component);
		}

		T *GetComponent(size_t index)
		{
			return &m_Components[index];
		}

		bool IsValidComponent(size_t index)
		{
			if (index > m_Components.size() || index == 0)
			{
				return false;
			}

			return true;
		}

	  private:
		std::vector<T> m_Components = {};
	};

	template<typename... Args>
	class View
	{
	  public:
		View() = default;

		View(const std::vector<std::pair<Entity *, std::vector<std::tuple<Args *...>>>> &components) : m_EntityComponents(components)
		{
		}

	  private:
		std::vector<std::pair<Entity *, std::vector<std::tuple<Args *...>>>> m_EntityComponents = {};
	};

	class Registry
	{
	  public:
		Entity Create()
		{
			Entity entity = {};
			m_Entities.push_back(entity);
			return entity;
		}

		void AddEntity(const Entity &entity)
		{
			m_Entities.push_back(entity);
		}

		template<typename T>
		void AddComponent(const Entity &entity, T component)
		{
			const char		  *typeName	  = typeid(T).name();
			ComponentArray<T> *components = GetComponentArray<T>();
			size_t			   position	  = components->GetComponentCount();
			m_ComponentIds[entity.ID][typeName].push_back(position);
			components->AddComponent(component);
		}

		template<typename T>
		T *GetFirstOrNull(GUID guid)
		{
			const char				  *typeName			= typeid(T).name();
			const std::vector<size_t> &entityComponents = m_ComponentIds[guid][typeName];

			if (entityComponents.size() > 0)
			{
				ComponentArray<T> *c = GetComponentArray<T>();

				if (c->IsValidComponent(entityComponents[0]))
				{
					return c->GetComponent(entityComponents[0]);
				}
			}

			return nullptr;
		}

		std::vector<ComponentPtr> GetAllComponents(GUID guid)
		{
			std::vector<ComponentPtr> returnComponents;

			for (const auto &[id, entityComponents] : m_ComponentIds)
			{
				if (id != guid)
				{
					continue;
				}

				for (const auto &[name, components] : entityComponents)
				{
					IComponentArray *componentArray = GetBaseComponentArray(name);

					for (const auto &componentIndex : components)
					{
						int x = 0;
						// std::any component = componentArray->GetComponentAsAny(componentIndex);
						// returnComponents.push_back(component);
						// returnComponents.push_back(components.GetAbstractComponent(componentIndex));

						ComponentPtr ptr = componentArray->GetAbstractComponent(componentIndex);
						returnComponents.push_back(ptr);
					}
				}
			}

			return returnComponents;
		}

		template<typename... Args>
		std::tuple<Args *...> GetFirstOrNullComponents(GUID guid)
		{
			return std::make_tuple((Args *)GetFirstOrNull<Args>(guid)...);
		}

		template<typename T>
		std::vector<T *> GetComponentVector(GUID guid)
		{
			std::vector<T *>		   returnComponents = {};
			const char				  *typeName			= typeid(T).name();
			const std::vector<size_t> &entityComponents = m_ComponentIds[guid][typeName];

			for (size_t i = 0; i < entityComponents.size(); i++)
			{
				ComponentArray<T> *components = GetComponentArray<T>();
				for (size_t i = 0; i < components->GetComponentCount(); i++)
				{
					T *casted = components->GetComponent(i);
					returnComponents.push_back(casted);
				}
			}

			return returnComponents;
		}

		template<typename... Args>
		std::tuple<std::vector<Args *>...> GetAllOrEmpty(GUID guid)
		{
			return std::make_tuple(GetComponentVector<Args>(guid)...);
		}

		Entity *GetEntityOrNull(GUID guid)
		{
			for (size_t i = 0; i < m_Entities.size(); i++)
			{
				if (m_Entities[i].ID == guid)
				{
					return &m_Entities[i];
				}
			}

			return nullptr;
		}

		std::vector<Entity> &GetEntities()
		{
			return m_Entities;
		}

		template<typename... Args>
		View<Args...> GetView()
		{
			std::vector<std::pair<Entity *, std::vector<std::tuple<Args *...>>>> viewData;

			for (Entity &entity : m_Entities)
			{
				auto components = GetAllOrEmpty<Args...>(entity.ID);

				bool hasAllComponents = ((std::get<std::vector<Args *>>(components).size() > 0) && ...);
				if (hasAllComponents)
				{
					std::vector<std::tuple<Args *...>> entityComponents;
					size_t							   minSize = std::min({std::get<std::vector<Args *>>(components).size()...});
					for (size_t i = 0; i < minSize; ++i)
					{
						entityComponents.emplace_back(std::make_tuple(std::get<std::vector<Args *>>(components)[i]...));
					}
					viewData.push_back({&entity, entityComponents});
				}
			}

			return View<Args...>(viewData);
		}

	  private:
		template<typename T>
		ComponentArray<T> *GetComponentArray()
		{
			const std::type_info &typeInfo = typeid(T);
			if (m_Components.find(typeInfo.name()) == m_Components.end())
			{
				m_Components[typeInfo.name()] = std::make_unique<ComponentArray<T>>();
			}

			return (ComponentArray<T> *)m_Components[typeInfo.name()].get();
		}

		IComponentArray *GetBaseComponentArray(const char *typeName)
		{
			return m_Components[typeName].get();
		}

	  private:
		// vector of entities
		std::vector<Entity> m_Entities = {};

		// vector of components of each type
		std::map<const char *, std::unique_ptr<IComponentArray>> m_Components = {};

		// map of entity ownership
		std::map<GUID, std::map<const char *, std::vector<size_t>>> m_ComponentIds = {};
	};
}	 // namespace Nexus::ECS