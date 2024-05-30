#if defined(NX_PLATFORM_OPENGL)

#include "GraphicsDeviceOpenGL.hpp"

#include "PipelineOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "CommandListOpenGL.hpp"
#include "ResourceSetOpenGL.hpp"
#include "SamplerOpenGL.hpp"
#include "ShaderModuleOpenGL.hpp"

#if defined(WIN32)
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *msg, const void *data)
{
    char *_source;
    char *_type;
    char *_severity;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    if (type != GL_DEBUG_TYPE_OTHER)
        printf("%d: %s of %s severity, raised from %s: %s\n",
               id, _type, _severity, _source, msg);
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
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugMessageCallback, nullptr);
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
        auto &commands = commandListGL->GetRenderCommands();

        for (auto &command : commands)
        {
            command(commandList);
        }
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
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);

        for (GLint i = 0; i < n; i++)
        {
            const char *extension = (const char *)glGetStringi(GL_EXTENSIONS, i);
            extensions.push_back(extension);
        }

        return extensions;
    }

    Ref<Texture> GraphicsDeviceOpenGL::CreateTexture(const TextureSpecification &spec)
    {
        return CreateRef<TextureOpenGL>(spec);
    }

    Ref<Pipeline> GraphicsDeviceOpenGL::CreatePipeline(const PipelineDescription &description)
    {
        return CreateRef<PipelineOpenGL>(description);
    }

    Ref<CommandList> GraphicsDeviceOpenGL::CreateCommandList()
    {
        return CreateRef<CommandListOpenGL>(this);
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