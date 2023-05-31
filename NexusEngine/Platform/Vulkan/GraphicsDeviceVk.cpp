#include "GraphicsDeviceVk.h"

#include "SDL_vulkan.h"

#include <set>

namespace Nexus
{
    GraphicsDeviceVk::GraphicsDeviceVk(const GraphicsDeviceCreateInfo &createInfo)
        : GraphicsDevice(createInfo)
    {
        CreateInstance();
        CreateDebug();
        CreateSurface();
        SelectPhysicalDevice();
        SelectQueueFamily();
        CreateDevice();

        CreateSwapchain();
        CreateSwapchainImageViews();
        SetupDepthStencil();
        CreateRenderPass();
        CreateFramebuffers();

        CreateCommandPool();
        CreateCommandBuffer();
        CreateSemaphores();
        CreateFences();
    }

    GraphicsDeviceVk::~GraphicsDeviceVk()
    {
    }

    void GraphicsDeviceVk::SetContext()
    {
    }

    void GraphicsDeviceVk::Clear(float red, float green, float blue, float alpha)
    {
    }

    void GraphicsDeviceVk::SetFramebuffer(Ref<Framebuffer> framebuffer)
    {
    }

    void GraphicsDeviceVk::DrawElements(PrimitiveType type, uint32_t start, uint32_t count)
    {
    }

    void GraphicsDeviceVk::DrawIndexed(PrimitiveType type, uint32_t count, uint32_t offset)
    {
    }

    void GraphicsDeviceVk::SetViewport(const Viewport &viewport)
    {
    }

    const Viewport &GraphicsDeviceVk::GetViewport()
    {
        // TODO: insert return statement here

        return Viewport();
    }

    const char *GraphicsDeviceVk::GetAPIName()
    {
        return nullptr;
    }

    const char *GraphicsDeviceVk::GetDeviceName()
    {
        return nullptr;
    }

    void *GraphicsDeviceVk::GetContext()
    {
        return nullptr;
    }

