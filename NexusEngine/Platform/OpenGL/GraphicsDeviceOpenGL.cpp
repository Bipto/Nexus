#include "GraphicsDeviceOpenGL.h"

namespace Nexus
{
    GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(Nexus::Window* window, GraphicsAPI api)
        : GraphicsDevice(window, api)
    {
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
