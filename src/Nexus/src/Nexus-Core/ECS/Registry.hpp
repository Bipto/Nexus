#pragma once

#include "Nexus-Core/Runtime/Entity.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::ECS
{
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
			const char *typeName = typeid(T).name();
			size_t		position = m_Components[typeName].size();
			m_Components[typeName].push_back(component);
			m_ComponentIds[entity.ID][typeName].push_back(position);
		}

		template<typename T>
		T *GetFirstOrNull(GUID guid)
		{
			const char				  *typeName			= typeid(T).name();
			const std::vector<size_t> &entityComponents = m_ComponentIds[guid][typeName];

			if (entityComponents.size() > 0)
			{
				std::vector<std::any> &c = m_Components[typeName];

				std::any &type = c.at(entityComponents[0]);
				if (type.has_value())
				{
					T &casted = std::any_cast<T &>(type);
					return &casted;
				}
			}

			return nullptr;
		}

		std::vector<std::any *> GetAllComponents(GUID guid)
		{
			std::vector<std::any *> returnComponents;

			for (const auto &[id, entityComponents] : m_ComponentIds)
			{
				if (id != guid)
				{
					continue;
				}

				for (const auto &[name, components] : entityComponents)
				{
					for (const auto &componentIndex : components) { returnComponents.push_back(&m_Components[name][componentIndex]); }
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
				std::vector<std::any> &c	= m_Components[typeName];
				std::any			  &type = c.at(entityComponents[i]);

				if (type.has_value())
				{
					T &casted = std::any_cast<T &>(type);
					returnComponents.push_back(&casted);
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
		// vector of entities
		std::vector<Entity> m_Entities = {};

		// vector of components of each type
		std::map<const char *, std::vector<std::any>> m_Components = {};

		// map of entity ownership
		std::map<GUID, std::map<const char *, std::vector<size_t>>> m_ComponentIds = {};
	};
}	 // namespace Nexus::ECS