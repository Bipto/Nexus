#include "BufferVk.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    VertexBufferVk::VertexBufferVk(const BufferDescription &description, const void *data, const VertexBufferLayout &layout, GraphicsDeviceVk *device)
        : VertexBuffer(description, data, layout), m_Device(device)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = description.Size;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        if (vmaCreateBuffer(device->GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer");
        }

        void *buffer;
        vmaMapMemory(device->GetAllocator(), m_Buffer.Allocation, &buffer);
        memcpy(buffer, data, description.Size);
        vmaUnmapMemory(device->GetAllocator(), m_Buffer.Allocation);
    }

    void *VertexBufferVk::Map(MapMode mode)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);
        return buffer;
    }

    void VertexBufferVk::Unmap()
    {
        vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer.Allocation);
    }

    VkBuffer VertexBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }

    IndexBufferVk::IndexBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device)
        : IndexBuffer(description, data), m_Device(device)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = description.Size;
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        if (vmaCreateBuffer(device->GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create index buffer");
        }

        void *buffer;
        vmaMapMemory(device->GetAllocator(), m_Buffer.Allocation, &buffer);
        memcpy(buffer, data, description.Size);
        vmaUnmapMemory(device->GetAllocator(), m_Buffer.Allocation);
    }

    void *IndexBufferVk::Map(MapMode mode)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);
        return buffer;
    }

    void IndexBufferVk::Unmap()
    {
        vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer.Allocation);
    }

    VkBuffer IndexBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }

    UniformBufferVk::UniformBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device)
        : UniformBuffer(description, data), m_Device(device)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = description.Size;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        if (vmaCreateBuffer(device->GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create uniform buffer");
        }

        void *buffer;
        vmaMapMemory(device->GetAllocator(), m_Buffer.Allocation, &buffer);
        memcpy(buffer, data, description.Size);
        vmaUnmapMemory(device->GetAllocator(), m_Buffer.Allocation);
    }

    void *UniformBufferVk::Map(MapMode mode)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);
        return buffer;
    }

    void UniformBufferVk::Unmap()
    {
        vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer.Allocation);
    }

    VkBuffer UniformBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }
}
