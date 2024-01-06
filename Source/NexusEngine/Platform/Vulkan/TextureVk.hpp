#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "GraphicsDeviceVk.hpp"
#include "Nexus/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureVk : public Texture
    {
    public:
        TextureVk(GraphicsDeviceVk *graphicsDevice, const TextureSpecification &spec);
        ~TextureVk();
        virtual ResourceHandle GetHandle() override;
        virtual void SetData(const void *data, uint32_t size) override;
        VkImage GetImage();
        VkImageView GetImageView();
        VkSampler GetSampler();

    private:
        GraphicsDeviceVk *m_GraphicsDevice;
        VkImage m_Image;
        VmaAllocation m_Allocation;
        VkImageView m_ImageView;
        VkSampler m_Sampler;
        VkFormat m_Format;
        AllocatedBuffer m_StagingBuffer;
    };
}

#endif