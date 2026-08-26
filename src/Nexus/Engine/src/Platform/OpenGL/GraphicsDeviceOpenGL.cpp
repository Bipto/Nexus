#if defined(NX_PLATFORM_OPENGL)

#include "GraphicsDeviceOpenGL.hpp"

#include "CommandListOpenGL.hpp"
#include "CommandQueueOpenGL.hpp"
#include "DeviceBufferOpenGL.hpp"
#include "FenceOpenGL.hpp"
#include "PipelineOpenGL.hpp"
#include "ResourceSetOpenGL.hpp"
#include "SamplerOpenGL.hpp"
#include "ShaderModuleOpenGL.hpp"
#include "SwapchainOpenGL.hpp"
#include "TexelBufferOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "TextureViewOpenGL.hpp"
#include "TimingQueryOpenGL.hpp"

#if defined(WIN32)
#include "Surface/WGL/SurfaceWGL.hpp"
#elif defined(__linux__)
#include "Surface/EGL/SurfaceEGL.hpp"
#endif

#include "Platform/OpenGL/OpenGLInclude.hpp"

namespace Nexus::Graphics
{
    static void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam)
    {
        if (type == GL_DEBUG_TYPE_ERROR)
        {
            std::cout << "OpenGL Debug Message: " << message << std::endl;
        }
    }

    GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(std::shared_ptr<IPhysicalDevice> physicalDevice, bool enableDebug)
    {
        m_PhysicalDevice = std::dynamic_pointer_cast<PhysicalDeviceOpenGL>(physicalDevice);

        GL::IGLContext *context = m_PhysicalDevice->GetOffscreenContext();
        // retrieve available extensions
        m_Extensions = GetSupportedExtensions(context);

        // retrieve API and graphics adapter name
        m_APIName = std::string("OpenGL - ") + std::string((const char *)context->GetString(GL_VERSION));
        m_RendererName = (const char *)context->GetString(GL_RENDERER);

        // enable debugging if available
#if !defined(__EMSCRIPTEN__)
        if (enableDebug)
        {
            context->EnableCapability(GL_DEBUG_OUTPUT, true);
            context->EnableCapability(GL_DEBUG_OUTPUT_SYNCHRONOUS, true);

            context->DebugMessageCallback(glDebugCallback, nullptr);
        }
#endif
        // set pixel alignment to the default globally
        context->PixelStorei(GL_PACK_ALIGNMENT, 4);
        context->PixelStorei(GL_UNPACK_ALIGNMENT, 4);

        GetFeatures();
    }

    GraphicsDeviceOpenGL::~GraphicsDeviceOpenGL()
    {
        // release all resources before cleaning up the vulkan device
        m_Resources = {};
    }

    std::shared_ptr<IPhysicalDevice> GraphicsDeviceOpenGL::GetPhysicalDevice() const
    {
        return m_PhysicalDevice;
    }

    GL::IOffscreenContext *GraphicsDeviceOpenGL::GetOffscreenContext()
    {
        return m_PhysicalDevice->GetOffscreenContext();
    }

    ShaderModuleHandle GraphicsDeviceOpenGL::CreateShaderModule(const ShaderModuleDescription &moduleDesc)
    {
        auto shader = std::make_unique<ShaderModuleOpenGL>(moduleDesc, this);
        return m_Resources.ShaderModules.CreateShared(std::move(shader));
    }

    std::vector<std::string> GraphicsDeviceOpenGL::GetSupportedExtensions(GL::IGLContext *context)
    {
        std::vector<std::string> extensions;

        GLint n = 0;
        context->GetIntegerv(GL_NUM_EXTENSIONS, &n);

        for (GLint i = 0; i < n; i++)
        {
            const char *extension = (const char *)context->GetStringi(GL_EXTENSIONS, i);
            extensions.push_back(extension);
        }

        return extensions;
    }

    void GraphicsDeviceOpenGL::GetFeatures()
    {
        GL::IOffscreenContext *offscreenContext = m_PhysicalDevice->GetOffscreenContext();
        m_Features = offscreenContext->GetDeviceFeatures();
    }

    PixelFormatProperties GraphicsDeviceOpenGL::GetPixelFormatProperties(PixelFormat format, TextureType type,
                                                                         TextureUsageFlags usage) const
    {
        PixelFormatProperties properties = {};
        return properties;
    }

    const DeviceFeatures &GraphicsDeviceOpenGL::GetPhysicalDeviceFeatures() const
    {
        return m_Features;
    }

    const DeviceLimits &GraphicsDeviceOpenGL::GetPhysicalDeviceLimits() const
    {
        return m_Limits;
    }

    bool GraphicsDeviceOpenGL::IsIndexBufferFormatSupported(IndexFormat format) const
    {
        switch (format)
        {
        case IndexFormat::UInt8:
        case IndexFormat::UInt16:
        case IndexFormat::UInt32:
            return true;

        default:
            throw std::runtime_error("Failed to find a valid format");
        }
    }

    AccelerationStructureBuildSizeDescription GraphicsDeviceOpenGL::GetAccelerationStructureBuildSize(
        const AccelerationStructureGeometryBuildDescription &description) const
    {
        NX_VALIDATE(0, "Ray tracing not supported on OpenGL backend");
        return AccelerationStructureBuildSizeDescription();
    }

    RayTracingDeviceDescription GraphicsDeviceOpenGL::GetRayTracingDeviceDescription() const
    {
        NX_VALIDATE(0, "Ray tracing not supported on OpenGL backend");
        return RayTracingDeviceDescription();
    }

    AccelerationStructureProperties GraphicsDeviceOpenGL::GetAccelerationStructureProperties() const
    {
        NX_VALIDATE(0, "Ray tracing not supported on OpenGL backend");
        return AccelerationStructureProperties();
    }

    SurfaceHandle GraphicsDeviceOpenGL::CreateSurfaceFromWin32(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance)
    {
#if defined(WIN32)
        auto surface = std::make_unique<SurfaceWGL>(hwnd, hdc, hinstance, this);
        return m_Resources.Surfaces.CreateShared(std::move(surface));
#else
        return {};
#endif
    }

    SurfaceHandle GraphicsDeviceOpenGL::CreateSurfaceFromX11(uintptr_t display, uint32_t screen, uint32_t window)
    {
#if defined(__linux__)
        auto surface = std::make_unique<SurfaceEGL>(display, screen, window, this);
        return m_Resources.Surfaces.CreateShared(std::move(surface));
#else
        return {};
#endif
    }

    SurfaceHandle GraphicsDeviceOpenGL::CreateSurfaceFromWayland(uintptr_t display, uintptr_t surface)
    {
        return {};
    }

    SurfaceHandle GraphicsDeviceOpenGL::CreateSurfaceFromAndroid(uintptr_t nativeWindow)
    {
        return {};
    }

    SurfaceHandle GraphicsDeviceOpenGL::CreateSurfaceFromHTML(const std::string &canvasId)
    {
        return {};
    }

    Ref<PhysicalDeviceOpenGL> GraphicsDeviceOpenGL::GetPhysicalDeviceOpenGL()
    {
        return m_PhysicalDevice;
    }

    PipelineHandle GraphicsDeviceOpenGL::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
    {
        auto pipeline = std::make_unique<GraphicsPipelineOpenGL>(description, this);
        return m_Resources.Pipelines.CreateShared(std::move(pipeline));
    }

    PipelineHandle GraphicsDeviceOpenGL::CreateComputePipeline(const ComputePipelineDescription &description)
    {
        auto pipeline = std::make_unique<ComputePipelineOpenGL>(description, this);
        return m_Resources.Pipelines.CreateShared(std::move(pipeline));
    }

    PipelineHandle GraphicsDeviceOpenGL::CreateMeshletPipeline(const MeshletPipelineDescription &description)
    {
        NX_VALIDATE(false, "Meshlet pipelines are not supported by OpenGL");
        return {};
    }

    PipelineHandle GraphicsDeviceOpenGL::CreateRayTracingPipeline(const RayTracingPipelineDescription &description)
    {
        NX_VALIDATE(false, "Ray tracing pipelines are not supported by OpenGL");
        return {};
    }

    ResourceSetHandle GraphicsDeviceOpenGL::CreateResourceSet(PipelineHandle pipeline)
    {
        auto resourceSet = std::make_unique<ResourceSetOpenGL>(pipeline, this);
        return m_Resources.ResourceSets.CreateShared(std::move(resourceSet));
    }

    FramebufferHandle GraphicsDeviceOpenGL::CreateFramebuffer(const FramebufferTextureSetDescription &desc)
    {
        auto framebuffer = std::make_unique<FramebufferOpenGL>(desc, this);
        return m_Resources.Framebuffers.CreateShared(std::move(framebuffer));
    }

    SamplerHandle GraphicsDeviceOpenGL::CreateSampler(const SamplerDescription &spec)
    {
        auto sampler = std::make_unique<SamplerOpenGL>(spec, this);
        return m_Resources.Samplers.CreateShared(std::move(sampler));
    }

    TimingQueryHandle GraphicsDeviceOpenGL::CreateTimingQuery()
    {
        auto timingQuery = std::make_unique<TimingQueryOpenGL>();
        return m_Resources.TimingQueries.CreateShared(std::move(timingQuery));
    }

    DeviceBufferHandle GraphicsDeviceOpenGL::CreateDeviceBuffer(const DeviceBufferDescription &desc)
    {
        auto deviceBuffer = std::make_unique<DeviceBufferOpenGL>(desc, this);
        return m_Resources.DeviceBuffers.CreateShared(std::move(deviceBuffer));
    }

    AccelerationStructureHandle GraphicsDeviceOpenGL::CreateAccelerationStructure(
        const AccelerationStructureDescription &desc)
    {
        return {};
    }

    TexelBufferHandle GraphicsDeviceOpenGL::CreateTexelBuffer(const TexelBufferDescription &desc)
    {
        auto texelBuffer = std::make_unique<TexelBufferOpenGL>(desc, this);
        return m_Resources.TexelBuffers.CreateShared(std::move(texelBuffer));
    }

    const GraphicsCapabilities GraphicsDeviceOpenGL::GetGraphicsCapabilities() const
    {
        GraphicsCapabilities capabilities;

#if defined(NX_PLATFORM_GL_DESKTOP)
        capabilities.SupportsLODBias = true;
        capabilities.SupportsMultisampledTextures = true;
        capabilities.SupportsMultipleSwapchains = true;
        capabilities.SupportsSeparateColourAndBlendMasks = true;
#endif

        return capabilities;
    }

    FenceHandle GraphicsDeviceOpenGL::CreateFence(const FenceDescription &desc)
    {
        auto fence = std::make_unique<FenceOpenGL>(desc, this);
        return m_Resources.Fences.CreateShared(std::move(fence));
    }

    FenceWaitResult GraphicsDeviceOpenGL::WaitForFences(FenceHandle *fences, uint32_t count, bool waitAll,
                                                        uint64_t timeoutNS)
    {
        std::vector<FenceWaitResult> success(count);

        for (uint32_t i = 0; i < count; i++)
        {
            FenceOpenGL *fence = fences[i].AsDerived<FenceOpenGL>();

            GLenum result = fence->Wait(timeoutNS);
            if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED)
            {
                // if a fence has been signalled successfully and we are not waiting
                // for all the fences, we can return that a fence has been signalled
                if (!waitAll)
                {
                    return FenceWaitResult::Signalled;
                }

                success[i] = FenceWaitResult::Signalled;
            }
            else if (result == GL_TIMEOUT_EXPIRED)
            {
                success[i] = FenceWaitResult::TimedOut;
            }
            else
            {
                success[i] = FenceWaitResult::Failed;
            }
        }

        // if we are waiting for all fences, we need to check that they have all been
        // completed
        bool allCompleted = true;
        bool errorEncountered = false;
        for (size_t i = 0; i < success.size(); i++)
        {
            if (success[i] != FenceWaitResult::Signalled)
            {
                allCompleted = false;
            }

            if (success[i] == FenceWaitResult::Failed)
            {
                errorEncountered = true;
            }
        }

        if (!errorEncountered)
        {
            if (allCompleted)
            {
                return FenceWaitResult::Signalled;
            }
            else
            {
                return FenceWaitResult::TimedOut;
            }
        }

        return FenceWaitResult::Failed;
    }

    std::vector<QueueFamilyInfo> GraphicsDeviceOpenGL::GetQueueFamilies()
    {
        std::vector<QueueFamilyInfo> queueFamilies = {};

        QueueFamilyInfo &info = queueFamilies.emplace_back();
        info.QueueFamily = 0;
        info.QueueCount = std::numeric_limits<uint32_t>::max();
        info.Capabilities =
            QueueCapabilities(QueueCapabilities::Graphics | QueueCapabilities::Compute | QueueCapabilities::Transfer);

        GL::IGLContext *context = m_PhysicalDevice->GetOffscreenContext();
        if (context->SupportsSparseBuffers() && context->SupportsSparseTextures())
        {
            info.Capabilities = QueueCapabilities(info.Capabilities | QueueCapabilities::SparseBinding);
        }

        return queueFamilies;
    }

    CommandQueueHandle GraphicsDeviceOpenGL::CreateCommandQueue(const CommandQueueDescription &description)
    {
        auto commandQueue = std::make_unique<CommandQueueOpenGL>(this, description);
        return m_Resources.CommandQueues.CreateShared(std::move(commandQueue));
    }

    void GraphicsDeviceOpenGL::ResetFences(FenceHandle *fences, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            FenceOpenGL *fence = fences[i].AsDerived<FenceOpenGL>();
            fence->Reset();
        }
    }

    TextureHandle GraphicsDeviceOpenGL::CreateTexture(const TextureDescription &spec)
    {
        auto texture = std::make_unique<TextureOpenGL>(spec, this);
        return m_Resources.Textures.CreateShared(std::move(texture));
    }

    TextureViewHandle GraphicsDeviceOpenGL::CreateTextureView(const TextureViewDescription &desc)
    {
        auto textureView = std::make_unique<TextureViewOpenGL>(desc, this);
        return m_Resources.TextureViews.CreateShared(std::move(textureView));
    }

    ShaderLanguage GraphicsDeviceOpenGL::GetSupportedShaderFormat()
    {
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__) || defined(ANDROID)
        return ShaderLanguage::GLSLES;
#else
        return ShaderLanguage::GLSL;
#endif
    }

    bool GraphicsDeviceOpenGL::IsBufferUsageSupported(BufferUsage usage)
    {
        return false;
    }

    void GraphicsDeviceOpenGL::WaitForIdle()
    {
    }

    GraphicsAPIInfo GraphicsDeviceOpenGL::GetGraphicsAPI()
    {
        GLint major = 0;
        GLint minor = 0;

        GL::IGLContext *context = m_PhysicalDevice->GetOffscreenContext();

        context->GetIntegerv(GL_MAJOR_VERSION, &major);
        context->GetIntegerv(GL_MINOR_VERSION, &minor);

        return GraphicsAPIInfo{
            .API = GraphicsAPI::OpenGL,
            .Major = static_cast<uint32_t>(major),
            .Minor = static_cast<uint32_t>(minor),
        };
    }

    bool GraphicsDeviceOpenGL::Validate()
    {
        GL::IOffscreenContext *context = m_PhysicalDevice->GetOffscreenContext();
        return context->Validate();
    }
} // namespace Nexus::Graphics

#endif