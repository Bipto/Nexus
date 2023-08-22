#include "BufferVk.hpp"

namespace Nexus::Graphics
{
    VertexBufferVk::VertexBufferVk(const BufferDescription &description, const void *data, const VertexBufferLayout &layout, GraphicsDeviceVk *device)
        : VertexBuffer(description, data, layout)
    {
        /* VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.size = description.Size;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        if (vmaCreateBuffer(device->GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer");
        }

        void *mappedData;
        vmaMapMemory(device->GetAllocator(), m_Buffer.Allocation, &mappedData);
        memcpy(mappedData, &data, bufferInfo.size);
        vmaUnmapMemory(device->GetAllocator(), m_Buffer.Allocation); */

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
    }

    VkBuffer VertexBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }

    IndexBufferVk::IndexBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device)
        : IndexBuffer(description, data)
    {
    }

    void IndexBufferVk::SetData(const void *data, uint32_t size, uint32_t offset)
    {
    }

    VkBuffer IndexBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }

    UniformBufferVk::UniformBufferVk(const BufferDescription &description, const void *data, GraphicsDeviceVk *device)
        : UniformBuffer(description, data)
    {
    }

    void UniformBufferVk::SetData(const void *data, uint32_t size, uint32_t offset)
    {
    }

    VkBuffer UniformBufferVk::GetBuffer()
    {
        return m_Buffer.Buffer;
    }
}
