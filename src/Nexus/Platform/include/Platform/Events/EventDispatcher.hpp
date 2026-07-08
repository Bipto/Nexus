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
    class EventDispatcher
    {
      public:
        template <EventType Message>
        void Subscribe(EventHandlerFunction<Message> handler)
        {
            // Store erased handler
            auto &handlers = m_Subscribers[typeid(Message)];

            handlers.push_back([handler](const Event &e) {
                handler(static_cast<const Message &>(e));
            });
        }

        void Dispatch(const Event &message) const
        {
            auto it = m_Subscribers.find(typeid(message));
            if (it == m_Subscribers.end())
                return;

            for (const auto &fn : it->second)
                fn(message);
        }

      private:
        using ErasedHandler = std::function<void(const Event &)>;
        std::unordered_map<std::type_index, std::vector<ErasedHandler>>
            m_Subscribers;
    };
} // namespace Nexus