#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "Nexus/Graphics/Buffer.hpp"

#include "Platform/Vulkan/ShaderVk.hpp"

namespace Nexus::Graphics
{
    class VertexBufferVk : public VertexBuffer
    {
    public:
        VertexBufferVk(const BufferDescription &description, const void *data, const VertexBufferLayout &layout, GraphicsDeviceVk *device);
        virtual ~VertexBufferVk();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        VkBuffer GetBuffer();

    private:
        AllocatedBuffer m_Buffer;
        GraphicsDeviceVk *m_Device;
    };

    class IndexBufferVk : public IndexBuffer
    {
    public:
        IndexBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device, IndexBufferFormat format);
        virtual ~IndexBufferVk();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        VkBuffer GetBuffer();

    private:
        AllocatedBuffer m_Buffer;
        GraphicsDeviceVk *m_Device;
    };

    class UniformBufferVk : public UniformBuffer
    {
    public:
        UniformBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device);
        virtual ~UniformBufferVk();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        VkBuffer GetBuffer();

    private:
        std::vector<AllocatedBuffer> m_Buffers;
        GraphicsDeviceVk *m_Device;
    };
}

#endif