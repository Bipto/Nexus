#if defined(NX_PLATFORM_VULKAN)

#include "SwapchainVk.hpp"

#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    VkPresentModeKHR GetPresentMode(VSyncState vSyncState)
    {
        switch (vSyncState)
        {
        case VSyncState::Enabled:
            return VK_PRESENT_MODE_FIFO_KHR;
        case VSyncState::Disabled:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    SwapchainVk::SwapchainVk(Window *window, GraphicsDevice *graphicsDevice, const SwapchainSpecification &swapchainSpec)
        : Swapchain(swapchainSpec), m_Window(window), m_VsyncState(swapchainSpec.VSyncState)
    {
        m_GraphicsDevice = (GraphicsDeviceVk *)graphicsDevice;

        CreateSurface();
    }

    SwapchainVk::~SwapchainVk()
    {
        // cleanup swapchain
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
        }

        // cleanup depth/stencil
        {
            vkDestroyImageView(m_GraphicsDevice->GetVkDevice(), m_DepthImageView, nullptr);
            vkDestroyImage(m_GraphicsDevice->GetVkDevice(), m_DepthImage, nullptr);
            vkFreeMemory(m_GraphicsDevice->GetVkDevice(), m_DepthImageMemory, nullptr);
        }

        // cleanup surface
        {
            vkDestroySurfaceKHR(m_GraphicsDevice->m_Instance, m_Surface, nullptr);
        }
    }

    void SwapchainVk::SwapBuffers()
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_GraphicsDevice->GetCurrentFrame().PresentSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_CurrentFrameIndex;

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

        m_FrameNumber++;
    }

    VSyncState SwapchainVk::GetVsyncState()
    {
        return m_VsyncState;
    }

    void SwapchainVk::SetVSyncState(VSyncState vsyncState)
    {
    }

    const VkFramebuffer &SwapchainVk::GetCurrentFramebuffer()
    {
        return m_SwapchainFramebuffers[m_CurrentFrameIndex];
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
        AcquireNextImage();
    }

    void SwapchainVk::Initialise()
    {
        CreateSwapchain();
        CreateSwapchainImageViews();
        CreateDepthStencil();
        CreateRenderPass();
        CreateFramebuffers();
    }

    void SwapchainVk::RecreateSwapchain()
    {
        vkDeviceWaitIdle(m_GraphicsDevice->GetVkDevice());

        CleanupSwapchain();
        CleanupDepthStencil();

        Initialise();
    }

    VkRenderPass SwapchainVk::GetRenderPass()
    {
        return m_SwapchainRenderPass;
    }

    uint32_t SwapchainVk::GetImageCount()
    {
        return m_SwapchainImageCount;
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
        createInfo.presentMode = GetPresentMode(m_VsyncState);
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

    void SwapchainVk::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_SurfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment = {};
        depthAttachment.flags = 0;
        depthAttachment.format = m_DepthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentReference = {};
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;
        subpass.pDepthStencilAttachment = &depthAttachmentReference;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkSubpassDependency depthDependency;
        depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depthDependency.dstSubpass = 0;
        depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.srcAccessMask = 0;
        depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        depthDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkSubpassDependency dependencies[2] = {dependency, depthDependency};
        VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.pAttachments = &attachments[0];
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = &dependencies[0];

        if (vkCreateRenderPass(m_GraphicsDevice->GetVkDevice(), &renderPassInfo, nullptr, &m_SwapchainRenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
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
            framebufferInfo.renderPass = m_SwapchainRenderPass;
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

    bool SwapchainVk::AcquireNextImage()
    {
        VkResult result = vkAcquireNextImageKHR(m_GraphicsDevice->GetVkDevice(), m_Swapchain, 0, m_GraphicsDevice->GetCurrentFrame().PresentSemaphore, VK_NULL_HANDLE, &m_CurrentFrameIndex);

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

    uint32_t SwapchainVk::GetCurrentFrameIndex()
    {
        return m_FrameNumber % FRAMES_IN_FLIGHT;
    }
}

#endif