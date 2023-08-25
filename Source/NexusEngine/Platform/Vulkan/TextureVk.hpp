#pragma once

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

    private:
        VkBuffer m_Buffer;
        VmaAllocation m_Allocation;
        VkImageView m_ImageView;
    };
}