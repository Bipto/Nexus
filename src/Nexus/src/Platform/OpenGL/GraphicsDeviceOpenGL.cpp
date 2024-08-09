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

#if defined(WIN32)
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *msg, const void *data)
{
    std::string textText;
    std::string typeText;
    std::string severityText;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        textText = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        textText = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        textText = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        textText = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        textText = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        textText = "UNKNOWN";
        break;

    default:
        textText = "UNKNOWN";
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        typeText = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeText = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeText = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        typeText = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        typeText = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        typeText = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        typeText = "MARKER";
        break;

    default:
        typeText = "UNKNOWN";
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        severityText = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        severityText = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        severityText = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severityText = "NOTIFICATION";
        break;

    default:
        severityText = "UNKNOWN";
        break;
    }

    if (type != GL_DEBUG_TYPE_OTHER)
    {
        std::stringstream ss;
        ss << "GL error of :";
        ss << severityText;
        ss << ", raised from ";
        ss << textText;

        NX_ERROR(ss.str());
    }
}
#endif

namespace Nexus::Graphics
{
    GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec)
        : GraphicsDevice(createInfo, window, swapchainSpec)
    {

        window->CreateSwapchain(this, swapchainSpec);

#if defined(WIN32)
        gladLoadGL();
        glCall(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        // glDebugMessageCallback(GLDebugMessageCallback, nullptr);
#endif

        m_Extensions = GetSupportedExtensions();
    }

    void GraphicsDeviceOpenGL::SetContext()
    {
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

    void *GraphicsDeviceOpenGL::GetContext()
    {
        return nullptr;
    }

    void GraphicsDeviceOpenGL::BeginFrame()
    {
    }

    void GraphicsDeviceOpenGL::EndFrame()
    {
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