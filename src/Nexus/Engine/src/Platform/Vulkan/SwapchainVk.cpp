#if defined(NX_PLATFORM_VULKAN)

	#include "SwapchainVk.hpp"

	#include "CommandQueueVk.hpp"
	#include "FramebufferVk.hpp"
	#include "GraphicsDeviceVk.hpp"
	#include "PlatformVk.hpp"

	#include "Profiling/Profiler.hpp"

namespace Nexus::Graphics
{
	SwapchainVk::SwapchainVk(IGraphicsDevice *graphicsDevice, ICommandQueue *commandQueue, const SwapchainDescription &swapchainSpec)
		: ISwapchain(swapchainSpec),
		  m_SwapchainSize {swapchainSpec.Width, swapchainSpec.Height}
	{
		m_GraphicsDevice = (GraphicsDeviceVk *)graphicsDevice;
		m_CommandQueue	 = (CommandQueueVk *)commandQueue;

		std::shared_ptr<IPhysicalDevice>  physicalDevice   = graphicsDevice->GetPhysicalDevice();
		std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk = std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice);

		GraphicsDeviceVk *graphicsDeviceVk = (GraphicsDeviceVk *)graphicsDevice;
		CreateAll();
	}

	SwapchainVk::~SwapchainVk()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();
		m_GraphicsDevice->WaitForIdle();

		CleanupSwapchain();
		CleanupSemaphores();
		context.DestroySurfaceKHR(m_GraphicsDevice->m_Instance, m_Surface, nullptr);
	}

	void SwapchainVk::SwapBuffers(const SwapchainPresentDescription &presentDesc)
	{
		NX_PROFILE_FUNCTION();

		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		VkQueue vkQueue = m_CommandQueue->GetVkQueue();

		if (!m_SwapchainValid)
		{
			return;
		}

		std::vector<VkRectLayerKHR> presentRectLayers = {};

		for (const Graphics::SwapchainPresentDescription::Rectangle &rect : presentDesc.PresentRects)
		{
			VkRectLayerKHR rectLayer = {};
			rectLayer.offset.x		 = static_cast<int32_t>(rect.X);
			rectLayer.offset.y		 = static_cast<int32_t>(rect.Y);
			rectLayer.extent.width	 = rect.Width;
			rectLayer.extent.height	 = rect.Height;
			rectLayer.layer			 = 0;
			presentRectLayers.push_back(rectLayer);
		}

		VkPresentRegionKHR region = {};
		region.pRectangles		  = presentRectLayers.data();
		region.rectangleCount	  = static_cast<uint32_t>(presentRectLayers.size());

		VkPresentRegionsKHR presentRegions = {};
		presentRegions.sType			   = VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR;
		presentRegions.pNext			   = nullptr;
		presentRegions.swapchainCount	   = 1;
		presentRegions.pRegions			   = &region;

		VkPresentInfoKHR presentInfo   = {};
		presentInfo.sType			   = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext			   = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores	   = &m_PresentSemaphores[m_GraphicsDevice->GetCurrentFrameIndex()];
		presentInfo.swapchainCount	   = 1;
		presentInfo.pSwapchains		   = &m_Swapchain;
		presentInfo.pImageIndices	   = &m_CurrentFrameIndex;

		if (presentDesc.PresentRects.size() > 0 && context.KHR_incremental_present)
		{
			presentInfo.pNext = &presentRegions;
		}

		VkResult result = context.QueuePresentKHR(vkQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image");
		}

		if (context.QueueWaitIdle(vkQueue) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to wait for present queue");
		}

		m_FrameNumber++;
		AcquireNextImage();
	}

	Ref<IFramebuffer> SwapchainVk::GetCurrentFramebuffer()
	{
		return m_Framebuffers.at(m_CurrentFrameIndex);
	}

	void SwapchainVk::SetPresentMode(PresentMode presentMode)
	{
		m_Description.ImagePresentMode = presentMode;
		RecreateSwapchain();
	}

	std::pair<uint32_t, uint32_t> SwapchainVk::GetSize()
	{
		return {m_SwapchainSize.width, m_SwapchainSize.height};
	}

	PixelFormat SwapchainVk::GetColourFormat()
	{
		return Vk::GetNxPixelFormatFromVkPixelFormat(m_SurfaceFormat.format);
	}

	PixelFormat SwapchainVk::GetDepthFormat()
	{
		return m_DepthFormat;
	}

	std::expected<void, std::string> SwapchainVk::Resize(uint32_t width, uint32_t height)
	{
		m_SwapchainSize = {width, height};
		RecreateSwapchain();
		return std::expected<void, std::string>();
	}

	VkSurfaceKHR SwapchainVk::GetSurface()
	{
		return m_Surface;
	}

	VkSurfaceFormatKHR SwapchainVk::GetSurfaceFormat()
	{
		return m_SurfaceFormat;
	}

	void SwapchainVk::RecreateSwapchain()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();
		context.DeviceWaitIdle(m_GraphicsDevice->GetVkDevice());

		CleanupSwapchain();
		CleanupSemaphores();

		CreateAll();
	}

	VkExtent2D SwapchainVk::GetSwapchainSize() const
	{
		return m_SwapchainSize;
	}

	bool SwapchainVk::IsSwapchainValid() const
	{
		return m_SwapchainValid;
	}

	const VkSemaphore &SwapchainVk::GetSemaphore()
	{
		return m_PresentSemaphores[m_GraphicsDevice->GetCurrentFrameIndex()];
	}

	void SwapchainVk::CreateSurface(VkInstance instance)
	{
		Ref<PhysicalDeviceVk>	 physicalDevice = std::dynamic_pointer_cast<PhysicalDeviceVk>(m_GraphicsDevice->GetPhysicalDevice());
		const GladVulkanContext &context		= m_GraphicsDevice->GetVulkanContext();

		if (auto vulkanSurface = std::dynamic_pointer_cast<SurfaceVk>(m_Description.Surface))
		{
			auto result = vulkanSurface->CreateVkSurface(m_Description, instance, context);
			if (result.has_value())
			{
				m_Surface = result.value();
			}
			else
			{
				throw std::runtime_error("Failed to create Vulkan surface");
			}
		}

		context.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->GetVkPhysicalDevice(), m_Surface, &m_SurfaceCapabilities);

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		uint32_t						surfaceFormatCount;
		context.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->GetVkPhysicalDevice(), m_Surface, &surfaceFormatCount, nullptr);
		surfaceFormats.resize(surfaceFormatCount);
		context.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->GetVkPhysicalDevice(), m_Surface, &surfaceFormatCount, surfaceFormats.data());

		if (surfaceFormats.size() > 0)
		{
			m_SurfaceFormat = surfaceFormats[0];
		}
	}

	bool SwapchainVk::CreateSwapchain(std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		uint32_t width = std::clamp(m_SwapchainSize.width, m_SurfaceCapabilities.minImageExtent.width, m_SurfaceCapabilities.maxImageExtent.width);
		uint32_t height =
			std::clamp(m_SwapchainSize.height, m_SurfaceCapabilities.minImageExtent.height, m_SurfaceCapabilities.maxImageExtent.height);

		if (width == 0 || height == 0)
		{
			return false;
		}

		m_SwapchainSize.width  = width;
		m_SwapchainSize.height = height;

		uint32_t imageCount = m_SurfaceCapabilities.minImageCount + 1;
		if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount)
		{
			imageCount = m_SurfaceCapabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface					= m_Surface;
		createInfo.minImageCount			= m_SurfaceCapabilities.minImageCount;
		createInfo.imageFormat				= m_SurfaceFormat.format;
		createInfo.imageColorSpace			= m_SurfaceFormat.colorSpace;
		createInfo.imageExtent				= m_SwapchainSize;
		createInfo.imageArrayLayers			= 1;
		createInfo.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount)
		{
			imageCount = m_SurfaceCapabilities.maxImageCount;
		}

		createInfo.preTransform	  = m_SurfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode	  = Vk::GetVulkanPresentMode(m_Description.ImagePresentMode);
		createInfo.clipped		  = VK_TRUE;

		if (context.CreateSwapchainKHR(m_GraphicsDevice->m_Device, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swapchain");
		}

		std::vector<VkImage> swapchainImages	 = {};
		uint32_t			 swapchainImageCount = 0;

		context.GetSwapchainImagesKHR(m_GraphicsDevice->m_Device, m_Swapchain, &swapchainImageCount, nullptr);
		swapchainImages.resize(swapchainImageCount);
		context.GetSwapchainImagesKHR(m_GraphicsDevice->m_Device, m_Swapchain, &swapchainImageCount, swapchainImages.data());

		m_ColourAttachments.clear();

		for (VkImage image : swapchainImages)
		{
			Graphics::TextureDescription desc = {};
			desc.Width						  = m_SwapchainSize.width;
			desc.Height						  = m_SwapchainSize.height;
			desc.DepthOrArrayLayers			  = 1;
			desc.MipLevels					  = 1;
			desc.Type						  = TextureType::Texture2D;
			desc.Usage						  = Graphics::TextureUsage_ColourAttachment;
			desc.Samples					  = 1;
			desc.Format						  = Vk::GetNxPixelFormatFromVkPixelFormat(m_SurfaceFormat.format);
			Ref<TextureVk> texture			  = CreateRef<TextureVk>(image, desc, m_GraphicsDevice, false);

			m_ColourAttachments.push_back(texture);
		}

		return true;
	}

	void SwapchainVk::CreateDepthStencil(GraphicsDeviceVk *graphicsDevice)
	{
		TextureDescription depthDesc = {};
		depthDesc.Width				 = m_SwapchainSize.width;
		depthDesc.Height			 = m_SwapchainSize.height;
		depthDesc.DepthOrArrayLayers = 1;
		depthDesc.MipLevels			 = 1;
		depthDesc.Type				 = TextureType::Texture2D;
		depthDesc.Usage				 = Graphics::TextureUsage_DepthStencilAttachment;
		depthDesc.Samples			 = m_Description.Samples;
		depthDesc.Format			 = m_DepthFormat;

		m_DepthAttachment = std::dynamic_pointer_cast<TextureVk>(m_GraphicsDevice->CreateTexture(depthDesc));
	}

	void SwapchainVk::CreateResolveAttachment(GraphicsDeviceVk *graphicsDevice)
	{
		TextureDescription resolveDesc = {};
		resolveDesc.Width			   = m_SwapchainSize.width;
		resolveDesc.Height			   = m_SwapchainSize.height;
		resolveDesc.DepthOrArrayLayers = 1;
		resolveDesc.MipLevels		   = 1;
		resolveDesc.Type			   = TextureType::Texture2D;
		resolveDesc.Usage			   = Graphics::TextureUsage_ColourAttachment;
		resolveDesc.Samples			   = m_Description.Samples;
		resolveDesc.Format			   = Vk::GetNxPixelFormatFromVkPixelFormat(m_SurfaceFormat.format);

		m_ResolveAttachment = std::dynamic_pointer_cast<TextureVk>(m_GraphicsDevice->CreateTexture(resolveDesc));
	}

	void SwapchainVk::CreateSemaphores()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo = {};
			semaphoreCreateInfo.sType				  = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCreateInfo.flags				  = 0;

			if (context.CreateSemaphore(m_GraphicsDevice->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_PresentSemaphores[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create semaphore");
			}
		}
	}

	void SwapchainVk::CreateFramebuffers()
	{
		m_Framebuffers.clear();

		for (size_t i = 0; i < m_ColourAttachments.size(); i++)
		{
			Graphics::FramebufferTextureDescription colourAttachmentDesc = {};

			if (m_Description.Samples != 1)
			{
				colourAttachmentDesc.MipLevel		= 0;
				colourAttachmentDesc.BaseArrayLayer = 0;
				colourAttachmentDesc.LayerCount		= 1;
				colourAttachmentDesc.TargetTexture	= m_ResolveAttachment;
			}
			else
			{
				colourAttachmentDesc.MipLevel		= 0;
				colourAttachmentDesc.BaseArrayLayer = 0;
				colourAttachmentDesc.LayerCount		= 1;
				colourAttachmentDesc.TargetTexture	= m_ColourAttachments.at(i);
			}

			Graphics::FramebufferTextureDescription depthAttachmentDesc = {};
			depthAttachmentDesc.MipLevel								= 0;
			depthAttachmentDesc.BaseArrayLayer							= 0;
			depthAttachmentDesc.LayerCount								= 1;
			depthAttachmentDesc.TargetTexture							= m_DepthAttachment;

			std::optional<FramebufferTextureDescription> resolveAttachmentDescOpt = {};

			if (m_Description.Samples != 1)
			{
				Graphics::FramebufferTextureDescription resolveAttachmentDesc = {};
				resolveAttachmentDesc.MipLevel								  = 0;
				resolveAttachmentDesc.BaseArrayLayer						  = 0;
				resolveAttachmentDesc.LayerCount							  = 1;
				resolveAttachmentDesc.TargetTexture							  = m_ColourAttachments.at(i);
				resolveAttachmentDescOpt									  = resolveAttachmentDesc;
			}

			Graphics::FramebufferTextureSetDescription desc = {};
			desc.ColourAttachments							= {
				 FramebufferColourAttachmentDescription {.ColourAttachment = colourAttachmentDesc, .ResolveAttachment = resolveAttachmentDescOpt}};
			desc.DepthAttachment  = depthAttachmentDesc;
			desc.OwnedBySwapchain = true;

			Ref<FramebufferVk> framebuffer = CreateRef<FramebufferVk>(desc, m_GraphicsDevice);
			m_Framebuffers.push_back(framebuffer);
		}
	}

	void SwapchainVk::CreateAll()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();
		context.DestroySurfaceKHR(m_GraphicsDevice->GetVkInstance(), m_Surface, nullptr);

		std::shared_ptr<IPhysicalDevice>  physicalDevice   = m_GraphicsDevice->GetPhysicalDevice();
		std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk = std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice);

		CreateSurface(m_GraphicsDevice->GetVkInstance());
		if (CreateSwapchain(physicalDeviceVk))
		{
			CreateDepthStencil(m_GraphicsDevice);
			CreateResolveAttachment(m_GraphicsDevice);
			CreateSemaphores();
			CreateFramebuffers();
			AcquireNextImage();
			m_SwapchainValid = true;
		}
		else
		{
			m_SwapchainValid = false;
		}
	}

	void SwapchainVk::CleanupSwapchain()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();
		context.DeviceWaitIdle(m_GraphicsDevice->GetVkDevice());
		context.DestroySwapchainKHR(m_GraphicsDevice->m_Device, m_Swapchain, nullptr);
	}

	void SwapchainVk::CleanupSemaphores()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();
		for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			context.DestroySemaphore(m_GraphicsDevice->GetVkDevice(), m_PresentSemaphores[i], nullptr);
		}
	}

	bool SwapchainVk::AcquireNextImage()
	{
		VkResult result = Vk::AcquireNextImage(m_GraphicsDevice,
											   m_Swapchain,
											   UINT64_MAX,
											   m_PresentSemaphores[m_GraphicsDevice->GetCurrentFrameIndex()],
											   VK_NULL_HANDLE,
											   &m_CurrentFrameIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return false;
		}
		else if (result != VK_SUCCESS && result == VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swapchain image");
		}

		return true;
	}

	VkImageView SwapchainVk::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		VkImageViewCreateInfo viewInfo			 = {};
		viewInfo.sType							 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image							 = image;
		viewInfo.viewType						 = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format							 = format;
		viewInfo.subresourceRange.aspectMask	 = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel	 = 0;
		viewInfo.subresourceRange.levelCount	 = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount	 = 1;

		VkImageView imageView;
		if (context.CreateImageView(m_GraphicsDevice->m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view");
		}

		return imageView;
	}

	void SwapchainVk::CreateImage(uint32_t				width,
								  uint32_t				height,
								  VkFormat				format,
								  VkImageTiling			tiling,
								  VkImageUsageFlags		usage,
								  VkMemoryPropertyFlags properties,
								  VkImage			   &image,
								  VkDeviceMemory	   &imageMemory,
								  VkSampleCountFlagBits samples,
								  GraphicsDeviceVk	   *graphicsDevice)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType			= VK_IMAGE_TYPE_2D;
		imageInfo.extent.width		= width;
		imageInfo.extent.height		= height;
		imageInfo.extent.depth		= 1;
		imageInfo.mipLevels			= 1;
		imageInfo.arrayLayers		= 1;
		imageInfo.format			= format;
		imageInfo.tiling			= tiling;
		imageInfo.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage				= usage;
		imageInfo.samples			= samples;
		imageInfo.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;

		if (context.CreateImage(m_GraphicsDevice->GetVkDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image");
		}

		VkMemoryRequirements memRequirements;
		context.GetImageMemoryRequirements(m_GraphicsDevice->GetVkDevice(), image, &memRequirements);

		std::shared_ptr<IPhysicalDevice>  physicalDevice   = m_GraphicsDevice->GetPhysicalDevice();
		std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk = std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType				   = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize	   = memRequirements.size;
		allocInfo.memoryTypeIndex	   = FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDeviceVk->GetVkPhysicalDevice());

		if (context.AllocateMemory(m_GraphicsDevice->GetVkDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate image memory");
		}

		context.BindImageMemory(m_GraphicsDevice->GetVkDevice(), image, imageMemory, 0);
	}

	uint32_t SwapchainVk::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		VkPhysicalDeviceMemoryProperties memProperties;
		context.GetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << 1)) && (memProperties.memoryTypes[i].propertyFlags & properties))
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}

	uint32_t SwapchainVk::GetCurrentFrameIndex()
	{
		return m_FrameNumber % FRAMES_IN_FLIGHT;
	}
}	 // namespace Nexus::Graphics

#endif