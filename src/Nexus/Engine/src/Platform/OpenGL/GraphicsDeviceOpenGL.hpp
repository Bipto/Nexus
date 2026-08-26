#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "RHI/GraphicsDevice.hpp"

#include "CommandExecutorOpenGL.hpp"
#include "FramebufferOpenGL.hpp"
#include "GL.hpp"
#include "SwapchainOpenGL.hpp"

#include "PhysicalDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceOpenGL : public IGraphicsDevice
    {
      public:
        GraphicsDeviceOpenGL(std::shared_ptr<IPhysicalDevice> physicalDevicel, bool enableDebug);
        GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL &) = delete;
        virtual ~GraphicsDeviceOpenGL();

        std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const final;

        PipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDescription &description) final;
        PipelineHandle CreateComputePipeline(const ComputePipelineDescription &description) final;
        PipelineHandle CreateMeshletPipeline(const MeshletPipelineDescription &description) final;
        PipelineHandle CreateRayTracingPipeline(const RayTracingPipelineDescription &description) final;

        ResourceSetHandle CreateResourceSet(PipelineHandle pipeline) final;
        FramebufferHandle CreateFramebuffer(const FramebufferTextureSetDescription &desc) final;
        SamplerHandle CreateSampler(const SamplerDescription &spec) final;
        TimingQueryHandle CreateTimingQuery() final;
        DeviceBufferHandle CreateDeviceBuffer(const DeviceBufferDescription &desc) final;
        AccelerationStructureHandle CreateAccelerationStructure(const AccelerationStructureDescription &desc) final;
        TexelBufferHandle CreateTexelBuffer(const TexelBufferDescription &desc) final;

        const GraphicsCapabilities GetGraphicsCapabilities() const final;
        TextureHandle CreateTexture(const TextureDescription &spec) final;
        TextureViewHandle CreateTextureView(const TextureViewDescription &desc) final;
        FenceHandle CreateFence(const FenceDescription &desc) final;
        FenceWaitResult WaitForFences(FenceHandle *fences, uint32_t count, bool waitAll, uint64_t timeoutNS) final;
        std::vector<QueueFamilyInfo> GetQueueFamilies() final;
        CommandQueueHandle CreateCommandQueue(const CommandQueueDescription &description) final;
        void ResetFences(FenceHandle *fences, uint32_t count) final;
        ShaderLanguage GetSupportedShaderFormat() final;
        bool IsBufferUsageSupported(BufferUsage usage) final;
        void WaitForIdle() final;
        float GetUVCorrection() final
        {
            return 1.0f;
        }
        bool IsUVOriginTopLeft() final
        {
            return false;
        };

        GraphicsAPIInfo GetGraphicsAPI() final;

        bool Validate() final;

        GL::IOffscreenContext *GetOffscreenContext();

        const DeviceFeatures &GetPhysicalDeviceFeatures() const final;
        const DeviceLimits &GetPhysicalDeviceLimits() const final;
        bool IsIndexBufferFormatSupported(IndexFormat format) const final;
        AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(
            const AccelerationStructureGeometryBuildDescription &description) const final;
        RayTracingDeviceDescription GetRayTracingDeviceDescription() const final;
        AccelerationStructureProperties GetAccelerationStructureProperties() const final;

        SurfaceHandle CreateSurfaceFromWin32(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance) final;
        SurfaceHandle CreateSurfaceFromX11(uintptr_t display, uint32_t screen, uint32_t window) final;
        SurfaceHandle CreateSurfaceFromWayland(uintptr_t display, uintptr_t surface) final;
        SurfaceHandle CreateSurfaceFromAndroid(uintptr_t nativeWindow) final;
        SurfaceHandle CreateSurfaceFromHTML(const std::string &canvasId) final;

        Ref<PhysicalDeviceOpenGL> GetPhysicalDeviceOpenGL();
        ShaderModuleHandle CreateShaderModule(const ShaderModuleDescription &moduleDesc) final;
        PixelFormatProperties GetPixelFormatProperties(PixelFormat format, TextureType type,
                                                       TextureUsageFlags usage) const final;

      private:
        std::vector<std::string> GetSupportedExtensions(GL::IGLContext *context);
        void GetFeatures();

      private:
        WeakRef<FramebufferOpenGL> m_BoundFramebuffer = {};

        std::vector<std::string> m_Extensions{};

        CommandExecutorOpenGL m_CommandExecutor{};

        std::string m_APIName = {};
        std::string m_RendererName = {};
        std::shared_ptr<PhysicalDeviceOpenGL> m_PhysicalDevice = nullptr;

        DeviceFeatures m_Features = {};
        DeviceLimits m_Limits = {};
        GraphicsResourceManager m_Resources = {};
    };
} // namespace Nexus::Graphics

#endif