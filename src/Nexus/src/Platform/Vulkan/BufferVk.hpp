#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "Nexus-Core/Graphics/GPUBuffer.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceVk;

    class VertexBufferVk : public VertexBuffer
    {
    public:
        VertexBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device);
        virtual ~VertexBufferVk();
        VkBuffer GetBuffer();

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

    private:
        AllocatedBuffer m_Buffer;
        GraphicsDeviceVk *m_Device;
    };

    class IndexBufferVk : public IndexBuffer
    {
    public:
        IndexBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device, IndexBufferFormat format);
        virtual ~IndexBufferVk();
        VkBuffer GetBuffer();

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

    private:
        AllocatedBuffer m_Buffer;
        GraphicsDeviceVk *m_Device;
    };

    class UniformBufferVk : public UniformBuffer
    {
    public:
        UniformBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device);
        virtual ~UniformBufferVk();
        VkBuffer GetBuffer();

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

    private:
        std::vector<AllocatedBuffer> m_Buffers;
        GraphicsDeviceVk *m_Device;
    };
}

#endif