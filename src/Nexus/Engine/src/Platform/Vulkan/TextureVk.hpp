#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "RHI/Texture.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	struct VulkanTextureViewInfo
	{
		uint32_t BaseMipLevel	= 0;
		uint32_t LevelCount		= 0;
		uint32_t BaseArrayLayer = 0;
		uint32_t LayerCount		= 0;

		bool operator<(const VulkanTextureViewInfo &other) const
		{
			return std::tie(BaseMipLevel, LevelCount, BaseArrayLayer, LayerCount) <
				   std::tie(other.BaseMipLevel, other.LevelCount, other.BaseArrayLayer, other.LayerCount);
		}
	};

	class TextureVk : public ITexture
	{
	  public:
		TextureVk(const TextureDescription &spec, GraphicsDeviceVk *device);
		TextureVk(VkImage image, const TextureDescription &spec, GraphicsDeviceVk *device, bool owned = false);
		virtual ~TextureVk();

		const VkImage	  GetImage() const;
		const VkImageView GetImageView(const VulkanTextureViewInfo &desc) const;

		TextureLayout GetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel) const final;
		void		  SetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel, TextureLayout layout);

		SubresourceFootprint GetSubresourceFootprint(uint32_t arrayLayer, uint32_t mipLevel) const final;

	  private:
		GraphicsDeviceVk *m_GraphicsDevice = nullptr;
		VkImage			  m_Image		   = VK_NULL_HANDLE;
		VmaAllocation	  m_Allocation	   = VK_NULL_HANDLE;

		bool m_Owned = true;

		std::vector<TextureLayout>							 m_TextureLayouts = {};
		mutable std::map<VulkanTextureViewInfo, VkImageView> m_ImageViews	  = {};
	};
}	 // namespace Nexus::Graphics

#endif