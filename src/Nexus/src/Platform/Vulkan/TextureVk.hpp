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
		TextureVk(const TextureDescription &spec, GraphicsDeviceVk *device);
		virtual ~TextureVk();

		VkImage		GetImage();
		VkImageView GetImageView();

		VkImageLayout GetImageLayout(uint32_t arrayLayer, uint32_t mipLevel);
		void		  SetImageLayout(uint32_t arrayLayer, uint32_t mipLevel, VkImageLayout layout);

	  private:
		GraphicsDeviceVk		  *m_GraphicsDevice;
		VkImage					   m_Image = VK_NULL_HANDLE;
		VmaAllocation			   m_Allocation;
		VkImageView				   m_ImageView = VK_NULL_HANDLE;
		VkFormat				   m_Format;
		std::vector<VkImageLayout> m_Layouts;
	};
}	 // namespace Nexus::Graphics

#endif