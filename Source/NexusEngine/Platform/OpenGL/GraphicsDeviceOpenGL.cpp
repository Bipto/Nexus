#include "GraphicsDeviceOpenGL.hpp"

#include "PipelineOpenGL.hpp"
#include "ShaderOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "CommandListOpenGL.hpp"
#include "RenderPassOpenGL.hpp"

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

namespace Nexus::Graphics
{
    GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(const GraphicsDeviceCreateInfo &createInfo, Window *window)
        : GraphicsDevice(createInfo, window)
    {
#ifndef __EMSCRIPTEN__
        gladLoadGL();
#endif

        this->m_Context = SDL_GL_CreateContext(this->m_Window->GetSDLWindowHandle());

        if (this->m_Context == NULL)
        {
            std::cout << SDL_GetError() << std::endl;
        }

#ifndef __EMSCRIPTEN__
        gladLoadGL();
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugMessageCallback, nullptr);
#endif

        m_Swapchain = new SwapchainOpenGL(m_Window->GetSDLWindowHandle(), createInfo.VSyncStateSettings);
    }

    GraphicsDeviceOpenGL::~GraphicsDeviceOpenGL()
    {
        SDL_GL_DeleteContext(m_Context);
    }

    void GraphicsDeviceOpenGL::SetContext()
    {
        SDL_GL_MakeCurrent(this->m_Window->GetSDLWindowHandle(), this->m_Context);
    }

    void GraphicsDeviceOpenGL::SetFramebuffer(Ref<Framebuffer> framebuffer)
    {
        Ref<FramebufferOpenGL> fb = std::dynamic_pointer_cast<FramebufferOpenGL>(framebuffer);
        if (framebuffer)
        {
            fb->Bind();
            m_BoundFramebuffer = fb;
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            m_BoundFramebuffer = nullptr;
        }
    }

    void GraphicsDeviceOpenGL::SubmitCommandList(Ref<CommandList> commandList)
    {
        Ref<CommandListOpenGL> commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
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
        return (void *)this->m_Context;
    }

    void GraphicsDeviceOpenGL::BeginFrame()
    {
    }

    void GraphicsDeviceOpenGL::EndFrame()
    {
    }

    Ref<Shader> GraphicsDeviceOpenGL::CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout)
    {
        return CreateRef<ShaderOpenGL>(vertexShaderSource, fragmentShaderSource, layout);
    }

    Ref<Texture> GraphicsDeviceOpenGL::CreateTexture(const TextureSpecification &spec)
    {
        return CreateRef<TextureOpenGL>(spec);
    }

    Ref<Framebuffer> GraphicsDeviceOpenGL::CreateFramebuffer(Ref<RenderPass> renderPass)
    {
        auto framebufferOpenGL = CreateRef<FramebufferOpenGL>(renderPass);
        auto renderPassOpenGL = std::dynamic_pointer_cast<RenderPassOpenGL>(renderPass);
        renderPassOpenGL->m_Framebuffer = framebufferOpenGL;
        return framebufferOpenGL;
    }

    Ref<Pipeline> GraphicsDeviceOpenGL::CreatePipeline(const PipelineDescription &description)
    {
        return CreateRef<PipelineOpenGL>(description);
    }

    Ref<CommandList> GraphicsDeviceOpenGL::CreateCommandList()
    {
        return CreateRef<CommandListOpenGL>(this);
    }

    Ref<VertexBuffer> GraphicsDeviceOpenGL::CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
    {
        return CreateRef<VertexBufferOpenGL>(description, data, layout);
    }

    Ref<IndexBuffer> GraphicsDeviceOpenGL::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
    {
        return CreateRef<IndexBufferOpenGL>(description, data, format);
    }

    Ref<UniformBuffer> GraphicsDeviceOpenGL::CreateUniformBuffer(const BufferDescription &description, const void *data)
    {
        return CreateRef<UniformBufferOpenGL>(description, data);
    }

    Ref<RenderPass> GraphicsDeviceOpenGL::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification)
    {
        return CreateRef<RenderPassOpenGL>(renderPassSpecification, framebufferSpecification);
    }

    Ref<RenderPass> GraphicsDeviceOpenGL::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        return CreateRef<RenderPassOpenGL>(renderPassSpecification, swapchain);
    }

    Ref<ResourceSet> GraphicsDeviceOpenGL::CreateResourceSet(const ResourceSetSpecification &spec)
    {
        return nullptr;
    }

    void GraphicsDeviceOpenGL::Resize(Point<int> size)
    {
        m_Swapchain->Resize(size.X, size.Y);

        if (m_BoundFramebuffer)
            m_BoundFramebuffer->Bind();
    }

    ShaderLanguage GraphicsDeviceOpenGL::GetSupportedShaderFormat()
    {
#if defined(EMSCRIPTEN)
        return ShaderLanguage::GLSLES;
#else
        return ShaderLanguage::GLSL;
#endif
    }

    Swapchain *GraphicsDeviceOpenGL::GetSwapchain()
    {
        return m_Swapchain;
    }
}
