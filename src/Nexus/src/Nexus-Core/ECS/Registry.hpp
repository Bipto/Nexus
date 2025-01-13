#pragma once

#include "Nexus-Core/Runtime/Entity.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::ECS
{
	template<typename... Args>
	class View
	{
	  public:
	  private:
	};

	class Registry
	{
	  public:
		Entity Create()
		{
			return Entity {};
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
		T *GetFirstOrNull(const Entity &entity)
		{
			const char				  *typeName			= typeid(T).name();
			const std::vector<size_t> &entityComponents = m_ComponentIds[entity.ID][typeName];

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

		template<typename... Args>
		std::tuple<Args *...> GetFirstOrNullComponents(const Entity &entity)
		{
			return std::make_tuple((Args *)GetFirstOrNull<Args>(entity)...);
		}

		template<typename T>
		std::vector<T *> GetAllOrEmpty(const Entity &entity)
		{
			std::vector<T *> returnComponents = {};

			const char				  *typeName			= typeid(T).name();
			const std::vector<size_t> &entityComponents = m_ComponentIds[entity.ID][typeName];

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

	  private:
		// vector of components of each type
		std::map<const char *, std::vector<std::any>> m_Components = {};

		// map of entity ownership
		std::map<GUID, std::map<const char *, std::vector<size_t>>> m_ComponentIds = {};
	};
}	 // namespace Nexus::ECS