#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "SwapchainVk.hpp"
#include "Vk.hpp"

#include "CommandExecutorVk.hpp"

namespace Nexus::Graphics
{
    struct FrameData
    {
        VkCommandPool CommandPool;
        VkCommandBuffer MainCommandBuffer;
    };

    struct UploadContext
    {
        VkFence UploadFence;
        VkCommandPool CommandPool;
        VkCommandBuffer CommandBuffer;
    };

    class GraphicsDeviceVk : public GraphicsDevice
    {
    public:
        GraphicsDeviceVk(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec);
        GraphicsDeviceVk(const GraphicsDeviceVk &) = delete;
        virtual ~GraphicsDeviceVk();

        void SetContext() override;
        virtual void SubmitCommandList(Ref<CommandList> commandList) override;

        virtual const std::string GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        virtual Ref<Texture> CreateTexture(const TextureSpecification &spec) override;
        virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) override;
        virtual Ref<CommandList> CreateCommandList();
        virtual Ref<VertexBuffer> CreateVertexBuffer(const BufferDescription &description, const void *data) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format = IndexBufferFormat::UInt32) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) override;
        virtual Ref<ResourceSet> CreateResourceSet(const ResourceSetSpecification &spec) override;
        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) override;
        virtual Ref<Sampler> CreateSampler(const SamplerSpecification &spec) override;
        virtual Ref<TimingQuery> CreateTimingQuery() override;

        virtual const GraphicsCapabilities GetGraphicsCapabilities() const override;

        virtual ShaderLanguage GetSupportedShaderFormat() override;
        virtual float GetUVCorrection() { return -1.0f; }
        virtual bool IsUVOriginTopLeft() override { return true; };

        VkDevice GetVkDevice();
        VkPhysicalDevice GetPhysicalDevice();
        uint32_t GetSwapchainImageCount();
        uint32_t GetGraphicsFamily();
        uint32_t GetPresentFamily();
        uint32_t GetCurrentFrameIndex();
        VmaAllocator GetAllocator();
        const VkPhysicalDeviceProperties &GetDeviceProperties();

        void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function);
        void TransitionVulkanImageLayout(VkImage image, uint32_t level, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlagBits aspectMask);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

        // vulkan functions
    private:
        virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) override;

        void CreateInstance();
        void SetupDebugMessenger();
        void SelectPhysicalDevice();
        void SelectQueueFamilies();
        void CreateDevice();
        void CreateAllocator();

        void CreateCommandStructures();
        void CreateSynchronisationStructures();

    private:
        // utility functions
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);
        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

        bool CheckValidationLayerSupport();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator);

        std::vector<const char *> GetRequiredInstanceExtensions();
        std::vector<std::string> GetSupportedInstanceExtensions();

        std::vector<const char *> GetRequiredDeviceExtensions();
        std::vector<std::string> GetSupportedDeviceExtensions();

    private:
        // Vulkan types
        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;
        VkDebugUtilsMessengerEXT m_DebugMessenger;

        uint32_t m_GraphicsQueueFamilyIndex;
        uint32_t m_PresentQueueFamilyIndex;

        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        // synchronisation
        FrameData m_Frames[FRAMES_IN_FLIGHT];
        FrameData &GetCurrentFrame();
        VkCommandBuffer m_CurrentCommandBuffer;

        // VMA types
        VmaAllocator m_Allocator;

        // vulkan upload context
        UploadContext m_UploadContext;

        uint32_t m_FrameNumber = 0;
        uint32_t m_CurrentFrameIndex = 0;

        VkPhysicalDeviceProperties m_DeviceProperties;

        VSyncState m_VsyncState = VSyncState::Enabled;

        CommandExecutorVk m_CommandExecutor;

        friend class SwapchainVk;
    };
}

#endif