#include "GraphicsDeviceOpenGL.h"

#include "backends/imgui_impl_opengl3.h"

#include "PipelineOpenGL.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
#include "TextureOpenGL.h"
#include "CommandListOpenGL.h"

namespace Nexus::Graphics
{
    GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(const GraphicsDeviceCreateInfo &createInfo)
        : GraphicsDevice(createInfo)
    {
// Decide GL+GLSL versions
#ifdef __EMSCRIPTEN__
        // GL ES 2.0 + GLSL 100
        m_GlslVersion = "#version 100";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
        m_GlslVersion = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
        // GL 3.0 + GLSL 130
        m_GlslVersion = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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
#endif

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
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
        auto commandCount = commandListGL->GetCommandCount();

        for (int i = 0; i < commandCount; i++)
        {
            RenderCommand command = commands[i];
            command(commandList);
        }
    }

    void GraphicsDeviceOpenGL::SetViewport(const Viewport &viewport)
    {
        m_Viewport = viewport;
        glViewport(
            viewport.X,
            viewport.Y,
            viewport.Width,
            viewport.Height);
    }

    const Viewport &GraphicsDeviceOpenGL::GetViewport()
    {
        return m_Viewport;
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

    Ref<Shader> GraphicsDeviceOpenGL::CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout)
    {
        return CreateRef<ShaderOpenGL>(vertexShaderSource, fragmentShaderSource, layout);
    }

    Ref<VertexBuffer> GraphicsDeviceOpenGL::CreateVertexBuffer(const std::vector<Vertex> vertices)
    {
        return CreateRef<VertexBufferOpenGL>(vertices);
    }

    Ref<IndexBuffer> GraphicsDeviceOpenGL::CreateIndexBuffer(const std::vector<unsigned int> indices)
    {
        return CreateRef<IndexBufferOpenGL>(indices);
    }

    Ref<UniformBuffer> GraphicsDeviceOpenGL::CreateUniformBuffer(const UniformResourceBinding &binding)
    {
        return CreateRef<UniformBufferOpenGL>(binding);
    }

    Ref<Texture> GraphicsDeviceOpenGL::CreateTexture(TextureSpecification spec)
    {
        return CreateRef<TextureOpenGL>(spec);
    }

    Ref<Framebuffer> GraphicsDeviceOpenGL::CreateFramebuffer(const FramebufferSpecification &spec)
    {
        return CreateRef<FramebufferOpenGL>(spec);
    }

    Ref<Pipeline> GraphicsDeviceOpenGL::CreatePipeline(const PipelineDescription &description)
    {
        return CreateRef<PipelineOpenGL>(description);
    }

    Ref<CommandList> GraphicsDeviceOpenGL::CreateCommandList()
    {
        return CreateRef<CommandListOpenGL>(this);
    }

    void GraphicsDeviceOpenGL::InitialiseImGui()
    {
        ImGui_ImplOpenGL3_Init(m_GlslVersion);
    }

    void GraphicsDeviceOpenGL::BeginImGuiRender()
    {
        ImGui_ImplOpenGL3_NewFrame();
    }

    void GraphicsDeviceOpenGL::EndImGuiRender()
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GraphicsDeviceOpenGL::Resize(Point<int> size)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, size.X, size.Y);

        if (m_BoundFramebuffer)
            m_BoundFramebuffer->Bind();
    }

    void GraphicsDeviceOpenGL::SwapBuffers()
    {
        SDL_GL_SwapWindow(this->m_Window->GetSDLWindowHandle());
    }

    void GraphicsDeviceOpenGL::SetVSyncState(VSyncState vSyncState)
    {
        SDL_GL_SetSwapInterval((unsigned int)vSyncState);
        m_VsyncState = vSyncState;
    }

    VSyncState GraphicsDeviceOpenGL::GetVsyncState()
    {
        return m_VsyncState;
    }
}
