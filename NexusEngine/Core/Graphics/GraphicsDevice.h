#pragma once

#include "../Window.h"
#include "SDL.h"
#include "Swapchain.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"

namespace Nexus
{
    enum class GraphicsAPI
    {
        None,
        OpenGL,
        DirectX11
    };

    class GraphicsDevice
    {
        public:
        class ResourceFactory
        {
            public:
                virtual void Print() = 0;
            protected:
                ResourceFactory(GraphicsDevice* device)
                {
                    this->m_GraphicsDevice = device;
                }
            protected:
                GraphicsDevice* m_GraphicsDevice;
                friend class GraphicsDevice;
        };

        public:
            GraphicsDevice(Nexus::Window* window, GraphicsAPI api)
            {
                this->m_Window = window;
                this->m_API = api;
            }
            GraphicsDevice(const GraphicsDevice&) = delete;
    
            virtual void SetContext() = 0;
            virtual void Clear(float red, float green, float blue, float alpha) = 0;
            virtual void DrawElements(unsigned int start, unsigned int count) = 0;
            virtual void DrawIndexed(unsigned int count) = 0;

            GraphicsAPI GetGraphicsAPI(){return this->m_API;}
            Swapchain* GetSwapchain(){return this->m_Swapchain;}

            virtual Shader* CreateShader(const char* vertexShaderSource, const char* fragmentShaderSource) = 0;
            virtual VertexBuffer* CreateVertexBuffer(const std::vector<Vertex> vertices) = 0;
            virtual IndexBuffer* CreateIndexBuffer(unsigned int indices[], unsigned int indexCount) = 0;
            virtual Texture* CreateTexture(const char* filepath) = 0;

            virtual ResourceFactory& GetResourceFactory() = 0;
        protected:
            Nexus::Window* m_Window;
            GraphicsAPI m_API;
            Swapchain* m_Swapchain;
    };
}