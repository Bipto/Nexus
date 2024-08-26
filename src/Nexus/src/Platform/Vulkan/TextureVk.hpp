#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "GraphicsDeviceVk.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"
#include "Vk.hpp"

namespace Nexus::Graphics
{
class TextureVk : public Texture
{
  public:
    TextureVk(GraphicsDeviceVk *graphicsDevice, const TextureSpecification &spec);
    ~TextureVk();
    virtual void SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    virtual std::vector<std::byte> GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    VkImage GetImage();
    VkImageView GetImageView();

  private:
    GraphicsDeviceVk *m_GraphicsDevice;
    VkImage m_Image;
    VmaAllocation m_Allocation;
    VkImageView m_ImageView;
    VkFormat m_Format;
    Vk::AllocatedBuffer m_StagingBuffer;
};
} // namespace Nexus::Graphics

#endif