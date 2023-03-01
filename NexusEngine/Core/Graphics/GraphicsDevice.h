#pragma once

#include "Core/Window.h"
#include "SDL.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Nexus
{
    enum class GraphicsAPI
    {
        None,
        OpenGL,
        DirectX11
    };

    enum class VSyncState
    {
        Adaptive = -1,
        Disabled = 0,
        Enabled = 1
    };

    class GraphicsDevice
    {
        public:
            GraphicsDevice(Nexus::Window* window, GraphicsAPI api)
            {
                this->m_Window = window;
                this->m_API = api;
            }

            virtual ~GraphicsDevice() {}

            GraphicsDevice(const GraphicsDevice&) = delete;
    
            virtual void SetContext() = 0;
            virtual void Clear(float red, float green, float blue, float alpha) = 0;
            virtual void DrawElements(unsigned int start, unsigned int count) = 0;
            virtual void DrawIndexed(unsigned int count) = 0;

            virtual void* GetContext() = 0;

            GraphicsAPI GetGraphicsAPI(){return this->m_API;}
            //Swapchain* GetSwapchain(){return this->m_Swapchain;}

            virtual Shader* CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) = 0;
            virtual Shader* CreateShaderFromFile(const std::string& filepath) = 0;
            virtual VertexBuffer* CreateVertexBuffer(const std::vector<float> vertices, const BufferLayout& layout) = 0;
            virtual IndexBuffer* CreateIndexBuffer(unsigned int indices[], unsigned int indexCount) = 0;
            virtual Texture* CreateTexture(const char* filepath) = 0;
            virtual Framebuffer* CreateFramebuffer(const Nexus::FramebufferSpecification& spec) = 0;

            virtual void Resize(Point size) = 0;
            virtual void SwapBuffers() = 0;
            virtual void SetVSyncState(VSyncState VSyncState) = 0;
        protected:
            Nexus::Window* m_Window;
            GraphicsAPI m_API;
    };
}