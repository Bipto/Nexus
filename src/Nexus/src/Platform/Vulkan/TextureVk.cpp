#if defined(NX_PLATFORM_VULKAN)

	#include "TextureVk.hpp"

	#include "CommandListVk.hpp"

namespace Nexus::Graphics
{
	TextureVk::TextureVk(const TextureDescription &spec, GraphicsDeviceVk *device) : Texture(spec), m_GraphicsDevice(device)
	{
		NX_VALIDATE(spec.DepthOrArrayLayers >= 1, "Texture must have at least one array layer");
		NX_VALIDATE(spec.MipLevels >= 1, "Texture must have at least one mip level");

		if (spec.Samples > 1)
		{
			NX_VALIDATE(spec.MipLevels == 0, "Multisampled textures do not support mipmapping");
		}

		uint32_t	 sizeInBytes = GetPixelFormatSizeInBytes(spec.Format);
		VkDeviceSize imageSize	 = spec.Width * spec.Height * sizeInBytes;
		m_Format				 = Vk::GetVkPixelDataFormat(m_Description.Format);
		VkImageType imageType	 = Vk::GetVkImageType(m_Description.Type);

		VkImageCreateFlagBits imageCreateFlags = Vk::GetVkImageCreateFlagBits(spec.Type, spec.Usage);

		VkExtent3D imageExtent = {};

		// only 3D textures can have a depth
		if (spec.Type == TextureType::Texture3D)
		{
			imageExtent = {spec.Width, spec.Height, spec.DepthOrArrayLayers};
		}
		else
		{
			imageExtent = {spec.Width, spec.Height, 1};
		}

		VkSampleCountFlagBits samples		   = Vk::GetVkSampleCountFlagsFromSampleCount(spec.Samples);
		VkImageUsageFlagBits  usage			   = Vk::GetVkImageUsageFlags(spec.Format, spec.Usage);

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.pNext				= nullptr;
		imageInfo.flags				= imageCreateFlags;
		imageInfo.imageType			= imageType;
		imageInfo.format			= m_Format;
		imageInfo.extent			= imageExtent;
		imageInfo.mipLevels			= spec.MipLevels;

		// 3D textures do not support array layers
		if (spec.Type == TextureType::Texture3D)
		{
			imageInfo.arrayLayers = 1;
		}
		else if (spec.Type == TextureType::TextureCube)
		{
			imageInfo.arrayLayers = spec.DepthOrArrayLayers * 6;
		}
		else
		{
			imageInfo.arrayLayers = spec.DepthOrArrayLayers;
		}

		imageInfo.samples			= samples;
		imageInfo.tiling			= VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage				= usage;

		VmaAllocationCreateInfo allocInfo = {.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE};

		NX_VALIDATE(vmaCreateImage(device->GetAllocator(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr) == VK_SUCCESS,
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
		createInfo.subresourceRange.layerCount	   = spec.DepthOrArrayLayers;

		PixelFormatType pixelFormatType = GetPixelFormatType(spec.Format);
		bool			isDepth			= pixelFormatType == PixelFormatType::DepthStencil;

		if (isDepth)
		{
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
		{
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		NX_VALIDATE(vkCreateImageView(device->GetVkDevice(), &createInfo, nullptr, &m_ImageView) == VK_SUCCESS, "Failed to create image view");

		// create resource states
		{
			for (uint32_t arrayLayer = 0; arrayLayer < spec.DepthOrArrayLayers; arrayLayer++)
			{
				for (uint32_t mipLevel = 0; mipLevel < spec.MipLevels; mipLevel++) { m_Layouts.push_back(VK_IMAGE_LAYOUT_GENERAL); }
			}	 // namespace Nexus::Graphics
		}

		m_GraphicsDevice->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				for (uint32_t layer = 0; layer < m_Description.DepthOrArrayLayers; layer++)
				{
					for (uint32_t mip = 0; mip < m_Description.MipLevels; mip++)
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

		m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)m_Image, m_Description.DebugName.c_str());
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
		NX_VALIDATE(arrayLayer <= m_Description.DepthOrArrayLayers, "Array layer is greater than the total number of array layers");
		NX_VALIDATE(mipLevel <= m_Description.MipLevels, "Mip level is greater than the total number of mip levels");

		return m_Layouts[arrayLayer * m_Description.MipLevels + mipLevel];
	}

	void TextureVk::SetImageLayout(uint32_t arrayLayer, uint32_t mipLevel, VkImageLayout layout)
	{
		NX_VALIDATE(arrayLayer <= m_Description.DepthOrArrayLayers, "Array layer is greater than the total number of array layers");
		NX_VALIDATE(mipLevel <= m_Description.MipLevels, "Mip level is greater than the total number of mip levels");
		m_Layouts[arrayLayer * m_Description.MipLevels + mipLevel] = layout;
	}
}	 // namespace Nexus::Graphics

#endif