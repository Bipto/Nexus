#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Platform/Events/Event.hpp"

namespace Nexus
{
	template<typename T>
	concept EventType = std::is_base_of_v<Event, T>;

	template<EventType Message>
	using Handler = std::function<void(const Message &)>;

	class EventDispatcher
	{
	  public:
		using Handler = std::function<void(const Event &)>;

		template<EventType Message>
		void Subscribe(Handler handler)
		{
			static_assert(std::is_base_of_v<Event, Message>, "Message must derive from Event");

			std::vector<ErasedHandler> &handlers = m_Subscribers[typeid(Message)];
			handlers.push_back([handler](const void *msg) { handler(*static_cast<const Message &>(msg)); });
		}

		void Dispatch(const Event &message) const
		{
			auto it = m_Subscribers.find(typeid(message));
			if (it == m_Subscribers.end())
				return;

			for (const ErasedHandler &fn : it->second) { fn(message); }
		}

	  private:
		using ErasedHandler = std::function<void(const Event &)>;
		std::unordered_map<std::type_index, std::vector<ErasedHandler>> m_Subscribers;
	};
}	 // namespace Nexus
