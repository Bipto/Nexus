#if defined(NX_PLATFORM_VULKAN)

#include "GraphicsDeviceVk.hpp"
#include "CommandListVk.hpp"
#include "RenderPassVk.hpp"
#include "ShaderVk.hpp"
#include "PipelineVk.hpp"
#include "BufferVk.hpp"
#include "TextureVk.hpp"
#include "ResourceSetVk.hpp"
#include "FramebufferVk.hpp"

#include "SDL_vulkan.h"

#include <set>

namespace Nexus::Graphics
{
    GraphicsDeviceVk::GraphicsDeviceVk(const GraphicsDeviceCreateInfo &createInfo, Window *window)
        : GraphicsDevice(createInfo, window)
    {
        CreateInstance();
        // CreateDebug();
        SetupDebugMessenger();
        m_Swapchain = new SwapchainVk(window, createInfo.VSyncStateSettings, this);
        m_Swapchain->CreateSurface();
        SelectPhysicalDevice();
        SelectQueueFamilies();
        CreateDevice();
        CreateAllocator();

        m_Swapchain->CreateSwapchain();
        m_Swapchain->CreateSwapchainImageViews();
        m_Swapchain->CreateDepthStencil();
        CreateRenderPass();
        m_Swapchain->CreateFramebuffers();

        CreateCommandStructures();
        CreateSynchronisationStructures();
        CreateImGuiCommandStructures();
    }

    GraphicsDeviceVk::~GraphicsDeviceVk()
    {
        vkDestroyCommandPool(m_Device, m_ImGuiCommandPool, nullptr);
        delete m_Swapchain;

        // destroy upload context
        vkDestroyFence(m_Device, m_UploadContext.UploadFence, nullptr);
        vkFreeCommandBuffers(m_Device, m_UploadContext.CommandPool, 1, &m_UploadContext.CommandBuffer);
        vkDestroyCommandPool(m_Device, m_UploadContext.CommandPool, nullptr);

        // clean up frame data
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(m_Device, m_Frames[i].PresentSemaphore, nullptr);
            vkDestroySemaphore(m_Device, m_Frames[i].RenderSemaphore, nullptr);
            vkDestroyFence(m_Device, m_Frames[i].RenderFence, nullptr);
            vkFreeCommandBuffers(m_Device, m_Frames[i].CommandPool, 1, &m_Frames[i].MainCommandBuffer);
            vkDestroyCommandPool(m_Device, m_Frames[i].CommandPool, nullptr);
        }

