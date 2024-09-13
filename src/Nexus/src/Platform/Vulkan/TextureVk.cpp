#if defined(NX_PLATFORM_VULKAN)

	#include "TextureVk.hpp"

	#include "CommandListVk.hpp"

namespace Nexus::Graphics
{
	Texture2D_Vk::Texture2D_Vk(GraphicsDeviceVk *graphicsDevice, const Texture2DSpecification &spec)
		: Texture2D(spec, graphicsDevice),
		  m_GraphicsDevice(graphicsDevice)
	{
		uint32_t sizeInBits = GetPixelFormatSizeInBytes(m_Specification.Format);

		bool				 isDepth;
		VkDeviceSize		 imageSize = spec.Width * spec.Height * sizeInBits;
		VkImageUsageFlagBits usage	   = Vk::GetVkImageUsageFlags(spec.Usage, isDepth);
		m_Format					   = Vk::GetVkPixelDataFormat(spec.Format, isDepth);

		m_StagingBuffer =
		graphicsDevice->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		VkExtent3D imageExtent;
		imageExtent.width  = spec.Width;
		imageExtent.height = spec.Height;
		imageExtent.depth  = 1;

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.pNext				= nullptr;
		imageInfo.imageType			= VK_IMAGE_TYPE_2D;
		imageInfo.format			= m_Format;
		imageInfo.extent			= imageExtent;
		imageInfo.mipLevels			= m_Specification.MipLevels;
		imageInfo.arrayLayers		= 1;
		imageInfo.samples			= VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling			= VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage				= usage;

		VmaAllocationCreateInfo imageAllocInfo = {};
		imageAllocInfo.usage				   = VMA_MEMORY_USAGE_AUTO;

		if (vmaCreateImage(graphicsDevice->GetAllocator(), &imageInfo, &imageAllocInfo, &m_Image, &m_Allocation, nullptr) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image");
		}

		VkImageViewCreateInfo createInfo {};
		createInfo.sType						   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext						   = nullptr;
		createInfo.viewType						   = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.image						   = m_Image;
		createInfo.format						   = m_Format;
		createInfo.subresourceRange.baseMipLevel   = 0;
		createInfo.subresourceRange.levelCount	   = m_Specification.MipLevels;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount	   = 1;

		if (isDepth)
		{
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
		{
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (vkCreateImageView(graphicsDevice->GetVkDevice(), &createInfo, nullptr, &m_ImageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image view");
		}

		VkImageAspectFlagBits aspectFlags = {};
		if (isDepth)
		{
			aspectFlags = VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		}
		else
		{
			aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		for (size_t i = 0; i < spec.MipLevels; i++) { m_Layouts.push_back(VK_IMAGE_LAYOUT_UNDEFINED); }
	}

	Texture2D_Vk::~Texture2D_Vk()
	{
		vkDestroyImageView(m_GraphicsDevice->GetVkDevice(), m_ImageView, nullptr);
		vmaDestroyImage(m_GraphicsDevice->GetAllocator(), m_Image, m_Allocation);
	}

	void Texture2D_Vk::SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		uint32_t	 numChannels = GetPixelFormatNumberOfChannels(m_Specification.Format);
		VkDeviceSize imageSize	 = width * height * GetPixelFormatSizeInBytes(m_Specification.Format);

		void *buffer;
		vmaMapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation, &buffer);
		memcpy(buffer, data, imageSize);
		vmaUnmapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation);

		VkExtent3D imageExtent;
		imageExtent.width  = width;
		imageExtent.height = height;
		imageExtent.depth  = 1;

		// upload texture to GPU
		{
			VkImageLayout before = m_Layouts.at(level);

			m_GraphicsDevice->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				m_GraphicsDevice
				->TransitionVulkanImageLayout(cmd, m_Image, level, 0, before, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

				VkBufferImageCopy copyRegion			   = {};
				copyRegion.bufferOffset					   = 0;
				copyRegion.bufferRowLength				   = 0;
				copyRegion.bufferImageHeight			   = 0;
				copyRegion.imageSubresource.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel	   = level;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount	   = 1;
				copyRegion.imageExtent					   = imageExtent;
				vkCmdCopyBufferToImage(cmd, m_StagingBuffer.Buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

				SetImageLayout(level, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			});
		}
	}

	std::vector<std::byte> Texture2D_Vk::GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		uint32_t	 offset = x * y * GetPixelFormatSizeInBits(m_Specification.Format);
		VkDeviceSize size	= width * height * GetPixelFormatSizeInBytes(m_Specification.Format);

		VkExtent3D extent = {};
		extent.width	  = width;
		extent.height	  = height;
		extent.depth	  = 1;

		// retrieve pixels from texture
		{
			VkImageLayout before = m_Layouts.at(level);

			m_GraphicsDevice->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				m_GraphicsDevice
				->TransitionVulkanImageLayout(cmd, m_Image, level, 0, before, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

				VkBufferImageCopy copyRegion			   = {};
				copyRegion.bufferOffset					   = 0;
				copyRegion.bufferRowLength				   = 0;
				copyRegion.bufferImageHeight			   = 0;
				copyRegion.imageSubresource.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel	   = level;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount	   = 1;
				copyRegion.imageExtent					   = extent;
				copyRegion.imageOffset.x				   = x;
				copyRegion.imageOffset.y				   = y;
				copyRegion.imageOffset.z				   = 0;
				vkCmdCopyImageToBuffer(cmd, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_StagingBuffer.Buffer, 1, &copyRegion);

				SetImageLayout(level, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			});
		}

		std::vector<std::byte> pixels(size);

		void *buffer;
		vmaMapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation, &buffer);
		memcpy(pixels.data(), buffer, pixels.size());
		vmaUnmapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation);

