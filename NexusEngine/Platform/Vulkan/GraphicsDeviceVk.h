#pragma once

#include "Core/Graphics/GraphicsDevice.h"

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "glm/glm.hpp"

#include <array>

struct alignas(16) UniformBufferObject
{
    glm::mat4 MVP;
};

namespace Nexus
{
    struct AllocatedBuffer
    {
        VkBuffer Buffer;
        VmaAllocation Allocation;
    };

    struct VulkanVertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(VulkanVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
        {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(VulkanVertex, color);

            return attributeDescriptions;
        }
    };

    const std::vector<VulkanVertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    struct FrameData
    {
        VkSemaphore PresentSemaphore, RenderSemaphore;
        VkFence RenderFence;

        VkCommandPool CommandPool;
        VkCommandBuffer MainCommandBuffer;
    };

    const uint32_t FRAMES_IN_FLIGHT = 2;

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

        virtual ShaderFormat GetSupportedShaderFormat() override { return ShaderFormat::SPIRV; }

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
        void DrawWithPipeline();

    private:
        void CreateInstance();
        void CreateDebug();
        void CreateSurface();
        void SelectPhysicalDevice();
        void SelectQueueFamily();
        void CreateDevice();
        void CreateAllocator();

        void CreateSwapchain();
        void CreateSwapchainImageViews();
        void SetupDepthStencil();
        void CreateRenderPass();
        void CreateFramebuffers();

        void CreateCommandStructures();
        void CreateSynchonisationStructures();

        void InitDescriptors();
        void InitPipelines();
        void InitBuffers();

    private:
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
        void CreateSemaphore(VkSemaphore *semaphore);
        VkShaderModule CreateShaderModule(const std::vector<uint32_t> &spirv_buffer, bool *successful);
        AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

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

        // synchronisation
        FrameData m_Frames[FRAMES_IN_FLIGHT];
        FrameData &GetCurrentFrame();

        // pipeline
        VkShaderModule m_VertexShader;
        VkShaderModule m_FragmentShader;
        VkPipelineLayout m_TrianglePipelineLayout;
        VkPipeline m_TrianglePipeline;

        // Vertex buffer
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;

        // VMA
        VmaAllocator m_Allocator;
        std::vector<AllocatedBuffer> m_UniformBuffers;

        VkDescriptorSetLayout m_GlobalSetLayout;
        VkDescriptorPool m_DescriptorPool;

        uint32_t m_FrameNumber = 0;
    };
}