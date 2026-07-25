#pragma once

#include <gtest/gtest.h>

#include <cstddef>
#include <cstring>

#include "RHI/CommandList.hpp"

namespace TestHelpers
{

    inline constexpr size_t AlignUp(size_t value, size_t alignment)
    {
        return (value + alignment - 1) & ~(alignment - 1);
    }

    inline const Nexus::Graphics::CommandHeader *FirstCommand(const Nexus::Graphics::CommandListStorage &storage)
    {
        if (storage.CommandData.empty())
            return nullptr;

        return reinterpret_cast<const Nexus::Graphics::CommandHeader *>(storage.CommandData.data());
    }

    inline const Nexus::Graphics::CommandHeader *NextCommand(const Nexus::Graphics::CommandHeader *header)
    {
        return reinterpret_cast<const Nexus::Graphics::CommandHeader *>(
            reinterpret_cast<const std::byte *>(header) + header->Length
        );
    }

    template <typename T> const T *GetCommand(const Nexus::Graphics::CommandHeader *header)
    {
        auto base = reinterpret_cast<const std::byte *>(header);

        size_t commandOffset = AlignUp(sizeof(Nexus::Graphics::CommandHeader), alignof(T));

        return reinterpret_cast<const T *>(base + commandOffset);
    }

    template <typename T> const T *GetCommand(const Nexus::Graphics::CommandListStorage &storage)
    {
        return GetCommand<T>(FirstCommand(storage));
    }

    template <typename T>
    const T *GetCommand(
        const Nexus::Graphics::CommandListStorage &storage, const Nexus::Graphics::CommandHeader *header
    )
    {
        return GetCommand<T>(header);
    }

    template <typename Command, typename Payload>
    const Payload *GetPayloadAs(const Nexus::Graphics::CommandHeader *header)
    {
        auto AlignUp = [](uintptr_t value, size_t alignment) {
            return (value + alignment - 1) & ~(uintptr_t(alignment) - 1);
        };

        uintptr_t headerAddr = reinterpret_cast<uintptr_t>(header);

        uintptr_t commandAddr = AlignUp(headerAddr + sizeof(Nexus::Graphics::CommandHeader), alignof(Command));

        uintptr_t payloadAddr = AlignUp(commandAddr + sizeof(Command), alignof(std::max_align_t));

        return reinterpret_cast<const Payload *>(payloadAddr);
    }

    inline const std::byte *Payload(
        const Nexus::Graphics::CommandHeader *header, size_t commandSize, size_t commandAlignment
    )
    {
        auto AlignUp = [](uintptr_t value, size_t alignment) {
            return (value + alignment - 1) & ~(uintptr_t(alignment) - 1);
        };

        uintptr_t headerAddr = reinterpret_cast<uintptr_t>(header);

        uintptr_t commandAddr = AlignUp(headerAddr + sizeof(Nexus::Graphics::CommandHeader), commandAlignment);

        uintptr_t payloadAddr = AlignUp(commandAddr + commandSize, alignof(std::max_align_t));

        return reinterpret_cast<const std::byte *>(payloadAddr);
    }

    inline std::string PayloadString(
        const Nexus::Graphics::CommandHeader *header, size_t commandSize, size_t commandAlignment, size_t length
    )
    {
        auto ptr = reinterpret_cast<const char *>(Payload(header, commandSize, commandAlignment));

        return std::string(ptr, length);
    }

} // namespace TestHelpers