#pragma once

#include "Core/nxpch.h"
#include "Core/Vertex.h"

namespace Nexus::Graphics
{
    enum class BufferType
    {
        Invalid = 0,
        Vertex,
        Index,
        Uniform
    };

    enum class BufferUsage
    {
        Invalid = 0,
        Static,
        Dynamic
    };

    struct BufferDescription
    {
        uint32_t Size = 0;
        BufferType Type = BufferType::Invalid;
        BufferUsage Usage = BufferUsage::Invalid;
    };

    class DeviceBuffer
    {
    public:
        DeviceBuffer(const BufferDescription &description, const void *data)
            : m_Description(description) {}

        virtual void SetData(const void *data, uint32_t size, uint32_t offset) = 0;

        const BufferDescription &GetDescription() { return m_Description; }

    protected:
        BufferDescription m_Description;
    };
}