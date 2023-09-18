#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "GraphicsDeviceVk.hpp"
#include "Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureVk : public Texture
    {
    public:
        TextureVk(GraphicsDeviceVk *graphicsDevice, const TextureSpecification &spec);
        ~TextureVk();
        virtual void *GetHandle() override;
        VkImage GetImage();
        VkImageView GetImageView();
        VkSampler GetSampler();

    private:
        GraphicsDeviceVk *m_GraphicsDevice;
        VkImage m_Image;
        VmaAllocation m_Allocation;
        VkImageView m_ImageView;
        VkSampler m_Sampler;
    };
}

#endif