#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "vulkan/vulkan.h"

namespace Nexus
{
    class GraphicsDeviceVk : public GraphicsDevice
    {
    public:
        GraphicsDeviceVk(const GraphicsDeviceCreateInfo &createInfo);
        GraphicsDeviceVk(const GraphicsDeviceVk &) = delete;
        virtual ~GraphicsDeviceVk();
        void SetContext() override;
        void Clear(float red, float green, float blue, float alpha) override;
        virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) override;
        void DrawElements(PrimitiveType type, uint32_t start, uint32_t count) override;
        void DrawIndexed(PrimitiveType type, uint32_t count, uint32_t offset) override;

        virtual void SetViewport(const Viewport &viewport) override;
        virtual const Viewport &GetViewport() override;

        virtual const char *GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetShader(Ref<Shader> shader) override;

        virtual Ref<Shader> CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<Vertex> vertices) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const UniformResourceBinding &binding) override;
        virtual Ref<Texture> CreateTexture(TextureSpecification spec) override;
        virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification &spec) override;

        virtual void InitialiseImGui() override;
        virtual void BeginImGuiRender() override;
        virtual void EndImGuiRender() override;

        virtual void Resize(Point<int> size) override;
        virtual void SwapBuffers() override;
        virtual void SetVSyncState(VSyncState vSyncState) override;
        virtual VSyncState GetVsyncState() override;

        virtual ShaderFormat GetSupportedShaderFormat() override { return ShaderFormat::GLSL; }

    public:
        void AcquireNextImage();
        void ResetCommandBuffer();
        void BeginCommandBuffer();
        void EndCommandBuffer();
        void FreeCommandBuffers();
        void BeginRenderPass(VkClearColorValue clear_color, VkClearDepthStencilValue clear_depth_stencil);
        void EndRenderPass();
        void QueueSubmit();
        void QueuePresent();
        void SetViewport(int width, int height);
        void SetScissor(int width, int height);

    private:
        void CreateInstance();
        void CreateDebug();
        void CreateSurface();
        void SelectPhysicalDevice();
        void SelectQueueFamily();
        void CreateDevice();

        void CreateSwapchain();
        void CreateSwapchainImageViews();
        void SetupDepthStencil();
        void CreateRenderPass();
        void CreateFramebuffers();

        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateSemaphores();
        void CreateFences();

    private:
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
        void CreateSemaphore(VkSemaphore *semaphore);

    private:
        VkInstance m_Instance;
        VkDebugReportCallbackEXT m_DebugCallback;
        VkSurfaceKHR m_Surface;
        VkPhysicalDevice m_PhysicalDevice;

        uint32_t m_GraphicsQueueFamilyIndex;
        uint32_t m_PresentQueueFamilyIndex;

        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        VkSwapchainKHR m_Swapchain;
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
        VkSurfaceFormatKHR m_SurfaceFormat;
        VkExtent2D m_SwapchainSize;

        std::vector<VkImage> m_SwapchainImages;
        uint32_t m_SwapchainImageCount;

        std::vector<VkImageView> m_SwapchainImageViews;

        // depth buffer
        VkFormat m_DepthFormat;
        VkImage m_DepthImage;
        VkDeviceMemory m_DepthImageMemory;
        VkImageView m_DepthImageView;

        // render pass
        VkRenderPass m_RenderPass;

        // framebuffer
        std::vector<VkFramebuffer> m_SwapchainFramebuffers;

        // rendering
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        // synchronisation
        VkSemaphore m_ImageAvailableSemaphore;
        VkSemaphore m_RenderingFinishedSemaphore;
        std::vector<VkFence> m_Fences;
    };
}