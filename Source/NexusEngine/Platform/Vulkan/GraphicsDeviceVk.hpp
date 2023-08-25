#pragma once

#include "Core/Graphics/GraphicsDevice.hpp"
#include "SwapchainVk.hpp"
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
        virtual Ref<Texture> CreateTexture(const TextureSpecification &spec) override;
        virtual Ref<Framebuffer> CreateFramebuffer(Ref<RenderPass> renderPass) override;
        virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) override;
        virtual Ref<CommandList> CreateCommandList();

        virtual Ref<VertexBuffer> CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const BufferDescription &description, const void *data) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) override;

        virtual Ref<RenderPass> CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification) override;
        virtual Ref<RenderPass> CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain) override;

        virtual void Resize(Point<int> size) override;
        virtual ShaderLanguage GetSupportedShaderFormat() override;
        virtual float GetUVCorrection() { return 1.0f; }

        virtual Swapchain *GetSwapchain() override;
        uint32_t GetSwapchainImageCount();

        VkDevice GetVkDevice();
        uint32_t GetGraphicsFamily();
        uint32_t GetPresentFamily();
        uint32_t GetCurrentFrameIndex();
        VmaAllocator GetAllocator();

        void CreateImGuiCommandStructures();

        void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        // vulkan functions
    private:
        void CreateInstance();
        void CreateDebug();
        void SetupDebugMessenger();
        void SelectPhysicalDevice();
        void SelectQueueFamilies();
        void CreateDevice();
        void CreateAllocator();

        void CreateRenderPass();

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

        void BeginImGuiRenderPass();
        void EndImGuiRenderPass();

        void RecreateSwapchain();
        void CleanupSwapchain();
        void CleanupDepthStencil();

    private:
        // utility functions
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);
        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

        bool CheckValidationLayerSupport();
        std::vector<const char *> GetRequiredExtensions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

    private:
        // Vulkan types
        VkInstance m_Instance;
        VkDebugReportCallbackEXT m_DebugCallback;
        VkPhysicalDevice m_PhysicalDevice;
        VkDebugUtilsMessengerEXT m_DebugMessenger;

        uint32_t m_GraphicsQueueFamilyIndex;
        uint32_t m_PresentQueueFamilyIndex;

        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

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

        SwapchainVk *m_Swapchain;
        VSyncState m_VsyncState = VSyncState::Enabled;

        VkCommandPool m_ImGuiCommandPool;
        VkCommandBuffer m_ImGuiCommandBuffer;

        friend class SwapchainVk;
        friend class ImGuiRenderer;
    };
}