		return pixels;
	}

	VkImage Texture2D_Vk::GetImage()
	{
		return m_Image;
	}

	VkImageView Texture2D_Vk::GetImageView()
	{
		return m_ImageView;
	}

	VkImageLayout Texture2D_Vk::GetImageLayout(uint32_t level)
	{
		return m_Layouts.at(level);
	}

	void Texture2D_Vk::SetImageLayout(uint32_t level, VkImageLayout layout)
	{
		m_Layouts[level] = layout;
	}

	Cubemap_Vk::Cubemap_Vk(GraphicsDeviceVk *graphicsDevice, const CubemapSpecification &spec)
		: Cubemap(spec, graphicsDevice),
		  m_GraphicsDevice(graphicsDevice)
	{
		uint32_t	 sizeInBits = GetPixelFormatSizeInBits(m_Specification.Format);
		VkDeviceSize imageSize	= m_Specification.Width * m_Specification.Height * sizeInBits;
		m_Format				= Vk::GetVkPixelDataFormat(m_Specification.Format, false);

		m_StagingBuffer =
		graphicsDevice->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		VkExtent3D imageExtent {.width = spec.Width, .height = spec.Height, .depth = 1};

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.pNext				= nullptr;
		imageInfo.flags				= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		imageInfo.imageType			= VK_IMAGE_TYPE_2D;
		imageInfo.format			= m_Format;
		imageInfo.extent			= imageExtent;
		imageInfo.mipLevels			= m_Specification.MipLevels;
		imageInfo.arrayLayers		= 6;
		imageInfo.samples			= VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling			= VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage				= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo imageAllocInfo = {.usage = VMA_MEMORY_USAGE_AUTO};

		if (vmaCreateImage(graphicsDevice->GetAllocator(), &imageInfo, &imageAllocInfo, &m_Image, &m_Allocation, nullptr) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image");
		}

		VkImageViewCreateInfo createInfo		   = {};
		createInfo.sType						   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext						   = nullptr;
		createInfo.viewType						   = VK_IMAGE_VIEW_TYPE_CUBE;
		createInfo.image						   = m_Image;
		createInfo.format						   = m_Format;
		createInfo.subresourceRange.baseMipLevel   = 0;
		createInfo.subresourceRange.levelCount	   = m_Specification.MipLevels;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount	   = 6;
		createInfo.subresourceRange.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;

		if (vkCreateImageView(graphicsDevice->GetVkDevice(), &createInfo, nullptr, &m_ImageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image view");
		}

		for (uint32_t arrayLayer = 0; arrayLayer < 6; arrayLayer++)
		{
			std::vector<VkImageLayout> layerLayouts;
			for (uint32_t mipLevel = 0; mipLevel < m_Specification.MipLevels; mipLevel++) { layerLayouts.push_back(VK_IMAGE_LAYOUT_UNDEFINED); }
			m_Layouts.push_back(layerLayouts);
		}
	}

	Cubemap_Vk::~Cubemap_Vk()
	{
		vkDestroyImageView(m_GraphicsDevice->GetVkDevice(), m_ImageView, nullptr);
		vmaDestroyImage(m_GraphicsDevice->GetAllocator(), m_Image, m_Allocation);
	}

	void Cubemap_Vk::SetData(const void *data, CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		uint32_t	 faceIndex	 = (uint32_t)face;
		uint32_t	 numChannels = GetPixelFormatNumberOfChannels(m_Specification.Format);
		VkDeviceSize imageSize	 = width * height * GetPixelFormatSizeInBytes(m_Specification.Format);

		void *buffer;
		vmaMapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation, &buffer);
		memcpy(buffer, data, imageSize);
		vmaUnmapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation);

		VkExtent3D imageExtent;
		imageExtent.width  = width;
		imageExtent.height = height;
		imageExtent.depth  = 1;

		// upload from staging buffer to GPU
		{
			VkImageLayout before = m_Layouts.at(faceIndex).at(level);

			m_GraphicsDevice->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				m_GraphicsDevice->TransitionVulkanImageLayout(cmd,
															  m_Image,
															  level,
															  faceIndex,
															  before,
															  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
															  VK_IMAGE_ASPECT_COLOR_BIT);

				VkBufferImageCopy copyRegion			   = {};
				copyRegion.bufferOffset					   = 0;
				copyRegion.bufferRowLength				   = 0;
				copyRegion.bufferImageHeight			   = 0;
				copyRegion.imageSubresource.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel	   = level;
				copyRegion.imageSubresource.baseArrayLayer = faceIndex;
				copyRegion.imageSubresource.layerCount	   = 1;
				copyRegion.imageExtent					   = imageExtent;
				vkCmdCopyBufferToImage(cmd, m_StagingBuffer.Buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

				SetImageLayout(faceIndex, level, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			});
		}
	}

	std::vector<std::byte> Cubemap_Vk::GetData(CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		uint32_t	 faceIndex = (uint32_t)face;
		uint32_t	 offset	   = x * y * GetPixelFormatSizeInBytes(m_Specification.Format);
		VkDeviceSize size	   = width * height * GetPixelFormatSizeInBytes(m_Specification.Format);

		VkExtent3D extent = {};
		extent.width	  = width;
		extent.height	  = height;
		extent.depth	  = 1;

		// retrieve pixels from texture
		{
			VkImageLayout before = m_Layouts.at(faceIndex).at(level);

			m_GraphicsDevice->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				m_GraphicsDevice->TransitionVulkanImageLayout(cmd,
															  m_Image,
															  level,
															  faceIndex,
															  before,
															  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
															  VK_IMAGE_ASPECT_COLOR_BIT);

				VkBufferImageCopy copyRegion			   = {};
				copyRegion.bufferOffset					   = 0;
				copyRegion.bufferRowLength				   = 0;
				copyRegion.bufferImageHeight			   = 0;
				copyRegion.imageSubresource.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel	   = level;
				copyRegion.imageSubresource.baseArrayLayer = faceIndex;
				copyRegion.imageSubresource.layerCount	   = 1;
				copyRegion.imageExtent					   = extent;
				copyRegion.imageOffset.x				   = x;
				copyRegion.imageOffset.y				   = y;
				copyRegion.imageOffset.z				   = 0;
				vkCmdCopyImageToBuffer(cmd, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_StagingBuffer.Buffer, 1, &copyRegion);
			});

			std::vector<std::byte> pixels(size);

			void *buffer;
			vmaMapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation, &buffer);
			memcpy(pixels.data(), buffer, pixels.size());
			vmaUnmapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation);

			return pixels;
		}

		return std::vector<std::byte>();
	}

	VkImage Cubemap_Vk::GetImage()
	{
		return m_Image;
	}

	VkImageView Cubemap_Vk::GetImageView()
	{
		return m_ImageView;
	}

	VkImageLayout Cubemap_Vk::GetImageLayout(uint32_t arrayLayer, uint32_t mipLevel)
	{
		return m_Layouts.at(arrayLayer).at(mipLevel);
	}

	void Cubemap_Vk::SetImageLayout(uint32_t arrayLayer, uint32_t mipLevel, VkImageLayout layout)
	{
		m_Layouts[arrayLayer][mipLevel] = layout;
	}
}	 // namespace Nexus::Graphics

#endif