#if defined(NX_PLATFORM_VULKAN)

	#include "TextureVk.hpp"

	#include "CommandListVk.hpp"

namespace Nexus::Graphics
{
	TextureVk::TextureVk(const TextureSpecification &spec, GraphicsDeviceVk *device) : Texture(spec), m_GraphicsDevice(device)
	{
		NX_ASSERT(spec.ArrayLayers >= 1, "Texture must have at least one array layer");
		NX_ASSERT(spec.MipLevels >= 1, "Texture must have at least one mip level");

		if (spec.Usage & TextureUsage_Cubemap)
		{
			NX_ASSERT(spec.ArrayLayers % 6 == 0, "Cubemap texture must have 6 array layers");
		}

		if (spec.Samples > 1)
		{
			NX_ASSERT(spec.MipLevels == 0, "Multisampled textures do not support mipmapping");
		}

		uint32_t	 sizeInBytes = GetPixelFormatSizeInBytes(spec.Format);
		bool		 isDepth	 = spec.Usage & TextureUsage_DepthStencil;
		VkDeviceSize imageSize	 = spec.Width * spec.Height * sizeInBytes;
		m_Format				 = Vk::GetVkPixelDataFormat(m_Specification.Format, isDepth);
		VkImageType imageType	 = Vk::GetVkImageType(m_Specification.Type);

		VkImageCreateFlagBits imageCreateFlags = Vk::GetVkImageCreateFlagBits(spec.Usage);
		VkExtent3D			  imageExtent	   = {.width = spec.Width, .height = spec.Height, .depth = spec.Depth};
		VkSampleCountFlagBits samples		   = Vk::GetVkSampleCountFlagsFromSampleCount(spec.Samples);
		VkImageUsageFlagBits  usage			   = Vk::GetVkImageUsageFlags(spec.Usage);

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.pNext				= nullptr;
		imageInfo.flags				= imageCreateFlags;
		imageInfo.imageType			= imageType;
		imageInfo.format			= m_Format;
		imageInfo.extent			= imageExtent;
		imageInfo.mipLevels			= spec.MipLevels;
		imageInfo.arrayLayers		= spec.ArrayLayers;
		imageInfo.samples			= samples;
		imageInfo.tiling			= VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage				= usage;

		VmaAllocationCreateInfo allocInfo = {.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE};

		NX_ASSERT(vmaCreateImage(device->GetAllocator(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr) == VK_SUCCESS,
				  "Failed to create image");

		VkImageViewType viewType = Vk::GetVkImageViewType(spec);

		VkImageViewCreateInfo createInfo		   = {};
		createInfo.sType						   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext						   = nullptr;
		createInfo.viewType						   = viewType;
		createInfo.image						   = m_Image;
		createInfo.format						   = m_Format;
		createInfo.subresourceRange.baseMipLevel   = 0;
		createInfo.subresourceRange.levelCount	   = spec.MipLevels;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount	   = spec.ArrayLayers;

		if (isDepth)
		{
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
		{
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		NX_ASSERT(vkCreateImageView(device->GetVkDevice(), &createInfo, nullptr, &m_ImageView) == VK_SUCCESS, "Failed to create image view");

		// create resource states
		{
			for (uint32_t arrayLayer = 0; arrayLayer < spec.ArrayLayers; arrayLayer++)
			{
				for (uint32_t mipLevel = 0; mipLevel < spec.MipLevels; mipLevel++) { m_Layouts.push_back(VK_IMAGE_LAYOUT_GENERAL); }
			}	 // namespace Nexus::Graphics
		}

		m_GraphicsDevice->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				for (uint32_t layer = 0; layer < m_Specification.ArrayLayers; layer++)
				{
					for (uint32_t mip = 0; mip < m_Specification.MipLevels; mip++)
					{
						VkImageAspectFlagBits aspectFlags = VK_IMAGE_ASPECT_NONE;
						if (isDepth)
						{
							aspectFlags = VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
						}
						else
						{
							aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
						}

						m_GraphicsDevice
							->TransitionVulkanImageLayout(cmd, m_Image, mip, layer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, aspectFlags);
					}
				}
			});

		m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)m_Image, m_Specification.DebugName.c_str());
	}

	TextureVk::~TextureVk()
	{
		vkDestroyImageView(m_GraphicsDevice->GetVkDevice(), m_ImageView, nullptr);
		vmaDestroyImage(m_GraphicsDevice->GetAllocator(), m_Image, m_Allocation);
	}

	VkImage TextureVk::GetImage()
	{
		return m_Image;
	}

	VkImageView TextureVk::GetImageView()
	{
		return m_ImageView;
	}

	VkImageLayout TextureVk::GetImageLayout(uint32_t arrayLayer, uint32_t mipLevel)
	{
		NX_ASSERT(arrayLayer <= m_Specification.ArrayLayers, "Array layer is greater than the total number of array layers");
		NX_ASSERT(mipLevel <= m_Specification.MipLevels, "Mip level is greater than the total number of mip levels");

		return m_Layouts[arrayLayer * m_Specification.MipLevels + mipLevel];
	}

	void TextureVk::SetImageLayout(uint32_t arrayLayer, uint32_t mipLevel, VkImageLayout layout)
	{
		NX_ASSERT(arrayLayer <= m_Specification.ArrayLayers, "Array layer is greater than the total number of array layers");
		NX_ASSERT(mipLevel <= m_Specification.MipLevels, "Mip level is greater than the total number of mip levels");
		m_Layouts[arrayLayer * m_Specification.MipLevels + mipLevel] = layout;
	}
}	 // namespace Nexus::Graphics

#endif