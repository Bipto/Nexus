#if defined(NX_PLATFORM_VULKAN)

#include "BufferVk.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    VertexBufferVk::VertexBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device)
        : VertexBuffer(description, data), m_Device(device)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = description.Size;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        vmaAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        // vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        if (vmaCreateBuffer(device->GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer");
        }

        if (!data)
            return;

        SetData(data, m_Description.Size, 0);
    }

    VertexBufferVk::~VertexBufferVk()
    {
        vmaDestroyBuffer(m_Device->GetAllocator(), m_Buffer.Buffer, m_Buffer.Allocation);
    }

    VkBuffer VertexBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }

    void VertexBufferVk::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);

        void *offsetIntoBuffer = (void *)((const char *)buffer + offset);
        memcpy(offsetIntoBuffer, data, size);

        vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer.Allocation);
    }

    IndexBufferVk::IndexBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device, IndexBufferFormat format)
        : IndexBuffer(description, data, format), m_Device(device)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = description.Size;
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        vmaAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        // vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        if (vmaCreateBuffer(device->GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create index buffer");
        }

        if (!data)
            return;

        SetData(data, m_Description.Size, 0);
    }

    IndexBufferVk::~IndexBufferVk()
    {
        vmaDestroyBuffer(m_Device->GetAllocator(), m_Buffer.Buffer, m_Buffer.Allocation);
    }

    VkBuffer IndexBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }

    void IndexBufferVk::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);

        void *offsetIntoBuffer = (void *)((const char *)buffer + offset);
        memcpy(offsetIntoBuffer, data, size);

        vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer.Allocation);
    }

    UniformBufferVk::UniformBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device)
        : UniformBuffer(description, data), m_Device(device)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = description.Size;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        vmaAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

        m_Buffers.resize(FRAMES_IN_FLIGHT);

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            if (vmaCreateBuffer(device->GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffers[i].Buffer, &m_Buffers[i].Allocation, nullptr) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create uniform buffer");
            }
        }

        if (!data)
            return;

        SetData(data, m_Description.Size, 0);
    }

    UniformBufferVk::~UniformBufferVk()
    {
        for (int i = 0; i < m_Buffers.size(); i++)
        {
            vmaDestroyBuffer(m_Device->GetAllocator(), m_Buffers[i].Buffer, m_Buffers[i].Allocation);
        }
    }

    VkBuffer UniformBufferVk::GetBuffer()
    {
        return m_Buffers[m_Device->GetCurrentFrameIndex()].Buffer;
    }

    void UniformBufferVk::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        void *buffer;
        vmaMapMemory(m_Device->GetAllocator(), m_Buffers[m_Device->GetCurrentFrameIndex()].Allocation, &buffer);

        void *offsetIntoBuffer = (void *)((const char *)buffer + offset);
        memcpy(offsetIntoBuffer, data, size);

        vmaUnmapMemory(m_Device->GetAllocator(), m_Buffers[m_Device->GetCurrentFrameIndex()].Allocation);
    }
}

#endif