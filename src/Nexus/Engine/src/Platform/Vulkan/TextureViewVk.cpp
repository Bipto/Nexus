#include "TextureViewVk.hpp"
#include "TextureVk.hpp"

namespace Nexus::Graphics
{
    TextureViewVk::TextureViewVk(
        const TextureViewDescription &desc, GraphicsDeviceVk *device
    )
        : m_Description(desc), m_Device(device)
    {
        const GladVulkanContext &context = device->GetVulkanContext();

        const TextureVk *const texture =
            desc.TargetTexture.AsDerived<const TextureVk>();
        VkImageViewType type = Vk::GetImageViewType(desc);

        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = texture->GetImage();
        createInfo.viewType = type;
        createInfo.format = Vk::GetVkPixelDataFormat(desc.Format);
        createInfo.subresourceRange.aspectMask =
            texture->IsDepth()
                ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
                : VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = desc.Range.BaseArrayLayer;
        createInfo.subresourceRange.layerCount = desc.Range.LayerCount;
        createInfo.subresourceRange.baseMipLevel = desc.Range.BaseMipLevel;
        createInfo.subresourceRange.levelCount = desc.Range.LevelCount;

        NX_VALIDATE(
            context.CreateImageView(
                device->GetVkDevice(), &createInfo, nullptr, &m_ImageView
            ) == VK_SUCCESS,
            "Failed to create image view"
        );

        device->SetObjectName(
            VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)m_ImageView, desc.DebugName.c_str()
        );
    }

    TextureViewVk::~TextureViewVk()
    {
        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.DestroyImageView(m_Device->GetVkDevice(), m_ImageView, nullptr);
    }

    const TextureViewDescription &TextureViewVk::GetDescription() const
    {
        return m_Description;
    }

    const VkImageView TextureViewVk::GetVkImageView() const
    {
        return m_ImageView;
    }
} // namespace Nexus::Graphics