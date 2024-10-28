#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	enum class EventPermissions
	{
		Default,
		Locked
	};

	/// @brief A class that is used to call a group of functions when an event
	/// occurs
	/// @tparam Args The parameters to use when invoking the event functions
	template<typename... Args>
	class EventHandler
	{
		using EventFunction = std::function<void(Args...)>;

	  public:
		/// @brief A method that calls the functions using a templated parameter
		/// @param param The payload to use for the functions
		void Invoke(Args... param)
		{
			for (const auto &[delegate, permissions] : m_EventFunctions) { delegate(param...); }
		}

		/// @brief A method to bind a new function to the event handler
		/// @param function The function to bind to the event handler
		void Bind(EventFunction function, EventPermissions permissions = EventPermissions::Default)
		{
			m_EventFunctions.emplace_back(function, permissions);
		}

		/// @brief A method that removes a function from the event handler
		/// @param function The function to unbind from the event handler
		void Unbind(EventFunction function)
		{
			auto position = std::find(m_EventFunctions.begin(), m_EventFunctions.end(), function);
			if (position != m_EventFunctions.end())
				m_EventFunctions.erase(position);
		}

		/// @brief A custom operator that can be used to bind a function to the event
		/// handler
		/// @param function The function to bind to the event handler
		/// @return The new event handler with the function bound
		EventHandler &operator+=(EventFunction function)
		{
			Bind(function, EventPermissions::Default);
			return *this;
		}

		/// @brief A custom operator that can be used to unbind a function from the
		/// event handler
		/// @param function The functio nto unbind from the event handler
		/// @return The new event handler with the function unbound
		EventHandler &operator-=(EventFunction function)
		{
			Unbind(function);
			return *this;
		}

		/// @brief A method that returns the amount of delegates bound to the event
		/// handler
		/// @return The amount of delegates bound to the event handler
		int GetDelegateCount()
		{
			return m_EventFunctions.size();
		}

		void Clear()
		{
			for (auto it = m_EventFunctions.begin(); it != m_EventFunctions.end();)
			{
				if (it->second == EventPermissions::Default)
				{
					it = m_EventFunctions.erase(it);
				}
			}
		}

	  private:
		/// @brief A vector containing the bound delegates to call
		std::vector<std::pair<EventFunction, EventPermissions>> m_EventFunctions;
	};
}	 // namespace Nexus