        vkDestroyDevice(m_Device, nullptr);
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }

    void GraphicsDeviceVk::SetContext()
    {
    }

    void GraphicsDeviceVk::SubmitCommandList(Ref<CommandList> commandList)
    {
        VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        Ref<CommandListVk> vulkanCommandList = std::dynamic_pointer_cast<CommandListVk>(commandList);

        vkWaitForFences(m_Device, 1, &GetCurrentFrame().RenderFence, VK_TRUE, 0);
        vkResetFences(m_Device, 1, &GetCurrentFrame().RenderFence);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &GetCurrentFrame().PresentSemaphore;
        submitInfo.pWaitDstStageMask = &waitDestStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vulkanCommandList->GetCurrentCommandBuffer();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &GetCurrentFrame().PresentSemaphore;

        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, GetCurrentFrame().RenderFence) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit queue");
        }

        vkWaitForFences(m_Device, 1, &GetCurrentFrame().RenderFence, VK_TRUE, UINT32_MAX);
        vkResetFences(m_Device, 1, &GetCurrentFrame().RenderFence);
    }

    const std::string GraphicsDeviceVk::GetAPIName()
    {
        return "Vulkan";
    }

    const char *GraphicsDeviceVk::GetDeviceName()
    {
        return nullptr;
    }

    void *GraphicsDeviceVk::GetContext()
    {
        return nullptr;
    }

    void GraphicsDeviceVk::BeginFrame()
    {
        AcquireNextImage();
    }

    void GraphicsDeviceVk::EndFrame()
    {
    }

    Ref<Shader> GraphicsDeviceVk::CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout)
    {
        Nexus::Graphics::ShaderGenerator generator;

        Nexus::Graphics::ShaderGenerationOptions vertOptions;
        vertOptions.OutputFormat = ShaderLanguage::SPIRV;
        vertOptions.Type = Nexus::Graphics::ShaderType::Vertex;
        auto vertexResult = generator.Generate(vertexShaderSource, vertOptions);

        if (!vertexResult.Successful)
        {
            NX_ERROR(vertexResult.Error);
            throw std::runtime_error(vertexResult.Error);
        }

        Nexus::Graphics::ShaderGenerationOptions fragOptions;
        fragOptions.OutputFormat = ShaderLanguage::SPIRV;
        fragOptions.Type = Nexus::Graphics::ShaderType::Fragment;
        auto fragmentResult = generator.Generate(fragmentShaderSource, fragOptions);

        if (!fragmentResult.Successful)
        {
            NX_ERROR(fragmentResult.Error);
            throw std::runtime_error(fragmentResult.Error);
        }

        return CreateRef<ShaderVk>(vertexResult.SpirvBinary, fragmentResult.SpirvBinary, vertexShaderSource, fragmentShaderSource, layout, this);
    }

    Ref<Texture> GraphicsDeviceVk::CreateTexture(const TextureSpecification &spec)
    {
        return CreateRef<TextureVk>(this, spec);
    }

    Ref<Framebuffer> GraphicsDeviceVk::CreateFramebuffer(Ref<RenderPass> renderPass)
    {
        return CreateRef<FramebufferVk>(renderPass, this);
    }

    Ref<Pipeline> GraphicsDeviceVk::CreatePipeline(const PipelineDescription &description)
    {
        return CreateRef<PipelineVk>(description, this);
    }

    Ref<CommandList> GraphicsDeviceVk::CreateCommandList()
    {
        return CreateRef<CommandListVk>(this);
    }

    Ref<VertexBuffer> GraphicsDeviceVk::CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
    {
        return CreateRef<VertexBufferVk>(description, data, layout, this);
    }

    Ref<IndexBuffer> GraphicsDeviceVk::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
    {
        return CreateRef<IndexBufferVk>(description, data, this, format);
    }

    Ref<UniformBuffer> GraphicsDeviceVk::CreateUniformBuffer(const BufferDescription &description, const void *data)
    {
        return CreateRef<UniformBufferVk>(description, data, this);
    }

    Ref<RenderPass> GraphicsDeviceVk::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification)
    {
        return CreateRef<RenderPassVk>(renderPassSpecification, framebufferSpecification, this);
    }

    Ref<RenderPass> GraphicsDeviceVk::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        return CreateRef<RenderPassVk>(renderPassSpecification, swapchain, this);
    }

    Ref<ResourceSet> GraphicsDeviceVk::CreateResourceSet(const ResourceSetSpecification &spec)
    {
        return CreateRef<ResourceSetVk>(spec, this);
    }

    void GraphicsDeviceVk::Resize(Point<int> size)
    {
        vkDeviceWaitIdle(m_Device);

        m_Swapchain->CleanupSwapchain();
        m_Swapchain->CleanupDepthStencil();

        m_Swapchain->CreateSwapchain();
        m_Swapchain->CreateSwapchainImageViews();
        m_Swapchain->CreateDepthStencil();
        m_Swapchain->CreateFramebuffers();
    }

    ShaderLanguage GraphicsDeviceVk::GetSupportedShaderFormat()
    {
        return ShaderLanguage::SPIRV;
    }

    Swapchain *GraphicsDeviceVk::GetSwapchain()
    {
        return m_Swapchain;
    }

    uint32_t GraphicsDeviceVk::GetSwapchainImageCount()
    {
        return m_Swapchain->m_SwapchainImageCount;
    }

    VkDevice GraphicsDeviceVk::GetVkDevice()
    {
        return m_Device;
    }

    VkPhysicalDevice GraphicsDeviceVk::GetPhysicalDevice()
    {
        return m_PhysicalDevice;
    }

    uint32_t GraphicsDeviceVk::GetGraphicsFamily()
    {
        return m_GraphicsQueueFamilyIndex;
    }

    uint32_t GraphicsDeviceVk::GetPresentFamily()
    {
        return m_PresentQueueFamilyIndex;
    }

    uint32_t GraphicsDeviceVk::GetCurrentFrameIndex()
    {
        return m_FrameNumber % FRAMES_IN_FLIGHT;
    }

    VmaAllocator GraphicsDeviceVk::GetAllocator()
    {
        return m_Allocator;
    }

    void GraphicsDeviceVk::CreateImGuiCommandStructures()
    {
        {
            VkCommandPoolCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            createInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
            if (vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_ImGuiCommandPool) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create command pool");
            }
        }

        {
            VkCommandBufferAllocateInfo allocateInfo = {};
            allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocateInfo.commandPool = m_ImGuiCommandPool;
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.commandBufferCount = 1;

            if (vkAllocateCommandBuffers(m_Device, &allocateInfo, &m_ImGuiCommandBuffer) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to allocate command buffer");
            }
        }
    }

    void GraphicsDeviceVk::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function)
    {
        VkCommandBuffer cmd = m_UploadContext.CommandBuffer;
        VkCommandBufferBeginInfo cmdBeginInfo = {};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;
        cmdBeginInfo.pInheritanceInfo = nullptr;
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (vkBeginCommandBuffer(cmd, &cmdBeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin command buffer");
        }

        function(cmd);

        if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to end command buffer");
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        vkWaitForFences(m_Device, 1, &m_UploadContext.UploadFence, false, 0);
        vkResetFences(m_Device, 1, &m_UploadContext.UploadFence);

        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_UploadContext.UploadFence) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit queue");
        }

        vkWaitForFences(m_Device, 1, &m_UploadContext.UploadFence, true, UINT64_MAX);
        vkResetFences(m_Device, 1, &m_UploadContext.UploadFence);
        vkResetCommandPool(m_Device, m_UploadContext.CommandPool, 0);
    }

    const std::vector<const char *> validationLayers =
        {
            "VK_LAYER_KHRONOS_validation",
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    void GraphicsDeviceVk::CreateInstance()
    {
        if (enableValidationLayers && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers were requested, but were not available");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Nexus Application";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "Nexus";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        if (enableValidationLayers)
        {
            instanceCreateInfo.enabledLayerCount = validationLayers.size();
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            instanceCreateInfo.enabledLayerCount = 0;
        }

        auto extensions = GetRequiredExtensions();
        instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance!");
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

    void GraphicsDeviceVk::SetupDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to setup debug messenger");
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

    void GraphicsDeviceVk::SelectQueueFamilies()
    {
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        uint32_t queueFamilyCount;

        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
        queueFamilyProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        int graphicsIndex = -1;
        int presentIndex = -1;

        int i = 0;
        for (const auto &queueFamily : queueFamilyProperties)
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsIndex = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Swapchain->m_Surface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport)
            {
                presentIndex = i;
            }

            if (graphicsIndex != -1 && presentIndex != -1)
            {
                break;
            }
            i++;
        }

        if (graphicsIndex == -1 || presentIndex == -1)
        {
            throw std::runtime_error("Failed to find a graphics or present queue");
        }

        m_GraphicsQueueFamilyIndex = graphicsIndex;
        m_PresentQueueFamilyIndex = presentIndex;
    }

    void GraphicsDeviceVk::CreateDevice()
    {
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        const float queuePriority[] = {1.0f};

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex};

        float priority = queuePriority[0];
        for (int queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &priority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;

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

    void GraphicsDeviceVk::CreateAllocator()
    {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = m_Instance;

        if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create VMA allocator");
        }
    }

    void GraphicsDeviceVk::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_Swapchain->m_SurfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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
        depthAttachment.format = m_Swapchain->m_DepthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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

        if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_SwapchainRenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
    }

    void GraphicsDeviceVk::CreateCommandStructures()
    {
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            // create command pools
            {
                VkCommandPoolCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
                createInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
                if (vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_Frames[i].CommandPool) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to create command pool");
                }
            }

            // allocate command buffers
            {
                VkCommandBufferAllocateInfo allocateInfo = {};
                allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocateInfo.commandPool = m_Frames[i].CommandPool;
                allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocateInfo.commandBufferCount = m_Swapchain->m_SwapchainImageCount;

                if (vkAllocateCommandBuffers(m_Device, &allocateInfo, &m_Frames[i].MainCommandBuffer) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to allocate command buffers");
                }
            }

            // upload command pool
            {
                VkCommandPoolCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
                createInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
                if (vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_UploadContext.CommandPool) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to create command pool");
                }
            }

            // upload command buffer
            {
                VkCommandBufferAllocateInfo allocateInfo = {};
                allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocateInfo.commandPool = m_UploadContext.CommandPool;
                allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocateInfo.commandBufferCount = m_Swapchain->m_SwapchainImageCount;

                if (vkAllocateCommandBuffers(m_Device, &allocateInfo, &m_UploadContext.CommandBuffer) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to allocate command buffers");
                }
            }
        }
    }

    void GraphicsDeviceVk::CreateSynchronisationStructures()
    {
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            // create fence
            if (vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_Frames[i].RenderFence) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create fence");
            }

            if (vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Frames[i].PresentSemaphore) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create semaphore");
            }

            if (vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Frames[i].RenderSemaphore) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create semaphore");
            }
        }

        if (vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_UploadContext.UploadFence) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create fence");
        }
    }

    VkImage image;
    bool GraphicsDeviceVk::AcquireNextImage()
    {
        VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain->m_Swapchain, 0, GetCurrentFrame().PresentSemaphore, VK_NULL_HANDLE, &m_CurrentFrameIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchain();
            return false;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swapchain image");
        }

        vkWaitForFences(m_Device, 1, &GetCurrentFrame().RenderFence, VK_FALSE, 0);
        vkResetFences(m_Device, 1, &GetCurrentFrame().RenderFence);

        m_CurrentCommandBuffer = GetCurrentFrame().MainCommandBuffer;
        image = m_Swapchain->m_SwapchainImages[m_CurrentFrameIndex];

        return true;
    }

    void GraphicsDeviceVk::ResetCommandBuffer()
    {
        vkResetCommandBuffer(m_CurrentCommandBuffer, 0);
    }

    void GraphicsDeviceVk::BeginCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        if (vkBeginCommandBuffer(m_CurrentCommandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin command buffer");
        }
    }

    void GraphicsDeviceVk::EndCommandBuffer()
    {
        vkEndCommandBuffer(m_CurrentCommandBuffer);
    }

    VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    void GraphicsDeviceVk::QueueSubmit()
    {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &GetCurrentFrame().PresentSemaphore;
        submitInfo.pWaitDstStageMask = &waitDestStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CurrentCommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &GetCurrentFrame().PresentSemaphore;
        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, GetCurrentFrame().RenderFence) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit queue");
        }
    }

    void GraphicsDeviceVk::QueuePresent()
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &GetCurrentFrame().PresentSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain->m_Swapchain;
        presentInfo.pImageIndices = &m_CurrentFrameIndex;

        VkResult result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapchain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swapchain image");
        }

        if (vkQueueWaitIdle(m_PresentQueue) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to wait for present queue");
        }

        m_FrameNumber++;
    }

    void GraphicsDeviceVk::BeginRenderPass(VkClearColorValue clear_color, VkClearDepthStencilValue clear_depth_stencil)
    {
        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = m_SwapchainRenderPass;
        render_pass_info.framebuffer = m_Swapchain->m_SwapchainFramebuffers[m_CurrentFrameIndex];
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = m_Swapchain->m_SwapchainSize;
        render_pass_info.clearValueCount = 1;

        std::vector<VkClearValue> clearValues(2);
        clearValues[0].color = clear_color;
        clearValues[1].depthStencil = clear_depth_stencil;

        render_pass_info.clearValueCount = (uint32_t)clearValues.size();
        render_pass_info.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CurrentCommandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    void GraphicsDeviceVk::EndRenderPass()
    {
        vkCmdEndRenderPass(m_CurrentCommandBuffer);
    }

    void GraphicsDeviceVk::BeginImGuiRenderPass()
    {
        // reset command buffer
        vkResetCommandBuffer(m_ImGuiCommandBuffer, 0);

        // begin command buffer
        {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            if (vkBeginCommandBuffer(m_ImGuiCommandBuffer, &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to begin ImGui command buffer");
            }
        }

        // begin render pass
        {
            VkRenderPassBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            beginInfo.renderPass = m_SwapchainRenderPass;
            beginInfo.framebuffer = m_Swapchain->GetCurrentFramebuffer();
            beginInfo.renderArea.offset = {0, 0};
            beginInfo.renderArea.extent = m_Swapchain->m_SwapchainSize;
            beginInfo.clearValueCount = 1;

            beginInfo.clearValueCount = 0;
            beginInfo.pClearValues = nullptr;

            vkCmdBeginRenderPass(m_ImGuiCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
        }
    }

    void GraphicsDeviceVk::EndImGuiRenderPass()
    {
        vkCmdEndRenderPass(m_ImGuiCommandBuffer);
        vkEndCommandBuffer(m_ImGuiCommandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &GetCurrentFrame().PresentSemaphore;
        submitInfo.pWaitDstStageMask = &waitDestStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_ImGuiCommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &GetCurrentFrame().PresentSemaphore;

        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, GetCurrentFrame().RenderFence) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit queue");
        }
    }

    void GraphicsDeviceVk::RecreateSwapchain()
    {
        vkDeviceWaitIdle(m_Device);

        m_Swapchain->CleanupSwapchain();
        m_Swapchain->CleanupDepthStencil();

        m_Swapchain->CreateSwapchain();
        m_Swapchain->CreateSwapchainImageViews();
        m_Swapchain->CreateDepthStencil();
        m_Swapchain->CreateFramebuffers();
    }

    void GraphicsDeviceVk::CleanupSwapchain()
    {
        for (size_t i = 0; i < m_Swapchain->m_SwapchainFramebuffers.size(); i++)
        {
            vkDestroyFramebuffer(m_Device, m_Swapchain->m_SwapchainFramebuffers[i], nullptr);
        }

        for (size_t i = 0; i < m_Swapchain->m_SwapchainImageViews.size(); i++)
        {
            vkDestroyImageView(m_Device, m_Swapchain->m_SwapchainImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(m_Device, m_Swapchain->m_Swapchain, nullptr);
    }

    void GraphicsDeviceVk::CleanupDepthStencil()
    {
        vkDestroyImageView(m_Device, m_Swapchain->m_DepthImageView, nullptr);
        vkDestroyImage(m_Device, m_Swapchain->m_DepthImage, nullptr);
        vkFreeMemory(m_Device, m_Swapchain->m_DepthImageMemory, nullptr);
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

    bool GraphicsDeviceVk::CheckValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : validationLayers)
        {
            bool layerFound = false;
            for (const auto &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    std::vector<const char *> GraphicsDeviceVk::GetRequiredExtensions()
    {
        uint32_t sdlExtensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(m_Window->GetSDLWindowHandle(), &sdlExtensionCount, nullptr);
        std::vector<const char *> extensionNames(sdlExtensionCount);
        SDL_Vulkan_GetInstanceExtensions(m_Window->GetSDLWindowHandle(), &sdlExtensionCount, extensionNames.data());

        if (enableValidationLayers)
        {
            extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensionNames;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL GraphicsDeviceVk::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    VkResult GraphicsDeviceVk::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void GraphicsDeviceVk::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, messenger, pAllocator);
        }
    }

    AllocatedBuffer GraphicsDeviceVk::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.size = allocSize;
        bufferInfo.usage = usage;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = memoryUsage;

        AllocatedBuffer buffer;

        if (vmaCreateBuffer(m_Allocator, &bufferInfo, &vmaAllocInfo, &buffer.Buffer, &buffer.Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create buffer");
        }

        return buffer;
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

    FrameData &GraphicsDeviceVk::GetCurrentFrame()
    {
        return m_Frames[m_FrameNumber % FRAMES_IN_FLIGHT];
    }
}

#endif