#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "CommandExecutorVk.hpp"
#include "DeviceBufferVk.hpp"
#include "PhysicalDeviceVk.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "SwapchainVk.hpp"
#include "Vk.hpp"

namespace Nexus::Graphics
{
    struct VulkanDeviceConfig
    {
        bool Debug = false;
        bool UseDynamicRenderingIfAvailable = false;
    };

    struct VulkanDeviceFeatures
    {
        bool DynamicRenderingAvailable = false;
        bool Supports8BitIndices = false;
        bool SupportsRayTracing = false;
    };

    class GraphicsDeviceVk final : public IGraphicsDevice
    {
      public:
        GraphicsDeviceVk(
            std::shared_ptr<IPhysicalDevice> physicalDevice, VkInstance instance,
            const VulkanDeviceConfig &config
        );
        GraphicsDeviceVk(const GraphicsDeviceVk &) = delete;
        virtual ~GraphicsDeviceVk();

        std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const final;

        PipelineHandle CreateGraphicsPipeline(
            const GraphicsPipelineDescription &description
        ) final;
        PipelineHandle CreateComputePipeline(
            const ComputePipelineDescription &description
        ) final;
        PipelineHandle CreateMeshletPipeline(
            const MeshletPipelineDescription &description
        ) final;
        PipelineHandle CreateRayTracingPipeline(
            const RayTracingPipelineDescription &description
        ) final;

        ResourceSetHandle CreateResourceSet(PipelineHandle pipeline) final;
        FramebufferHandle CreateFramebuffer(
            const FramebufferTextureSetDescription &desc
        ) final;
        SamplerHandle CreateSampler(const SamplerDescription &spec) final;
        TimingQueryHandle CreateTimingQuery() final;
        DeviceBufferHandle CreateDeviceBuffer(
            const DeviceBufferDescription &desc
        ) final;
        AccelerationStructureHandle CreateAccelerationStructure(
            const AccelerationStructureDescription &desc
        ) final;
        TexelBufferHandle CreateTexelBuffer(
            const TexelBufferDescription &desc
        ) final;

        const GraphicsCapabilities GetGraphicsCapabilities() const final;
        TextureHandle CreateTexture(const TextureDescription &spec) final;
        TextureViewHandle CreateTextureView(
            const TextureViewDescription &desc
        ) final;
        FenceHandle CreateFence(const FenceDescription &desc) final;
        std::vector<QueueFamilyInfo> GetQueueFamilies() final;
        FenceWaitResult WaitForFences(
            FenceHandle *fences, uint32_t count, bool waitAll, uint64_t timeoutNS
        ) final;
        CommandQueueHandle CreateCommandQueue(
            const CommandQueueDescription &description
        ) final;
        void ResetFences(FenceHandle *fences, uint32_t count) final;

        ShaderLanguage GetSupportedShaderFormat() final;
        bool IsBufferUsageSupported(BufferUsage usage) final;
        void WaitForIdle() final;
        float GetUVCorrection() final
        {
            return -1.0f;
        }
        bool IsUVOriginTopLeft() final
        {
            return true;
        };

        GraphicsAPIInfo GetGraphicsAPI() final;

        void SetObjectName(VkObjectType type, uint64_t handle, const char *name);

        VkInstance GetVkInstance();
        VkDevice GetVkDevice();
        uint32_t GetCurrentFrameIndex();
        VmaAllocator GetAllocator();

        uint32_t FindMemoryType(
            uint32_t typeFilter, VkMemoryPropertyFlags properties,
            std::shared_ptr<PhysicalDeviceVk> physicalDevice
        );
        Vk::AllocatedBuffer CreateBuffer(
            size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage
        );

        const VulkanDeviceFeatures GetDeviceFeatures() const;

        bool Validate() final;

        PixelFormatProperties GetPixelFormatProperties(
            PixelFormat format, TextureType type, TextureUsageFlags usage
        ) const final;
        const DeviceFeatures &GetPhysicalDeviceFeatures() const final;
        const DeviceLimits &GetPhysicalDeviceLimits() const final;
        bool IsIndexBufferFormatSupported(IndexFormat format) const final;
        AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(
            const AccelerationStructureGeometryBuildDescription &description
        ) const final;

        RayTracingDeviceDescription GetRayTracingDeviceDescription() const final;
        AccelerationStructureProperties
        GetAccelerationStructureProperties() const final;

        SurfaceHandle CreateSurfaceFromWin32(
            uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance
        ) final;
        SurfaceHandle CreateSurfaceFromX11(
            uintptr_t display, uint32_t screen, uint32_t window
        ) final;
        SurfaceHandle CreateSurfaceFromWayland(
            uintptr_t display, uintptr_t surface
        ) final;
        SurfaceHandle CreateSurfaceFromAndroid(uintptr_t nativeWindow) final;
        SurfaceHandle CreateSurfaceFromHTML(const std::string &canvasId) final;

        bool IsExtensionSupported(const char *extension) const;
        bool IsVersionGreaterThan(uint32_t version) const;

        const GladVulkanContext &GetVulkanContext() const;

        // vulkan functions
        virtual ShaderModuleHandle CreateShaderModule(
            const ShaderModuleDescription &moduleSpec
        ) override;

      private:
        void RetrieveQueueFamilies(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
        void CreateDevice(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
        void CreateAllocator(
            std::shared_ptr<PhysicalDeviceVk> physicalDevice, VkInstance instance
        );

      private:
        std::vector<const char *> GetRequiredDeviceExtensions();
        std::vector<std::string> GetSupportedDeviceExtensions(
            std::shared_ptr<PhysicalDeviceVk> physicalDevice
        );

        VkDeviceAddress GetBufferDeviceAddress(Ref<DeviceBufferVk> buffer);

      private:
        std::shared_ptr<PhysicalDeviceVk> m_PhysicalDevice = nullptr;
        VkInstance m_Instance = nullptr;
        VkDevice m_Device = VK_NULL_HANDLE;
        std::vector<QueueFamilyInfo> m_QueueFamilies = {};

        // VMA types
        VmaAllocator m_Allocator;

        uint32_t m_FrameNumber = 0;
        uint32_t m_CurrentFrameIndex = 0;
        std::unique_ptr<CommandExecutorVk> m_CommandExecutor = nullptr;

        VulkanDeviceConfig m_DeviceConfig = {};
        VulkanDeviceFeatures m_DeviceFeatures = {};

        DeviceFeatures m_Features = {};
        DeviceLimits m_Limits = {};

        GladVulkanContext m_Context = {};

        GraphicsAPIInfo m_GraphicsAPIInfo = {};

        GraphicsResourceManager m_Resources = {};

        friend class SwapchainVk;
    };
} // namespace Nexus::Graphics

#endif