    void GraphicsDeviceVk::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
    }

    void GraphicsDeviceVk::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
    }

    void GraphicsDeviceVk::SetShader(Ref<Shader> shader)
    {
    }

    Ref<Shader> GraphicsDeviceVk::CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout)
    {
        return Ref<Shader>();
    }
    Ref<VertexBuffer> GraphicsDeviceVk::CreateVertexBuffer(const std::vector<Vertex> vertices)
    {
        return Ref<VertexBuffer>();
    }

    Ref<IndexBuffer> GraphicsDeviceVk::CreateIndexBuffer(const std::vector<unsigned int> indices)
    {
        return Ref<IndexBuffer>();
    }

    Ref<UniformBuffer> GraphicsDeviceVk::CreateUniformBuffer(const UniformResourceBinding &binding)
    {
        return Ref<UniformBuffer>();
    }

    Ref<Texture> GraphicsDeviceVk::CreateTexture(TextureSpecification spec)
    {
        return Ref<Texture>();
    }

    Ref<Framebuffer> GraphicsDeviceVk::CreateFramebuffer(const Nexus::FramebufferSpecification &spec)
    {
        return Ref<Framebuffer>();
    }
    void GraphicsDeviceVk::InitialiseImGui()
    {
    }

    void GraphicsDeviceVk::BeginImGuiRender()
    {
    }

    void GraphicsDeviceVk::EndImGuiRender()
    {
    }

    void GraphicsDeviceVk::Resize(Point<int> size)
    {
    }

    void GraphicsDeviceVk::SwapBuffers()
    {
    }

    void GraphicsDeviceVk::SetVSyncState(VSyncState vSyncState)
    {
    }

    VSyncState GraphicsDeviceVk::GetVsyncState()
    {
        return VSyncState();
    }

    uint32_t frameIndex;
    VkCommandBuffer commandBuffer;
    VkImage image;
    void GraphicsDeviceVk::AcquireNextImage()
    {
        vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &frameIndex);
        vkWaitForFences(m_Device, 1, &m_Fences[frameIndex], VK_FALSE, UINT64_MAX);
        vkResetFences(m_Device, 1, &m_Fences[frameIndex]);

        commandBuffer = m_CommandBuffers[frameIndex];
        image = m_SwapchainImages[frameIndex];
    }

    void GraphicsDeviceVk::ResetCommandBuffer()
    {
        vkResetCommandBuffer(commandBuffer, 0);
    }

    void GraphicsDeviceVk::BeginCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin command buffer");
        }
    }

    void GraphicsDeviceVk::EndCommandBuffer()
    {
        vkEndCommandBuffer(commandBuffer);
    }

    void GraphicsDeviceVk::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
    }

    void GraphicsDeviceVk::BeginRenderPass(VkClearColorValue clear_color, VkClearDepthStencilValue clear_depth_stencil)
    {
        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = m_RenderPass;
        render_pass_info.framebuffer = m_SwapchainFramebuffers[frameIndex];
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = m_SwapchainSize;
        render_pass_info.clearValueCount = 1;

        std::vector<VkClearValue> clearValues(2);
        clearValues[0].color = clear_color;
        clearValues[1].depthStencil = clear_depth_stencil;

        render_pass_info.clearValueCount = (uint32_t)clearValues.size();
        render_pass_info.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    void GraphicsDeviceVk::EndRenderPass()
    {
        vkCmdEndRenderPass(commandBuffer);
    }

    VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    void GraphicsDeviceVk::QueueSubmit()
    {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphore;
        submitInfo.pWaitDstStageMask = &waitDestStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_RenderingFinishedSemaphore;
        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_Fences[frameIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit queue");
        }
    }

    void GraphicsDeviceVk::QueuePresent()
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_RenderingFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &frameIndex;

        if (vkQueuePresentKHR(m_PresentQueue, &presentInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present queue");
        }

        if (vkQueueWaitIdle(m_PresentQueue) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to wait for present queue");
        }
    }

    void GraphicsDeviceVk::SetViewport(int width, int height)
    {
        VkViewport viewport;
        viewport.width = (float)width / 2;
        viewport.height = (float)height;
        viewport.minDepth = (float)0.0f;
        viewport.maxDepth = (float)1.0f;
        viewport.x = 0;
        viewport.y = 0;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    void GraphicsDeviceVk::SetScissor(int width, int height)
    {
        VkRect2D scissor;
        scissor.extent.width = width / 2;
        scissor.extent.height = height;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    const std::vector<const char *> validationLayers =
        {
            "VK_LAYER_KHRONOS_validation"};

    void GraphicsDeviceVk::CreateInstance()
    {
        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(m_Window->GetSDLWindowHandle(), &extensionCount, nullptr);
        std::vector<const char *> extensionNames(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(m_Window->GetSDLWindowHandle(), &extensionCount, extensionNames.data());

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan Application";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "Engine";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledLayerCount = validationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        instanceCreateInfo.enabledExtensionCount = extensionNames.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance");
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanReportFunc(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char *layerPrefix,
        const char *msg,
        void *userData)
    {
        std::cout << "VULKAN VALIDATION: " << msg << std::endl;
        return VK_FALSE;
    }

    PFN_vkCreateDebugReportCallbackEXT SDL2_vkCreateDebugReportCallbackEXT = nullptr;
    void GraphicsDeviceVk::CreateDebug()
    {
        SDL2_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)SDL_Vulkan_GetVkGetInstanceProcAddr();
        VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo = {};
        debugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debugCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        debugCallbackCreateInfo.pfnCallback = VulkanReportFunc;

        SDL2_vkCreateDebugReportCallbackEXT(m_Instance, &debugCallbackCreateInfo, 0, &m_DebugCallback);
    }

    void GraphicsDeviceVk::CreateSurface()
    {
        if (!SDL_Vulkan_CreateSurface(m_Window->GetSDLWindowHandle(), m_Instance, &m_Surface))
        {
            throw std::runtime_error("Failed to create surface");
        }
    }

    void GraphicsDeviceVk::SelectPhysicalDevice()
    {
        std::vector<VkPhysicalDevice> physicalDevices;
        uint32_t physicalDeviceCount = 0;

        vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
        physicalDevices.resize(physicalDeviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data());

        m_PhysicalDevice = physicalDevices[0];
    }

    void GraphicsDeviceVk::SelectQueueFamily()
    {
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        uint32_t queueFamilyCount;

        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
        queueFamilyProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        int graphicIndex = -1;
        int presentIndex = -1;

        int i = 0;
        for (const auto &queueFamily : queueFamilyProperties)
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicIndex = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport)
            {
                presentIndex = i;
            }

            if (graphicIndex != -1 && presentIndex != -1)
            {
                break;
            }

            i++;
        }

        if (graphicIndex == -1 || presentIndex == -1)
        {
            throw std::runtime_error("Failed to find a graphics or present queue");
        }

        m_GraphicsQueueFamilyIndex = graphicIndex;
        m_PresentQueueFamilyIndex = presentIndex;
    }

    void GraphicsDeviceVk::CreateDevice()
    {
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        const float queue_priority[] = {1.0f};

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex};

        float queuePriority = queue_priority[0];
        for (int queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create device");
        }

        vkGetDeviceQueue(m_Device, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_PresentQueueFamilyIndex, 0, &m_PresentQueue);
    }

    void GraphicsDeviceVk::CreateSwapchain()
    {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &m_SurfaceCapabilities);

        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        uint32_t surfaceFormatsCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatsCount, nullptr);
        surfaceFormats.resize(surfaceFormatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatsCount, surfaceFormats.data());

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

        uint32_t queueFamilyIndices[] = {m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex};
        if (m_GraphicsQueueFamilyIndex != m_PresentQueueFamilyIndex)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = m_SurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swapchain");
        }

        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &m_SwapchainImageCount, nullptr);
        m_SwapchainImages.resize(m_SwapchainImageCount);
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &m_SwapchainImageCount, m_SwapchainImages.data());
    }

    void GraphicsDeviceVk::CreateSwapchainImageViews()
    {
        m_SwapchainImageViews.resize(m_SwapchainImages.size());

        for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
        {
            m_SwapchainImageViews[i] = CreateImageView(m_SwapchainImages[i], m_SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void GraphicsDeviceVk::SetupDepthStencil()
    {
        VkBool32 validDepthFormat = GetSupportedDepthFormat(m_PhysicalDevice, &m_DepthFormat);
        CreateImage(m_SwapchainSize.width, m_SwapchainSize.height, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    m_DepthImage, m_DepthImageMemory);
        m_DepthImageView = CreateImageView(m_DepthImage, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void GraphicsDeviceVk::CreateRenderPass()
    {
        std::vector<VkAttachmentDescription> attachments(2);

        attachments[0].format = m_SurfaceFormat.format;
        attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachments[1].format = m_DepthFormat;
        attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorReference = {};
        colorReference.attachment = 0;
        colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthReference = {};
        depthReference.attachment = 1;
        depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorReference;
        subpassDescription.pDepthStencilAttachment = &depthReference;
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.pInputAttachments = nullptr;
        subpassDescription.preserveAttachmentCount = 0;
        subpassDescription.pPreserveAttachments = nullptr;
        subpassDescription.pResolveAttachments = nullptr;

        std::vector<VkSubpassDependency> dependencies(1);
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescription;
        renderPassInfo.dependencyCount = (uint32_t)dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
    }

    void GraphicsDeviceVk::CreateFramebuffers()
    {
        m_SwapchainFramebuffers.resize(m_SwapchainImageViews.size());

        for (size_t i = 0; i < m_SwapchainImageViews.size(); i++)
        {
            std::vector<VkImageView> attachments(2);
            attachments[0] = m_SwapchainImageViews[i];
            attachments[1] = m_DepthImageView;

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = (uint32_t)attachments.size();
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_SwapchainSize.width;
            framebufferInfo.height = m_SwapchainSize.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create framebuffer");
            }
        }
    }

    void GraphicsDeviceVk::CreateCommandPool()
    {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        createInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
        if (vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool");
        }
    }

    void GraphicsDeviceVk::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = m_CommandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = m_SwapchainImageCount;

        m_CommandBuffers.resize(m_SwapchainImageCount);
        if (vkAllocateCommandBuffers(m_Device, &allocateInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers");
        }
    }

    void GraphicsDeviceVk::CreateSemaphores()
    {
        CreateSemaphore(&m_ImageAvailableSemaphore);
        CreateSemaphore(&m_RenderingFinishedSemaphore);
    }

    void GraphicsDeviceVk::CreateFences()
    {
        m_Fences.resize(m_SwapchainImageCount);
        for (uint32_t i = 0; i < m_SwapchainImageCount; i++)
        {
            VkFenceCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            if (vkCreateFence(m_Device, &createInfo, nullptr, &m_Fences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create fence");
            }
        }
    }

    VkImageView GraphicsDeviceVk::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
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
        if (vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture image view");
        }

        return imageView;
    }

    VkBool32 GraphicsDeviceVk::GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat)
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

    uint32_t GraphicsDeviceVk::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties))
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }

    void GraphicsDeviceVk::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory)
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

        if (vkCreateImage(m_Device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate image memory");
        }

        vkBindImageMemory(m_Device, image, imageMemory, 0);
    }

    void GraphicsDeviceVk::CreateSemaphore(VkSemaphore *semaphore)
    {
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(m_Device, &createInfo, nullptr, semaphore) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create semaphore");
        }
    }
}