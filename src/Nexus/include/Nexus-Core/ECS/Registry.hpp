#pragma once

#include "Nexus-Core/Runtime/Entity.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::ECS
{
	struct ComponentPtr
	{
		const char *typeName	   = nullptr;
		size_t		componentIndex = 0;
	};

	class IComponentArray
	{
	  public:
		virtual ~IComponentArray()
		{
		}

		virtual size_t		 GetComponentCount()				= 0;
		virtual ComponentPtr GetAbstractComponent(size_t index) = 0;
		virtual void		*GetRawComponent(size_t index)		= 0;
		virtual void		 RemoveComponent(size_t index)		= 0;
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
			std::string			  typeName = typeInfo.name();

			ComponentPtr ptr {.typeName = typeInfo.name(), .componentIndex = index};
			return ptr;
		}

		void *GetRawComponent(size_t index) final
		{
			return &m_Components[index];
		}

		virtual void RemoveComponent(size_t index) final
		{
			m_Components.erase(m_Components.begin() + index);
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
		// iterator for a view
		class iterator
		{
		  public:
			using IteratorCategory = std::forward_iterator_tag;
			using difference_type  = std::ptrdiff_t;
			using value_type	   = std::pair<Entity *, std::vector<std::tuple<Args *...>>>;
			using pointer		   = value_type *;
			using reference		   = value_type &;

			iterator(pointer ptr) : m_Pointer(ptr)
			{
			}

			reference operator*() const
			{
				return *m_Pointer;
			}

			pointer operator->()
			{
				return m_Pointer;
			}

			iterator &operator++()
			{
				m_Pointer++;
				return *this;
			}

			iterator operator++(int)
			{
				iterator temp = *this;
				++(*this);
				return temp;
			}

			friend bool operator==(const iterator &a, const iterator &b)
			{
				return a.m_Pointer == b.m_Pointer;
			}

			friend bool operator!=(const iterator &a, const iterator &b)
			{
				return a.m_Pointer != b.m_Pointer;
			}

		  private:
			pointer m_Pointer = nullptr;
		};

	  public:
		View() = default;

		View(const std::vector<std::pair<Entity *, std::vector<std::tuple<Args *...>>>> &components) : m_EntityComponents(components)
		{
		}

		iterator begin()
		{
			return iterator(&m_EntityComponents[0]);
		}

		iterator end()
		{
			return iterator(&m_EntityComponents[0] + m_EntityComponents.size());
		}

		std::vector<Entity *> GetEntities() const
		{
			std::vector<Entity *> entities;
			for (const auto &[entity, component] : m_EntityComponents) { entities.push_back(entity); }
			return entities;
		}

		std::vector<std::tuple<Args *...>> GetComponents(Entity *entity) const
		{
			for (const auto &[entityPtr, components] : m_EntityComponents)
			{
				if (entity->ID.Value == entityPtr->ID.Value)
				{
					return components;
				}
			}

			return {};
		}

		template<typename Func>
		void Each(Func func)
		{
			for (auto &[entity, components] : m_EntityComponents)
			{
				for (const auto &component : components)
				{
					int x = 0;
					func(entity, component);
				}
			}
		}

		bool HasComponents()
		{
			return m_EntityComponents.size() > 0;
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
		void AddComponent(GUID guid, T component)
		{
			const char		  *typeName	  = typeid(T).name();
			ComponentArray<T> *components = GetComponentArray<T>();
			size_t			   position	  = components->GetComponentCount();
			m_ComponentIds[guid][typeName].push_back(position);
			components->AddComponent(component);
		}

		template<typename T>
		T *GetComponent(size_t index)
		{
			const char		  *typeName	  = typeid(T).name();
			ComponentArray<T> *components = GetComponentArray<T>();

			if (index > components->GetComponentCount())
			{
				return nullptr;
			}

			return components->GetComponent(index);
		}

		template<typename T>
		T *GetComponent(GUID id, size_t index = 0)
		{
			const char *typeName = typeid(T).name();
			if (m_ComponentIds.find(id) == m_ComponentIds.end())
			{
				return nullptr;
			}

			const std::vector<size_t> &componentIndices = m_ComponentIds[id][typeName];
			if (componentIndices.empty() || componentIndices.size() < index)
			{
				return nullptr;
			}

			ComponentArray<T> *componentArray = GetComponentArray<T>();
			return componentArray->GetComponent(componentIndices[index]);
		}

		void *GetRawComponent(const std::string &typeName, size_t index)
		{
			IComponentArray *components = GetBaseComponentArray(typeName.c_str());
			return components->GetRawComponent(index);
		}

		void *GetRawComponent(ComponentPtr component)
		{
			return GetRawComponent(component.typeName, component.componentIndex);
		}

		void RemoveComponent(GUID guid, const std::string name, size_t index)
		{
			IComponentArray *components = GetBaseComponentArray(name.c_str());
			if (!components)
			{
				return;
			}

			components->RemoveComponent(index);

			std::vector<size_t> &ids = m_ComponentIds[guid][name.c_str()];

			size_t removeIndex = 0;
			for (const auto &id : ids)
			{
				if (id == index)
				{
					break;
				}

				removeIndex++;
			}

			ids.erase(ids.begin() + removeIndex);
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

					if (!componentArray)
					{
						continue;
						;
					}

					for (const auto &componentIndex : components)
					{
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

			// the problem is here somewhere...
			ComponentArray<T> *components = GetComponentArray<T>();
			for (size_t i = 0; i < entityComponents.size(); i++)
			{
				for (size_t i = 0; i < components->GetComponentCount(); i++)
				{
					if (std::find(entityComponents.begin(), entityComponents.end(), i) != entityComponents.end())
					{
						T *casted = components->GetComponent(i);
						returnComponents.push_back(casted);
					}
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

					for (size_t i = 0; i < minSize; i++)
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
				m_Components[typeInfo.name()] = new ComponentArray<T>();
			}

			IComponentArray *components = m_Components[typeInfo.name()];

			return (ComponentArray<T> *)components;
		}

		IComponentArray *GetBaseComponentArray(const std::string &typeName)
		{
			if (m_Components.find(typeName) == m_Components.end())
			{
				return nullptr;
			}

			return m_Components[typeName];
		}

	  private:
		// vector of entities
		std::vector<Entity> m_Entities = {};

		// vector of components of each type
		std::map<std::string, IComponentArray *> m_Components = {};

		// map of entity ownership
		std::map<GUID, std::map<std::string, std::vector<size_t>>> m_ComponentIds = {};
	};
}	 // namespace Nexus::ECS