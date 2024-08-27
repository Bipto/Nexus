#if defined(NX_PLATFORM_VULKAN)

#include "Nexus-Core/nxpch.hpp"

#include "BufferVk.hpp"
#include "CommandListVk.hpp"
#include "FramebufferVk.hpp"
#include "GraphicsDeviceVk.hpp"
#include "PipelineVk.hpp"
#include "ResourceSetVk.hpp"
#include "SamplerVk.hpp"
#include "ShaderModuleVk.hpp"
#include "TextureVk.hpp"
#include "TimingQueryVk.hpp"

#include "SDL_vulkan.h"

namespace Nexus::Graphics
{
GraphicsDeviceVk::GraphicsDeviceVk(const GraphicsDeviceSpecification &createInfo, Window *window, const SwapchainSpecification &swapchainSpec)
    : GraphicsDevice(createInfo, window, swapchainSpec), m_CommandExecutor(this)
{
    CreateInstance();

#if defined(_DEBUG)
    SetupDebugMessenger();
#endif

    window->CreateSwapchain(this, swapchainSpec);

    SelectPhysicalDevice();
    SelectQueueFamilies();
    CreateDevice();
    auto deviceExtensions = GetSupportedDeviceExtensions();
    CreateAllocator();

    SwapchainVk *swapchain = (SwapchainVk *)window->GetSwapchain();
    swapchain->Initialise();

    CreateCommandStructures();
    CreateSynchronisationStructures();

    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_DeviceProperties);
}

GraphicsDeviceVk::~GraphicsDeviceVk()
{
    vkDestroyFence(m_Device, m_UploadContext.UploadFence, nullptr);
    vkFreeCommandBuffers(m_Device, m_UploadContext.CommandPool, 1, &m_UploadContext.CommandBuffer);
    vkDestroyCommandPool(m_Device, m_UploadContext.CommandPool, nullptr);

    for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        vkFreeCommandBuffers(m_Device, m_Frames[i].CommandPool, 1, &m_Frames[i].MainCommandBuffer);
        vkDestroyCommandPool(m_Device, m_Frames[i].CommandPool, nullptr);
    }
}

void GraphicsDeviceVk::SubmitCommandList(Ref<CommandList> commandList)
{
    VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    Ref<CommandListVk> commandListVk = std::dynamic_pointer_cast<CommandListVk>(commandList);

    vkWaitForFences(m_Device, 1, &commandListVk->GetCurrentFence(), VK_TRUE, 0);
    vkResetFences(m_Device, 1, &commandListVk->GetCurrentFence());

    const std::vector<Nexus::Graphics::RenderCommandData> &commands = commandListVk->GetCommandData();
    m_CommandExecutor.SetCommandListSpecification(commandList->GetSpecification());
    m_CommandExecutor.SetCommandBuffer(commandListVk->GetCurrentCommandBuffer());
    m_CommandExecutor.ExecuteCommands(commands, this);
    m_CommandExecutor.Reset();

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = &waitDestStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandListVk->GetCurrentCommandBuffer();
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, commandListVk->GetCurrentFence()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit queue");
    }

    vkWaitForFences(m_Device, 1, &commandListVk->GetCurrentFence(), VK_TRUE, UINT32_MAX);
    vkResetFences(m_Device, 1, &commandListVk->GetCurrentFence());
}

const std::string GraphicsDeviceVk::GetAPIName()
{
    return "Vulkan";
}

const char *GraphicsDeviceVk::GetDeviceName()
{
    return nullptr;
}

Ref<ShaderModule> GraphicsDeviceVk::CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources)
{
    return CreateRef<ShaderModuleVk>(moduleSpec, resources, this);
}

Ref<Texture> GraphicsDeviceVk::CreateTexture(const TextureSpecification &spec)
{
    return CreateRef<TextureVk>(this, spec);
}

Ref<Pipeline> GraphicsDeviceVk::CreatePipeline(const PipelineDescription &description)
{
    return CreateRef<PipelineVk>(description, this);
}

Ref<CommandList> GraphicsDeviceVk::CreateCommandList(const CommandListSpecification &spec)
{
    return CreateRef<CommandListVk>(this, spec);
}

Ref<VertexBuffer> GraphicsDeviceVk::CreateVertexBuffer(const BufferDescription &description, const void *data)
{
    return CreateRef<VertexBufferVk>(description, data, this);
}

Ref<IndexBuffer> GraphicsDeviceVk::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
{
    return CreateRef<IndexBufferVk>(description, data, this, format);
}

Ref<UniformBuffer> GraphicsDeviceVk::CreateUniformBuffer(const BufferDescription &description, const void *data)
{
    return CreateRef<UniformBufferVk>(description, data, this);
}

Ref<ResourceSet> GraphicsDeviceVk::CreateResourceSet(const ResourceSetSpecification &spec)
{
    return CreateRef<ResourceSetVk>(spec, this);
}

Ref<Framebuffer> GraphicsDeviceVk::CreateFramebuffer(const FramebufferSpecification &spec)
{
    return CreateRef<FramebufferVk>(spec, this);
}

