#include "TexelBufferVk.hpp"
#include "DeviceBufferVk.hpp"

namespace Nexus::Graphics
{
    TexelBufferVk::TexelBufferVk(
        const TexelBufferDescription &desc, GraphicsDeviceVk *device
    )
        : m_Device(device), m_Description(desc)
    {
        const GladVulkanContext &context = device->GetVulkanContext();

        const DeviceBufferVk *buffer = desc.Buffer.AsDerived<const DeviceBufferVk>();

        VkBufferViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        viewInfo.pNext = nullptr;
        viewInfo.buffer = buffer->GetVkBuffer();
        viewInfo.format = Vk::GetVkPixelDataFormat(desc.Format);
        viewInfo.flags = 0;
        viewInfo.offset = desc.Offset;
        viewInfo.range = desc.SizeInBytes;

        context.CreateBufferView(
            device->GetVkDevice(), &viewInfo, nullptr, &m_BufferView
        );
    }

    TexelBufferVk::~TexelBufferVk()
    {
        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.DestroyBufferView(m_Device->GetVkDevice(), m_BufferView, nullptr);
    }

    const TexelBufferDescription &TexelBufferVk::GetDescription() const
    {
        return m_Description;
    }

    VkBufferView TexelBufferVk::GetVkBufferView() const
    {
        return m_BufferView;
    }

} // namespace Nexus::Graphics
