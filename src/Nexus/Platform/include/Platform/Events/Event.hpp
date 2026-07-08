#pragma once

#include <cstdint>
#include <functional>

#include "Platform/Timings/DateTime.hpp"

namespace Nexus
{
    struct Event
    {
        virtual ~Event() = default;
    };

    template <typename T>
    concept EventType = std::is_base_of_v<Event, T>;

    template <EventType Message>
    using EventHandlerFunction = std::function<void(const Message &)>;
} // namespace Nexus