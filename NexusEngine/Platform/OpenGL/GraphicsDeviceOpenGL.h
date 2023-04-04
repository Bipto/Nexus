#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "GL.h"
#include "SDL_opengl.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
#include "TextureOpenGL.h"
#include "FramebufferOpenGL.h"

namespace Nexus
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
        public:
            GraphicsDeviceOpenGL(Nexus::Window* window, GraphicsAPI api) : GraphicsDevice(window, api)
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

            GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL&) = delete;

            virtual ~GraphicsDeviceOpenGL()
            {
                SDL_GL_DeleteContext(m_Context);
            }

            void SetContext() override 
            {
                SDL_GL_MakeCurrent(this->m_Window->GetSDLWindowHandle(), this->m_Context);
            }

            void Clear(float red, float green, float blue, float alpha) override 
            {
                glClearColor(red, green, blue, alpha);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            void DrawElements(Ref<VertexBuffer> vertexBuffer, Ref<Shader> shader) override
            {
                shader->Bind();

                Ref<VertexBufferOpenGL> vb = std::dynamic_pointer_cast<VertexBufferOpenGL>(vertexBuffer);
                vb->Bind();
                glDrawArrays(GL_TRIANGLES, 0, vertexBuffer->GetVertexCount());
            }

            void DrawIndexed(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer) override
            {
                Ref<VertexBufferOpenGL> vb = std::dynamic_pointer_cast<VertexBufferOpenGL>(vertexBuffer);
                Ref<IndexBufferOpenGL> ib = std::dynamic_pointer_cast<IndexBufferOpenGL>(indexBuffer);

                vb->Bind();
                ib->Bind();

                glDrawElements(GL_TRIANGLES, ib->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
            }

            virtual const char* GetAPIName() override
            {
                return (const char*)glGetString(GL_VERSION);
            }

            virtual const char* GetDeviceName() override
            {
                return (const char*)glGetString(GL_RENDERER);
            }

            virtual void* GetContext() override
            {
                return (void*)this->m_Context;
            }

            virtual Ref<Shader> CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout) override
            {
                return CreateRef<ShaderOpenGL>(vertexShaderSource, fragmentShaderSource, layout);
            }

            virtual Ref<Shader> CreateShaderFromFile(const std::string& filepath, const BufferLayout& layout) override
            {
                return CreateRef<ShaderOpenGL>(filepath, layout);
            }

            virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<float> vertices) override
            {
                //return new VertexBufferOpenGL(vertices, layout);
                return CreateRef<VertexBufferOpenGL>(vertices);
            }

            virtual Ref<IndexBuffer> CreateIndexBuffer(unsigned int indices[], unsigned int indexCount) override
            {
                //return new IndexBufferOpenGL(indices, indexCount);
                return CreateRef<IndexBufferOpenGL>(indices, indexCount);
            }

            virtual Ref<Texture> CreateTexture(const char* filepath) override
            {
                //return new TextureOpenGL(filepath);
                return CreateRef<TextureOpenGL>(filepath);
            }

            virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification& spec) override
            {
                //return new FramebufferOpenGL(spec);
                return CreateRef<FramebufferOpenGL>(spec);
            }
            
            virtual void Resize(Point size) override
            {
                glViewport(0, 0, size.Width, size.Height);
            }

            virtual void SwapBuffers() override
            {
                SDL_GL_SwapWindow(this->m_Window->GetSDLWindowHandle());
            }

            virtual void SetVSyncState(VSyncState vSyncState) override
            {
                SDL_GL_SetSwapInterval((unsigned int)vSyncState);
            }

        private:
            SDL_GLContext m_Context;
    };
}