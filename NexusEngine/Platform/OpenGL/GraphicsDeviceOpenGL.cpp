#include "GraphicsDeviceOpenGL.h"

#include "backends/imgui_impl_opengl3.h"

namespace Nexus
{
    GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(Nexus::Window* window, GraphicsAPI api)
        : GraphicsDevice(window, api)
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
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
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
    }

    GraphicsDeviceOpenGL::~GraphicsDeviceOpenGL()
    {
        SDL_GL_DeleteContext(m_Context);
    }

    void GraphicsDeviceOpenGL::SetContext()
    {
        SDL_GL_MakeCurrent(this->m_Window->GetSDLWindowHandle(), this->m_Context);
    }

    void GraphicsDeviceOpenGL::Clear(float red, float green, float blue, float alpha)
    {
        glClearColor(red, green, blue, alpha);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void GraphicsDeviceOpenGL::DrawElements(Ref<VertexBuffer> vertexBuffer, Ref<Shader> shader)
    {
        shader->Bind();

        Ref<VertexBufferOpenGL> vb = std::dynamic_pointer_cast<VertexBufferOpenGL>(vertexBuffer);
        vb->Bind();
        glDrawArrays(GL_TRIANGLES, 0, vertexBuffer->GetVertexCount());
    }

    void GraphicsDeviceOpenGL::DrawIndexed(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, Ref<Shader> shader)
    {
        shader->Bind();

        Ref<VertexBufferOpenGL> vb = std::dynamic_pointer_cast<VertexBufferOpenGL>(vertexBuffer);
        Ref<IndexBufferOpenGL> ib = std::dynamic_pointer_cast<IndexBufferOpenGL>(indexBuffer);

        vb->Bind();
        ib->Bind();

        glDrawElements(GL_TRIANGLES, ib->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    }

    const char* GraphicsDeviceOpenGL::GetAPIName()
    {
        return (const char*)glGetString(GL_VERSION);
    }

    const char* GraphicsDeviceOpenGL::GetDeviceName()
    {
        return (const char*)glGetString(GL_RENDERER);
    }

    void* GraphicsDeviceOpenGL::GetContext()
    {
        return (void*)this->m_Context;
    }

    Ref<Shader> GraphicsDeviceOpenGL::CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout)
    {
        return CreateRef<ShaderOpenGL>(vertexShaderSource, fragmentShaderSource, layout);
    }

    Ref<Shader> GraphicsDeviceOpenGL::CreateShaderFromFile(const std::string& filepath, const BufferLayout& layout)
    {
        return CreateRef<ShaderOpenGL>(filepath, layout);
    }

    Ref<VertexBuffer> GraphicsDeviceOpenGL::CreateVertexBuffer(const std::vector<float> vertices)
    {
        return CreateRef<VertexBufferOpenGL>(vertices);
    }

    Ref<IndexBuffer> GraphicsDeviceOpenGL::CreateIndexBuffer(const std::vector<unsigned int> indices)
    {
        return CreateRef<IndexBufferOpenGL>(indices);
    }

    Ref<Texture> GraphicsDeviceOpenGL::CreateTexture(const char* filepath)
    {
        return CreateRef<TextureOpenGL>(filepath);
    }

    Ref<Framebuffer> GraphicsDeviceOpenGL::CreateFramebuffer(const Nexus::FramebufferSpecification& spec)
    {
        return CreateRef<FramebufferOpenGL>(spec);
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

    void GraphicsDeviceOpenGL::Resize(Point size)
    {
        glViewport(0, 0, size.Width, size.Height);
    }

    void GraphicsDeviceOpenGL::SwapBuffers()
    {
        SDL_GL_SwapWindow(this->m_Window->GetSDLWindowHandle());
    }

    void GraphicsDeviceOpenGL::SetVSyncState(VSyncState vSyncState)
    {
        SDL_GL_SetSwapInterval((unsigned int)vSyncState);
    }
}
