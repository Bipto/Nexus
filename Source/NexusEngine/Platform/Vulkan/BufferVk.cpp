#if defined(NX_PLATFORM_VULKAN)

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

    void VertexBufferVk::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);
        unsigned char *char_buffer = reinterpret_cast<unsigned char *>(buffer);
        char_buffer += offset;
        memcpy(char_buffer, data, size);
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

    void IndexBufferVk::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);
        unsigned char *char_buffer = reinterpret_cast<unsigned char *>(buffer);
        char_buffer += offset;
        memcpy(char_buffer, data, size);
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

    void UniformBufferVk::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);
        unsigned char *char_buffer = reinterpret_cast<unsigned char *>(buffer);
        char_buffer += offset;
        memcpy(char_buffer, data, size);
        vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer.Allocation);
    }

    VkBuffer UniformBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }
}

#endif