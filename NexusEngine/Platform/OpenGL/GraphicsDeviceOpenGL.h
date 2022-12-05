#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "GL.h"
#include "SDL_opengl.h"
#include "SwapchainOpenGL.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
#include "TextureOpenGL.h"

namespace Nexus
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
        class ResourceFactoryOpenGL : public GraphicsDevice::ResourceFactory
        {
            public:
                ResourceFactoryOpenGL(GraphicsDevice* device) :
                    GraphicsDevice::ResourceFactory(device){}
            void Print()
                {
                    std::cout << "Hello" << std::endl;
                }
        };

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

                this->m_Swapchain = new SwapchainOpenGL(this->m_Window->GetSDLWindowHandle(), this->m_Window->GetWindowSize());
            }

            GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL&) = delete;

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

            virtual Shader* CreateShader(const char* vertexShaderSource, const char* fragmentShaderSource) override
            {
                return new ShaderOpenGL(vertexShaderSource, fragmentShaderSource);
            }

            virtual VertexBuffer* CreateVertexBuffer(const std::vector<Vertex> vertices) override
            {
                return new VertexBufferOpenGL(vertices);
            }

            virtual IndexBuffer* CreateIndexBuffer(unsigned int indices[], unsigned int indexCount) override
            {
                return new IndexBufferOpenGL(indices, indexCount);
            }

            virtual Texture* CreateTexture(const char* filepath) override
            {
                return new TextureOpenGL(filepath);
            }

            virtual ResourceFactory& GetResourceFactory() override 
            {
                return m_ResourceFactory;
            }

        private:
            SDL_GLContext m_Context;
            ResourceFactoryOpenGL m_ResourceFactory {this};
    };
}