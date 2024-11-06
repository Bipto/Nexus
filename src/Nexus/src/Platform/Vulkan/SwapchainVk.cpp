#if defined(NX_PLATFORM_VULKAN)

	#include "SwapchainVk.hpp"

	#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
	VkPresentModeKHR GetPresentMode(VSyncState vSyncState)
	{
		switch (vSyncState)
		{
			case VSyncState::Enabled: return VK_PRESENT_MODE_FIFO_KHR;
			case VSyncState::Disabled: return VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	SwapchainVk::SwapchainVk(Window *window, GraphicsDevice *graphicsDevice, const SwapchainSpecification &swapchainSpec)
		: Swapchain(swapchainSpec),
		  m_Window(window),
		  m_VsyncState(swapchainSpec.VSyncState)
	{
		m_GraphicsDevice = (GraphicsDeviceVk *)graphicsDevice;

		CreateSurface();

		window->OnResize.Bind([&](const WindowResizedEventArgs &args) { RecreateSwapchain(); });
	}

	SwapchainVk::~SwapchainVk()
	{
		CleanupResolveAttachment();
		CleanupSwapchain();
		CleanupDepthStencil();
		CleanupSemaphores();
		vkDestroySurfaceKHR(m_GraphicsDevice->m_Instance, m_Surface, nullptr);
	}

	void SwapchainVk::SwapBuffers()
	{
		if (!m_SwapchainValid)
		{
			return;
		}

		// prevents the swapchain being presented on the first frame that the window
		// is minimized
		if (m_Window->GetCurrentWindowState() == WindowState::Minimized)
		{
			return;
		}

		m_GraphicsDevice->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				m_GraphicsDevice->TransitionVulkanImageLayout(cmd,
															  GetColourImage(),
															  0,
															  0,
															  GetColorImageLayout(),
															  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
															  VK_IMAGE_ASPECT_COLOR_BIT);
				SetColorImageLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			});

		VkPresentInfoKHR presentInfo   = {};
		presentInfo.sType			   = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores	   = &m_PresentSemaphores[m_GraphicsDevice->GetCurrentFrameIndex()];
		presentInfo.swapchainCount	   = 1;
		presentInfo.pSwapchains		   = &m_Swapchain;
		presentInfo.pImageIndices	   = &m_CurrentFrameIndex;

		VkResult result = vkQueuePresentKHR(m_GraphicsDevice->m_PresentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image");
		}

		if (vkQueueWaitIdle(m_GraphicsDevice->m_PresentQueue) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to wait for present queue");
		}

		m_FrameNumber++;
	}

	VSyncState SwapchainVk::GetVsyncState()
	{
		return m_VsyncState;
	}

	void SwapchainVk::SetVSyncState(VSyncState vsyncState)
	{
	}

	Nexus::Point2D<uint32_t> SwapchainVk::GetSize()
	{
		return {m_SwapchainSize.width, m_SwapchainSize.height};
	}

	VkSurfaceFormatKHR SwapchainVk::GetSurfaceFormat()
	{
		return m_SurfaceFormat;
	}

	VkFormat SwapchainVk::GetDepthFormat()
	{
		return m_DepthFormat;
	}

	void SwapchainVk::Prepare()
	{
		if (!m_SwapchainValid)
		{
			return;
		}

		AcquireNextImage();
	}

	void SwapchainVk::Initialise()
	{
		if (CreateSwapchain())
		{
			CreateSwapchainImageViews();
			CreateDepthStencil();
			CreateResolveAttachment();
			CreateSemaphores();
			m_SwapchainValid = true;
		}
		else
		{
			m_SwapchainValid = false;
		}
	}

	void SwapchainVk::RecreateSwapchain()
	{
		vkDeviceWaitIdle(m_GraphicsDevice->GetVkDevice());

		CleanupResolveAttachment();
		CleanupSwapchain();
		CleanupDepthStencil();
		CleanupSemaphores();

		Initialise();
	}

	uint32_t SwapchainVk::GetImageCount()
	{
		return m_SwapchainImageCount;
	}

	VkExtent2D SwapchainVk::GetSwapchainSize() const
	{
		return m_SwapchainSize;
	}

	VkImage SwapchainVk::GetColourImage()
	{
		return m_SwapchainImages[m_CurrentFrameIndex];
	}

	VkImage SwapchainVk::GetDepthImage()
	{
		return m_DepthImage;
	}

	VkImage SwapchainVk::GetResolveImage()
	{
		return m_ResolveImage;
	}

	VkImageView SwapchainVk::GetColourImageView()
	{
		return m_SwapchainImageViews[m_CurrentFrameIndex];
	}

	VkImageView SwapchainVk::GetDepthImageView()
	{
		return m_DepthImageView;
	}

	VkImageView SwapchainVk::GetResolveImageView()
	{
		return m_ResolveImageView;
	}

	VkImageLayout SwapchainVk::GetColorImageLayout()
	{
		return m_ImageLayouts[m_CurrentFrameIndex];
	}

	VkImageLayout SwapchainVk::GetDepthImageLayout()
	{
		return m_DepthLayout;
	}

	VkImageLayout SwapchainVk::GetResolveImageLayout()
	{
		return m_ResolveImageLayout;
	}

	void SwapchainVk::SetColorImageLayout(VkImageLayout layout)
	{
		m_ImageLayouts[m_CurrentFrameIndex] = layout;
	}

	void SwapchainVk::SetDepthImageLayout(VkImageLayout layout)
	{
		m_DepthLayout = layout;
	}

	void SwapchainVk::SetResolveImageLayout(VkImageLayout layout)
	{
		m_ResolveImageLayout = layout;
	}

	bool SwapchainVk::IsSwapchainValid() const
	{
		return m_SwapchainValid;
	}

	const VkSemaphore &SwapchainVk::GetSemaphore()
	{
		return m_PresentSemaphores[m_GraphicsDevice->GetCurrentFrameIndex()];
	}

	void SwapchainVk::CreateSurface()
	{
		if (!SDL_Vulkan_CreateSurface(m_Window->GetSDLWindowHandle(), m_GraphicsDevice->m_Instance, nullptr, &m_Surface))
		{
			throw std::runtime_error("Failed to create surface");
		}
	}

	bool SwapchainVk::CreateSwapchain()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_GraphicsDevice->m_PhysicalDevice, m_Surface, &m_SurfaceCapabilities);

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		uint32_t						surfaceFormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_GraphicsDevice->m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr);
		surfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_GraphicsDevice->m_PhysicalDevice, m_Surface, &surfaceFormatCount, surfaceFormats.data());

		if (surfaceFormats.size() > 0)
		{
			m_SurfaceFormat = surfaceFormats[0];
		}

		int width = 0, height = 0;
		SDL_GetWindowSizeInPixels(m_Window->GetSDLWindowHandle(), &width, &height);
		// SDL_Vulkan_GetDrawableSize(m_Window->GetSDLWindowHandle(), &width,
		// &height);
		width  = std::clamp(width, (int)m_SurfaceCapabilities.minImageExtent.width, (int)m_SurfaceCapabilities.maxImageExtent.width);
		height = std::clamp(height, (int)m_SurfaceCapabilities.minImageExtent.height, (int)m_SurfaceCapabilities.maxImageExtent.height);

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

		uint32_t queueFamilyIndices[] = {m_GraphicsDevice->m_GraphicsQueueFamilyIndex, m_GraphicsDevice->m_PresentQueueFamilyIndex};
		if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount)
		{
			imageCount = m_SurfaceCapabilities.maxImageCount;
		}

		createInfo.preTransform	  = m_SurfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode	  = GetPresentMode(m_VsyncState);
		createInfo.clipped		  = VK_TRUE;

		if (vkCreateSwapchainKHR(m_GraphicsDevice->m_Device, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swapchain");
		}

		vkGetSwapchainImagesKHR(m_GraphicsDevice->m_Device, m_Swapchain, &m_SwapchainImageCount, nullptr);
		m_SwapchainImages.resize(m_SwapchainImageCount);
		vkGetSwapchainImagesKHR(m_GraphicsDevice->m_Device, m_Swapchain, &m_SwapchainImageCount, m_SwapchainImages.data());

		return true;
	}

	void SwapchainVk::CreateSwapchainImageViews()
	{
		m_SwapchainImageViews.resize(m_SwapchainImages.size());
		m_ImageLayouts.resize(m_SwapchainImages.size());

		for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			m_SwapchainImageViews[i] = CreateImageView(m_SwapchainImages[i], m_SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
			m_ImageLayouts[i]		 = VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	void SwapchainVk::CreateDepthStencil()
	{
		auto	 samples		  = Vk::GetVkSampleCount(m_Specification.Samples);
		VkBool32 validDepthFormat = GetSupportedDepthFormat(m_GraphicsDevice->m_PhysicalDevice, &m_DepthFormat);
		CreateImage(m_SwapchainSize.width,
					m_SwapchainSize.height,
					VK_FORMAT_D24_UNORM_S8_UINT,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					m_DepthImage,
					m_DepthImageMemory,
					samples);
		m_DepthImageView = CreateImageView(m_DepthImage, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT);
		m_DepthLayout	 = VK_IMAGE_LAYOUT_UNDEFINED;
	}

	void SwapchainVk::CreateResolveAttachment()
	{
		auto samples = Vk::GetVkSampleCount(m_Specification.Samples);

		CreateImage(m_SwapchainSize.width,
					m_SwapchainSize.height,
					m_SurfaceFormat.format,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					m_ResolveImage,
					m_ResolveMemory,
					samples);

		m_ResolveImageView	 = CreateImageView(m_ResolveImage, m_SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
		m_ResolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	}

	void SwapchainVk::CreateSemaphores()
	{
		for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo = {};
			semaphoreCreateInfo.sType				  = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCreateInfo.flags				  = 0;

			if (vkCreateSemaphore(m_GraphicsDevice->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_PresentSemaphores[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create semaphore");
			}
		}
	}

	void SwapchainVk::CleanupSwapchain()
	{
		vkDeviceWaitIdle(m_GraphicsDevice->GetVkDevice());

		for (size_t i = 0; i < m_SwapchainImageViews.size(); i++)
		{
			vkDestroyImageView(m_GraphicsDevice->m_Device, m_SwapchainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(m_GraphicsDevice->m_Device, m_Swapchain, nullptr);
	}

	void SwapchainVk::CleanupDepthStencil()
	{
		vkDestroyImageView(m_GraphicsDevice->m_Device, m_DepthImageView, nullptr);
		vkDestroyImage(m_GraphicsDevice->m_Device, m_DepthImage, nullptr);
		vkFreeMemory(m_GraphicsDevice->m_Device, m_DepthImageMemory, nullptr);
	}

	void SwapchainVk::CleanupResolveAttachment()
	{
		vkDestroyImageView(m_GraphicsDevice->m_Device, m_ResolveImageView, nullptr);
		vkDestroyImage(m_GraphicsDevice->m_Device, m_ResolveImage, nullptr);
		vkFreeMemory(m_GraphicsDevice->m_Device, m_ResolveMemory, nullptr);
	}

	void SwapchainVk::CleanupSemaphores()
	{
		for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) { vkDestroySemaphore(m_GraphicsDevice->GetVkDevice(), m_PresentSemaphores[i], nullptr); }
	}

	bool SwapchainVk::AcquireNextImage()
	{
		VkResult result = vkAcquireNextImageKHR(m_GraphicsDevice->GetVkDevice(),
												m_Swapchain,
												0,
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

		m_CurrentImage = m_SwapchainImages[m_CurrentFrameIndex];

		return true;
	}

	VkImageView SwapchainVk::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
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
		if (vkCreateImageView(m_GraphicsDevice->m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view");
		}

		return imageView;
	}

	VkBool32 SwapchainVk::GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat)
	{
		std::vector<VkFormat> depthFormats = {VK_FORMAT_D32_SFLOAT_S8_UINT,
											  VK_FORMAT_D32_SFLOAT,
											  VK_FORMAT_D24_UNORM_S8_UINT,
											  VK_FORMAT_D16_UNORM_S8_UINT,
											  VK_FORMAT_D16_UNORM};

		for (auto &format : depthFormats)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				*depthFormat = format;
				return true;
			}
		}

		return false;
	}

	void SwapchainVk::CreateImage(uint32_t				width,
								  uint32_t				height,
								  VkFormat				format,
								  VkImageTiling			tiling,
								  VkImageUsageFlags		usage,
								  VkMemoryPropertyFlags properties,
								  VkImage			   &image,
								  VkDeviceMemory	   &imageMemory,
								  VkSampleCountFlagBits samples)
	{
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

		if (vkCreateImage(m_GraphicsDevice->m_Device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_GraphicsDevice->m_Device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType				   = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize	   = memRequirements.size;
		allocInfo.memoryTypeIndex	   = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_GraphicsDevice->m_Device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate image memory");
		}

		vkBindImageMemory(m_GraphicsDevice->m_Device, image, imageMemory, 0);
	}

	uint32_t SwapchainVk::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_GraphicsDevice->m_PhysicalDevice, &memProperties);

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