#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus
{
	using EventID = uint64_t;

	/// @brief A class that is used to call a group of functions when an event
	/// occurs
	/// @tparam Args The parameters to use when invoking the event functions
	template<typename... Args>
	class EventHandler
	{
		using EventFunction = std::function<void(Args...)>;

		struct BoundEvent
		{
			EventID		  ID   = {};
			EventFunction Func = {};
		};

	  public:
		/// @brief A method that calls the functions using a templated parameter
		/// @param param The payload to use for the functions
		NX_API void Invoke(Args... param)
		{
			for (const auto &[id, delegate] : m_EventFunctions) { delegate(param...); }
		}

		/// @brief A method to bind a new function to the event handler
		/// @param function The function to bind to the event handler
		NX_API EventID Bind(EventFunction function)
		{
			EventID	   id = Utils::GetCurrentTimeAsInt();
			BoundEvent event {.ID = id, .Func = function};
			m_EventFunctions.push_back(event);
			return id;
		}

		/// @brief A method that removes a function from the event handler
		/// @param id The ID of the function to unbind from the event handler
		NX_API void Unbind(EventID id)
		{
			for (size_t i = 0; i < m_EventFunctions.size(); i++)
			{
				if (m_EventFunctions[i].ID == id)
				{
					m_EventFunctions.erase(m_EventFunctions.begin() + i);
					i--;
				}
			}
		}

		/// @brief A method that returns the amount of delegates bound to the event
		/// handler
		/// @return The amount of delegates bound to the event handler
		NX_API int GetDelegateCount()
		{
			return m_EventFunctions.size();
		}

		NX_API void Clear()
		{
			m_EventFunctions.clear();
		}

	  private:
		/// @brief A vector containing the bound delegates to call
		std::vector<BoundEvent> m_EventFunctions;
	};

	template<typename... Args>
	class ScopedEvent
	{
	  public:
		NX_API ScopedEvent() = default;
		NX_API ScopedEvent(EventHandler<Args...> *eventHandler, std::function<void(Args...)> func) : m_EventHandler(eventHandler)
		{
			m_BoundEvent = eventHandler->Bind(func);
		}

		NX_API ~ScopedEvent()
		{
			if (m_EventHandler)
			{
				m_EventHandler->Unbind(m_BoundEvent);
			}
		}

	  private:
		EventHandler<Args...> *m_EventHandler = nullptr;
		EventID				   m_BoundEvent	  = 0;
	};
}	 // namespace Nexus