#pragma once

#include "Core/Graphics/GraphicsDevice.hpp"
#include "Vk.hpp"

const uint32_t FRAMES_IN_FLIGHT = 3;

namespace Nexus::Graphics
{
    struct FrameData
    {
        VkSemaphore PresentSemaphore, RenderSemaphore;
        VkFence RenderFence;
        VkCommandPool CommandPool;
        VkCommandBuffer MainCommandBuffer;
    };

    struct UploadContext
    {
        VkFence UploadFence;
        VkCommandPool CommandPool;
        VkCommandBuffer CommandBuffer;
    };

    struct VulkanSwapchain
    {
        VkSwapchainKHR Swapchain;
        VkSurfaceCapabilitiesKHR SurfaceCapabilities;
        VkSurfaceFormatKHR SurfaceFormat;
        VkExtent2D SwapchainSize;

        std::vector<VkImage> SwapchainImages;
        uint32_t SwapchainImageCount;
        std::vector<VkImageView> SwapchainImageViews;

        VkFormat DepthFormat;
        VkImage DepthImage;
        VkDeviceMemory DepthImageMemory;
        VkImageView DepthImageView;

        std::vector<VkFramebuffer> SwapchainFramebuffers;
    };

    class GraphicsDeviceVk : public GraphicsDevice
    {
    public:
        GraphicsDeviceVk(const GraphicsDeviceCreateInfo &createInfo, Window *window);
        GraphicsDeviceVk(const GraphicsDeviceVk &) = delete;
        virtual ~GraphicsDeviceVk();
        void SetContext() override;
        virtual void SubmitCommandList(Ref<CommandList> commandList) override;

        virtual void SetViewport(const Viewport &viewport) override;

        virtual const std::string GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        virtual Ref<Shader> CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Ref<Texture> CreateTexture(TextureSpecification spec) override;
        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) override;
        virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) override;
        virtual Ref<CommandList> CreateCommandList();

        virtual Ref<VertexBuffer> CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const BufferDescription &description, const void *data) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) override;
        virtual Ref<RenderPass> CreateRenderPass(const RenderPassSpecification &renderPassSpecification) override;

        virtual void Resize(Point<int> size) override;
        virtual ShaderLanguage GetSupportedShaderFormat() override;
        virtual float GetUVCorrection() { return 1.0f; }

        virtual Swapchain *GetSwapchain() override;

        const VulkanSwapchain &GetVulkanSwapchain();
        VkDevice GetVkDevice();
        uint32_t GetGraphicsFamily();
        uint32_t GetPresentFamily();
        uint32_t GetCurrentFrameIndex();

        // vulkan functions
    private:
        void CreateInstance();
        void CreateDebug();
        void CreateSurface();
        void SelectPhysicalDevice();
        void SelectQueueFamilies();
        void CreateDevice();
        void CreateAllocator();

        void CreateSwapchain();
        void CreateSwapchainImageViews();
        void CreateDepthStencil();
        void CreateRenderPass();
        void CreateFramebuffers();

        void CreateCommandStructures();
        void CreateSynchronisationStructures();

        bool AcquireNextImage();
        void ResetCommandBuffer();
        void BeginCommandBuffer();
        void EndCommandBuffer();
        void QueueSubmit();
        void QueuePresent();

        void BeginRenderPass(VkClearColorValue clear_color, VkClearDepthStencilValue clear_depth_stencil);
        void EndRenderPass();

        void RecreateSwapchain();
        void CleanupSwapchain();
        void CleanupDepthStencil();

        // utility functions
    private:
        VkImageView
        CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);
        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    private:
        // Vulkan types
        VkInstance m_Instance;
        VkDebugReportCallbackEXT m_DebugCallback;
        VkSurfaceKHR m_Surface;
        VkPhysicalDevice m_PhysicalDevice;

        uint32_t m_GraphicsQueueFamilyIndex;
        uint32_t m_PresentQueueFamilyIndex;

        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        VulkanSwapchain m_Swapchain;

        // render pass
        VkRenderPass m_SwapchainRenderPass;

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

        VSyncState m_VsyncState = VSyncState::Enabled;
    };
}