#if defined(NX_PLATFORM_OPENGL)

#include "GraphicsDeviceOpenGL.hpp"

#include "PipelineOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "CommandListOpenGL.hpp"
#include "ResourceSetOpenGL.hpp"
#include "SamplerOpenGL.hpp"
#include "ShaderModuleOpenGL.hpp"
#include "TimingQueryOpenGL.hpp"

namespace Nexus::Graphics
{
    GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(const GraphicsDeviceSpecification &createInfo, Window *window, const SwapchainSpecification &swapchainSpec)
        : GraphicsDevice(createInfo, window, swapchainSpec)
    {

        window->CreateSwapchain(this, swapchainSpec);

#if defined(WIN32)
        gladLoadGL();
#endif

        m_Extensions = GetSupportedExtensions();
    }

    void GraphicsDeviceOpenGL::SetFramebuffer(Ref<Framebuffer> framebuffer)
    {
        auto fb = std::dynamic_pointer_cast<FramebufferOpenGL>(framebuffer);
        if (framebuffer)
        {
            fb->BindAsRenderTarget();
            m_BoundFramebuffer = fb;
        }
    }

    void GraphicsDeviceOpenGL::SetSwapchain(Swapchain *swapchain)
    {
        auto glSwapchain = (SwapchainOpenGL *)swapchain;
        if (glSwapchain)
        {
            glSwapchain->BindAsRenderTarget();
        }
    }

    void GraphicsDeviceOpenGL::SubmitCommandList(Ref<CommandList> commandList)
    {
        auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);

        CommandRecorder &recorder = commandListGL->GetCommandRecorder();
        m_CommandExecutor.ExecuteCommands(recorder.GetCommands(), this);
        m_CommandExecutor.Reset();
    }

    const std::string GraphicsDeviceOpenGL::GetAPIName()
    {
        std::string name = std::string("OpenGL - ") + std::string((const char *)glGetString(GL_VERSION));
        return name;
    }

    const char *GraphicsDeviceOpenGL::GetDeviceName()
    {
        return (const char *)glGetString(GL_RENDERER);
    }

    void GraphicsDeviceOpenGL::OpenGLCall(std::function<void()> func)
    {
        glClearErrors();
        func();
        if (m_Specification.DebugLayer)
        {
            glCheckErrors();
        }
    }

    Ref<ShaderModule> GraphicsDeviceOpenGL::CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources)
    {
        return CreateRef<ShaderModuleOpenGL>(moduleSpec, resources);
    }

    std::vector<std::string> GraphicsDeviceOpenGL::GetSupportedExtensions()
    {
        std::vector<std::string> extensions;

        GLint n = 0;
        glCall(glGetIntegerv(GL_NUM_EXTENSIONS, &n));

        for (GLint i = 0; i < n; i++)
        {
            const char *extension = (const char *)glGetStringi(GL_EXTENSIONS, i);
            extensions.push_back(extension);
        }

        return extensions;
    }

    Ref<Texture> GraphicsDeviceOpenGL::CreateTexture(const TextureSpecification &spec)
    {
        return CreateRef<TextureOpenGL>(spec, this);
    }

    Ref<Pipeline> GraphicsDeviceOpenGL::CreatePipeline(const PipelineDescription &description)
    {
        return CreateRef<PipelineOpenGL>(description);
    }

    Ref<CommandList> GraphicsDeviceOpenGL::CreateCommandList()
    {
        return CreateRef<CommandListOpenGL>();
    }

    Ref<VertexBuffer> GraphicsDeviceOpenGL::CreateVertexBuffer(const BufferDescription &description, const void *data)
    {
        return CreateRef<VertexBufferOpenGL>(description, data);
    }

    Ref<IndexBuffer> GraphicsDeviceOpenGL::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
    {
        return CreateRef<IndexBufferOpenGL>(description, data, format);
    }

    Ref<UniformBuffer> GraphicsDeviceOpenGL::CreateUniformBuffer(const BufferDescription &description, const void *data)
    {
        return CreateRef<UniformBufferOpenGL>(description, data);
    }

    Ref<ResourceSet> GraphicsDeviceOpenGL::CreateResourceSet(const ResourceSetSpecification &spec)
    {
        return CreateRef<ResourceSetOpenGL>(spec);
    }

    Ref<Framebuffer> GraphicsDeviceOpenGL::CreateFramebuffer(const FramebufferSpecification &spec)
    {
        return CreateRef<FramebufferOpenGL>(spec, this);
    }

    Ref<Sampler> GraphicsDeviceOpenGL::CreateSampler(const SamplerSpecification &spec)
    {
        return CreateRef<SamplerOpenGL>(spec);
    }

    Ref<TimingQuery> GraphicsDeviceOpenGL::CreateTimingQuery()
    {
        return CreateRef<TimingQueryOpenGL>();
    }

    const GraphicsCapabilities GraphicsDeviceOpenGL::GetGraphicsCapabilities() const
    {
        GraphicsCapabilities capabilities;

#if defined(NX_PLATFORM_GL_DESKTOP)
        capabilities.SupportsLODBias = true;
        capabilities.SupportsMultisampledTextures = true;
        capabilities.SupportsMultipleSwapchains = true;
#endif

        return capabilities;
    }

    ShaderLanguage GraphicsDeviceOpenGL::GetSupportedShaderFormat()
    {
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__) || defined(ANDROID)
        return ShaderLanguage::GLSLES;
#else
        return ShaderLanguage::GLSL;
#endif
    }
}

#endif