#if defined(NX_PLATFORM_VULKAN)

#include "SwapchainVk.hpp"

#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    SwapchainVk::SwapchainVk(Window *window, VSyncState vSyncState, GraphicsDeviceVk *graphicsDevice)
    {
        m_Window = window;
        m_VsyncState = vSyncState;
        m_GraphicsDevice = graphicsDevice;
    }

    SwapchainVk::~SwapchainVk()
    {
        for (int i = 0; i < m_SwapchainFramebuffers.size(); i++)
        {
            vkDestroyFramebuffer(m_GraphicsDevice->GetVkDevice(), m_SwapchainFramebuffers[i], nullptr);
        }

        vkDestroyImageView(m_GraphicsDevice->GetVkDevice(), m_DepthImageView, nullptr);
        vkFreeMemory(m_GraphicsDevice->GetVkDevice(), m_DepthImageMemory, nullptr);
        vkDestroyImage(m_GraphicsDevice->GetVkDevice(), m_DepthImage, nullptr);

        for (int i = 0; i < m_SwapchainImageCount; i++)
        {
            vkDestroyImageView(m_GraphicsDevice->GetVkDevice(), m_SwapchainImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(m_GraphicsDevice->GetVkDevice(), m_Swapchain, nullptr);
        vkDestroySurfaceKHR(m_GraphicsDevice->m_Instance, m_Surface, nullptr);
    }

    void SwapchainVk::SwapBuffers()
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_GraphicsDevice->GetCurrentFrame().PresentSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_GraphicsDevice->m_CurrentFrameIndex;

        VkResult result = vkQueuePresentKHR(m_GraphicsDevice->m_PresentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            m_GraphicsDevice->RecreateSwapchain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swapchain image");
        }

        if (vkQueueWaitIdle(m_GraphicsDevice->m_PresentQueue) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to wait for present queue");
        }

        m_GraphicsDevice->m_FrameNumber++;
    }

    VSyncState SwapchainVk::GetVsyncState()
    {
        return VSyncState();
    }

    void SwapchainVk::SetVSyncState(VSyncState vsyncState)
    {
    }

    const VkFramebuffer &SwapchainVk::GetCurrentFramebuffer()
    {
        return m_SwapchainFramebuffers[m_GraphicsDevice->m_CurrentFrameIndex];
    }

    VkSurfaceFormatKHR SwapchainVk::GetSurfaceFormat()
    {
        return m_SurfaceFormat;
    }

    VkFormat SwapchainVk::GetDepthFormat()
    {
        return m_DepthFormat;
    }

    void SwapchainVk::CreateSurface()
    {
        if (!SDL_Vulkan_CreateSurface(m_Window->GetSDLWindowHandle(), m_GraphicsDevice->m_Instance, &m_Surface))
        {
            throw std::runtime_error("Failed to create surface");
        }
    }

    void SwapchainVk::CreateSwapchain()
    {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_GraphicsDevice->m_PhysicalDevice, m_Surface, &m_SurfaceCapabilities);

        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        uint32_t surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_GraphicsDevice->m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr);
        surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_GraphicsDevice->m_PhysicalDevice, m_Surface, &surfaceFormatCount, surfaceFormats.data());

        if (surfaceFormats[0].format != VK_FORMAT_B8G8R8A8_UNORM)
        {
            throw std::runtime_error("surfaceFormats[0].format != VK_FORMAT_B8G8R8A8_UNORM");
        }

        m_SurfaceFormat = surfaceFormats[0];
        int width = 0, height = 0;
        SDL_Vulkan_GetDrawableSize(m_Window->GetSDLWindowHandle(), &width, &height);
        width = std::clamp(width, (int)m_SurfaceCapabilities.minImageExtent.width, (int)m_SurfaceCapabilities.maxImageExtent.width);
        height = std::clamp(height, (int)m_SurfaceCapabilities.minImageExtent.height, (int)m_SurfaceCapabilities.maxImageExtent.height);

        m_SwapchainSize.width = width;
        m_SwapchainSize.height = height;

        uint32_t imageCount = m_SurfaceCapabilities.minImageCount + 1;
        if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount)
        {
            imageCount = m_SurfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;
        createInfo.minImageCount = m_SurfaceCapabilities.minImageCount;
        createInfo.imageFormat = m_SurfaceFormat.format;
        createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        createInfo.imageExtent = m_SwapchainSize;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {m_GraphicsDevice->m_GraphicsQueueFamilyIndex, m_GraphicsDevice->m_PresentQueueFamilyIndex};
        if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount)
        {
            imageCount = m_SurfaceCapabilities.maxImageCount;
        }

        createInfo.preTransform = m_SurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = m_PresentMode;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(m_GraphicsDevice->m_Device, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swapchain");
        }

        vkGetSwapchainImagesKHR(m_GraphicsDevice->m_Device, m_Swapchain, &m_SwapchainImageCount, nullptr);
        m_SwapchainImages.resize(m_SwapchainImageCount);
        vkGetSwapchainImagesKHR(m_GraphicsDevice->m_Device, m_Swapchain, &m_SwapchainImageCount, m_SwapchainImages.data());
    }

    void SwapchainVk::CreateSwapchainImageViews()
    {
        m_SwapchainImageViews.resize(m_SwapchainImages.size());

        for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
        {
            m_SwapchainImageViews[i] = CreateImageView(m_SwapchainImages[i], m_SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void SwapchainVk::CreateDepthStencil()
    {
        VkBool32 validDepthFormat = GetSupportedDepthFormat(m_GraphicsDevice->m_PhysicalDevice, &m_DepthFormat);
        CreateImage(m_SwapchainSize.width, m_SwapchainSize.height, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
        m_DepthImageView = CreateImageView(m_DepthImage, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void SwapchainVk::CreateFramebuffers()
    {
        m_SwapchainFramebuffers.resize(m_SwapchainImageViews.size());

        for (size_t i = 0; i < m_SwapchainImageViews.size(); i++)
        {
            std::vector<VkImageView> attachments(2);
            attachments[0] = m_SwapchainImageViews[i];
            attachments[1] = m_DepthImageView;

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_GraphicsDevice->m_SwapchainRenderPass;
            framebufferInfo.attachmentCount = (uint32_t)attachments.size();
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_SwapchainSize.width;
            framebufferInfo.height = m_SwapchainSize.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_GraphicsDevice->m_Device, &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create framebuffer");
            }
        }
    }

    void SwapchainVk::CleanupSwapchain()
    {
        for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
        {
            vkDestroyFramebuffer(m_GraphicsDevice->m_Device, m_SwapchainFramebuffers[i], nullptr);
        }

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

    VkImageView SwapchainVk::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(m_GraphicsDevice->m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture image view");
        }

        return imageView;
    }

    VkBool32 SwapchainVk::GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat)
    {
        std::vector<VkFormat> depthFormats =
            {
                VK_FORMAT_D32_SFLOAT_S8_UINT,
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

    void SwapchainVk::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory)
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(m_GraphicsDevice->m_Device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_GraphicsDevice->m_Device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

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
}

#endif