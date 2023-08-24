#pragma once

#include "Vk.hpp"
#include "Core/Graphics/Buffer.hpp"

#include "Platform/Vulkan/ShaderVk.hpp"

namespace Nexus::Graphics
{
    class VertexBufferVk : public VertexBuffer
    {
    public:
        VertexBufferVk(const BufferDescription &description, const void *data, const VertexBufferLayout &layout, GraphicsDeviceVk *device);
        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;
        VkBuffer GetBuffer();

    private:
        AllocatedBuffer m_Buffer;
        GraphicsDeviceVk* m_Device;
    };

    class IndexBufferVk : public IndexBuffer
    {
    public:
        IndexBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device);
        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;
        VkBuffer GetBuffer();

    private:
        AllocatedBuffer m_Buffer;
        GraphicsDeviceVk* m_Device;
    };

    class UniformBufferVk : public UniformBuffer
    {
    public:
        UniformBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device);
        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;
        VkBuffer GetBuffer();

    private:
        AllocatedBuffer m_Buffer;
        GraphicsDeviceVk* m_Device;
    };
}