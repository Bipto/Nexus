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

            void DrawElements(unsigned int start, unsigned int count) override
            {
                glDrawArrays(GL_TRIANGLES, start, count);
            }

            void DrawIndexed(unsigned int count) override
            {
                glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)0);
            }

            virtual void* GetContext() override
            {
                return (void*)this->m_Context;
            }

            virtual Shader* CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) override
            {
                return new ShaderOpenGL(vertexShaderSource, fragmentShaderSource);
            }

            virtual Shader* CreateShaderFromFile(const std::string& filepath) override
            {
                return new ShaderOpenGL(filepath);
            }

            virtual VertexBuffer* CreateVertexBuffer(const std::vector<float> vertices, const BufferLayout& layout) override
            {
                return new VertexBufferOpenGL(vertices, layout);
            }

            virtual IndexBuffer* CreateIndexBuffer(unsigned int indices[], unsigned int indexCount) override
            {
                return new IndexBufferOpenGL(indices, indexCount);
            }

            virtual Texture* CreateTexture(const char* filepath) override
            {
                return new TextureOpenGL(filepath);
            }

            virtual Framebuffer* CreateFramebuffer(const Nexus::FramebufferSpecification& spec) override
            {
                return new FramebufferOpenGL(spec);
            }
            
            virtual void Resize(Point size) override
            {
                glViewport(0, 0, size.Width, size.Height);
            }

            virtual void SwapBuffers() override
            {
                SDL_GL_SwapWindow(this->m_Window->GetSDLWindowHandle());
            }

            virtual void SetVSyncState(VSyncState VSyncState) override
            {
                SDL_GL_SetSwapInterval((unsigned int)VSyncState);
            }

        private:
            SDL_GLContext m_Context;
    };
}