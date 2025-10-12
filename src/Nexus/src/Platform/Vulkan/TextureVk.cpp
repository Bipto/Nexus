#if defined(NX_PLATFORM_VULKAN)

	#include "TextureVk.hpp"

	#include "CommandListVk.hpp"

namespace Nexus::Graphics
{
	TextureVk::TextureVk(const TextureDescription &spec, GraphicsDeviceVk *device) : Texture(spec), m_GraphicsDevice(device)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		NX_VALIDATE(spec.DepthOrArrayLayers >= 1, "Texture must have at least one array layer");
		NX_VALIDATE(spec.MipLevels >= 1, "Texture must have at least one mip level");

		if (spec.Samples > 1)
		{
			NX_VALIDATE(spec.MipLevels == 1, "Multisampled textures do not support mipmapping");
		}

		if (spec.Type == TextureType::TextureCube)
		{
			NX_VALIDATE(spec.DepthOrArrayLayers % 6 == 0, "Cubemap textures must have a multiple of 6 faces");
		}

		uint32_t	 sizeInBytes = GetPixelFormatSizeInBytes(spec.Format);
		VkDeviceSize imageSize	 = spec.Width * spec.Height * sizeInBytes;
		VkImageType	 imageType	 = Vk::GetVkImageType(m_Description.Type);

		VkImageCreateFlagBits imageCreateFlags = Vk::GetVkImageCreateFlagBits(spec);

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

		VkSampleCountFlagBits samples = Vk::GetVkSampleCountFlagsFromSampleCount(spec.Samples);
		VkImageUsageFlagBits  usage	  = Vk::GetVkImageUsageFlags(spec.Format, spec.Usage);

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.pNext				= nullptr;
		imageInfo.flags				= imageCreateFlags;
		imageInfo.imageType			= imageType;
		imageInfo.format			= Vk::GetVkPixelDataFormat(spec.Format);
		imageInfo.extent			= imageExtent;
		imageInfo.mipLevels			= spec.MipLevels;

		// 3D textures do not support array layers
		if (spec.Type == TextureType::Texture3D)
		{
			imageInfo.arrayLayers = 1;
		}
		else
		{
			imageInfo.arrayLayers = spec.DepthOrArrayLayers;
		}

		imageInfo.samples = samples;
		imageInfo.tiling  = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage	  = usage;

		// we only need to commit memory for this texture if it was not requested to bound sparsely
		bool sparseTexture = m_Description.CreateFlags & TextureCreateFlags_SparseBinding;
		if (!sparseTexture)
		{
			VmaAllocationCreateInfo allocInfo = {.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE};

			NX_VALIDATE(vmaCreateImage(device->GetAllocator(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr) == VK_SUCCESS,
						"Failed to create image");
		}

		m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)m_Image, m_Description.DebugName.c_str());
		m_TextureLayouts.resize(m_Description.DepthOrArrayLayers * m_Description.MipLevels, TextureLayout::Undefined);
	}

	TextureVk::TextureVk(VkImage image, const TextureDescription &spec, GraphicsDeviceVk *device, bool owned)
		: Texture(spec),
		  m_GraphicsDevice(device),
		  m_Image(image)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		NX_VALIDATE(spec.DepthOrArrayLayers >= 1, "Texture must have at least one array layer");
		NX_VALIDATE(spec.MipLevels >= 1, "Texture must have at least one mip level");

		if (spec.Samples > 1)
		{
			NX_VALIDATE(spec.MipLevels == 1, "Multisampled textures do not support mipmapping");
		}

		if (spec.Type == TextureType::TextureCube)
		{
			NX_VALIDATE(spec.DepthOrArrayLayers % 6 == 0, "Cubemap textures must have a multiple of 6 faces");
		}

		m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)m_Image, m_Description.DebugName.c_str());
		m_TextureLayouts.resize(m_Description.DepthOrArrayLayers * m_Description.MipLevels, TextureLayout::Undefined);
	}

	TextureVk::~TextureVk()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		for (const auto &[viewInfo, view] : m_ImageViews) { context.DestroyImageView(m_GraphicsDevice->GetVkDevice(), view, nullptr); }

		if (m_Owned)
		{
			vmaDestroyImage(m_GraphicsDevice->GetAllocator(), m_Image, m_Allocation);
		}

		m_Image		 = VK_NULL_HANDLE;
		m_Allocation = VK_NULL_HANDLE;
	}

	VkImage TextureVk::GetImage()
	{
		return m_Image;
	}

	VkImageView TextureVk::GetImageView(const VulkanTextureViewInfo &desc)
	{
		if (m_ImageViews.find(desc) != m_ImageViews.end())
		{
			return m_ImageViews.at(desc);
		}
		else
		{
			VkImageViewType viewType  = Vk::GetVkImageViewType(m_Description);
			VkImageView		imageView = VK_NULL_HANDLE;

			VkImageViewCreateInfo createInfo		   = {};
			createInfo.sType						   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext						   = nullptr;
			createInfo.viewType						   = viewType;
			createInfo.image						   = m_Image;
			createInfo.format						   = Vk::GetVkPixelDataFormat(m_Description.Format);
			createInfo.subresourceRange.baseMipLevel   = desc.BaseMipLevel;
			createInfo.subresourceRange.levelCount	   = desc.LevelCount;
			createInfo.subresourceRange.baseArrayLayer = desc.BaseArrayLayer;
			createInfo.subresourceRange.layerCount	   = desc.LayerCount;

			PixelFormatType pixelFormatType = GetPixelFormatType(m_Description.Format);
			bool			isDepth			= pixelFormatType == PixelFormatType::DepthStencil;

			if (isDepth)
			{
				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			else
			{
				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}

			const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

			NX_VALIDATE(context.CreateImageView(m_GraphicsDevice->GetVkDevice(), &createInfo, nullptr, &imageView) == VK_SUCCESS,
						"Failed to create image view");

			m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)imageView, m_Description.DebugName.c_str());

			m_ImageViews[desc] = imageView;
			return imageView;
		}
	}

	TextureLayout TextureVk::GetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel) const
	{
		NX_VALIDATE(arrayLayer <= m_Description.DepthOrArrayLayers, "Array layer is greater than the total number of array layers");
		NX_VALIDATE(mipLevel <= m_Description.MipLevels, "Mip level is greater than the total number of mip levels");

		return m_TextureLayouts[arrayLayer * m_Description.MipLevels + mipLevel];
	}

	void TextureVk::SetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel, TextureLayout layout)
	{
		NX_VALIDATE(arrayLayer <= m_Description.DepthOrArrayLayers, "Array layer is greater than the total number of array layers");
		NX_VALIDATE(mipLevel <= m_Description.MipLevels, "Mip level is greater than the total number of mip levels");

		m_TextureLayouts[arrayLayer * m_Description.MipLevels + mipLevel] = layout;
	}
}	 // namespace Nexus::Graphics

#endif