Ref<Sampler> GraphicsDeviceVk::CreateSampler(const SamplerSpecification &spec)
{
    return CreateRef<SamplerVk>(this, spec);
}

Ref<TimingQuery> GraphicsDeviceVk::CreateTimingQuery()
{
    return CreateRef<TimingQueryVk>(this);
}

const GraphicsCapabilities GraphicsDeviceVk::GetGraphicsCapabilities() const
{
    GraphicsCapabilities capabilities;
    capabilities.SupportsMultisampledTextures = true;
    capabilities.SupportsLODBias = true;
    capabilities.SupportsInstanceOffset = true;
    capabilities.SupportsMultipleSwapchains = true;
    return capabilities;
}

ShaderLanguage GraphicsDeviceVk::GetSupportedShaderFormat()
{
    return ShaderLanguage::SPIRV;
}

uint32_t GraphicsDeviceVk::GetSwapchainImageCount()
{
    SwapchainVk *swapchain = (SwapchainVk *)m_Window->GetSwapchain();
    return swapchain->GetImageCount();
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

const VkPhysicalDeviceProperties &GraphicsDeviceVk::GetDeviceProperties()
{
    return m_DeviceProperties;
}

void GraphicsDeviceVk::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function)
{
    vkResetFences(m_Device, 1, &m_UploadContext.UploadFence);
    vkResetCommandPool(m_Device, m_UploadContext.CommandPool, 0);

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

    if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_UploadContext.UploadFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit queue");
    }

    vkDeviceWaitIdle(m_Device);
    vkQueueWaitIdle(m_GraphicsQueue);
}

void GraphicsDeviceVk::TransitionVulkanImageLayout(VkCommandBuffer cmdBuffer, VkImage image, uint32_t level, VkImageLayout oldLayout, VkImageLayout newLayout,
                                                   VkImageAspectFlagBits aspectMask)
{
    if (oldLayout == newLayout)
    {
        return;
    }

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspectMask;
    barrier.subresourceRange.baseMipLevel = level;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &barrier);
}

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

#if defined(_DEBUG)
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
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
    appInfo.apiVersion = VK_API_VERSION_1_3;

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

    auto extensions = GetRequiredInstanceExtensions();
    instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanReportFunc(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code,
                                                       const char *layerPrefix, const char *msg, void *userData)
{
    std::cout << "VULKAN VALIDATION: " << msg << std::endl;
    return VK_FALSE;
}

void GraphicsDeviceVk::SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
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
        SwapchainVk *swapchain = (SwapchainVk *)m_Window->GetSwapchain();

        vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, swapchain->m_Surface, &presentSupport);
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
    std::vector<const char *> deviceExtensions = GetRequiredDeviceExtensions();
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
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
    dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamicRenderingFeatures.pNext = nullptr;
    dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &dynamicRenderingFeatures;
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

void GraphicsDeviceVk::CreateCommandStructures()
{
    SwapchainVk *swapchain = (SwapchainVk *)m_Window->GetSwapchain();

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
            allocateInfo.commandBufferCount = swapchain->m_SwapchainImageCount;

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
            allocateInfo.commandBufferCount = swapchain->m_SwapchainImageCount;

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
    semaphoreCreateInfo.flags = 0;

    if (vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_UploadContext.UploadFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create fence");
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
    std::vector<VkFormat> depthFormats = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM};

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

void GraphicsDeviceVk::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                                   VkImage &image, VkDeviceMemory &imageMemory)
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

std::vector<const char *> GraphicsDeviceVk::GetRequiredInstanceExtensions()
{
    uint32_t sdlExtensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    std::vector<const char *> extensionNames(sdlExtensionCount);
    const char *const *names = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

    for (uint32_t i = 0; i < sdlExtensionCount; i++)
    {
        extensionNames[i] = names[i];
    }

    if (enableValidationLayers)
    {
        extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensionNames;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GraphicsDeviceVk::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult GraphicsDeviceVk::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                                        VkDebugUtilsMessengerEXT *pDebugMessenger)
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

std::vector<std::string> GraphicsDeviceVk::GetSupportedInstanceExtensions()
{
    uint32_t count = 0;
    VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> properties(count);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to get supported instance extensions");
    }

    std::vector<std::string> extensions;
    for (const auto &property : properties)
    {
        extensions.push_back(property.extensionName);
    }

    return extensions;
}

std::vector<const char *> GraphicsDeviceVk::GetRequiredDeviceExtensions()
{
    std::vector<const char *> extensions;
    extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    extensions.push_back(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME);
    extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
    return extensions;
}

std::vector<std::string> GraphicsDeviceVk::GetSupportedDeviceExtensions()
{
    uint32_t count = 0;
    VkResult result = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> properties(count);
    result = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, properties.data());

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to get supported device extensions");
    }

    std::vector<std::string> extensions;
    for (const auto &property : properties)
    {
        extensions.push_back(property.extensionName);
    }

    return extensions;
}

Vk::AllocatedBuffer GraphicsDeviceVk::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = allocSize;
    bufferInfo.usage = usage;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = memoryUsage;
    vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    vmaAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

    Vk::AllocatedBuffer buffer;

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
} // namespace Nexus::Graphics

#endif