#pragma once

#include "../Window.h"
#include "SDL.h"
#include "Swapchain.h"
#include "Shader.h"
#include "VertexBuffer.h"

namespace NexusEngine
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
            GraphicsDevice(NexusEngine::Window* window, GraphicsAPI api)
            {
                this->m_Window = window;
                this->m_API = api;
            }
            GraphicsDevice(const GraphicsDevice&) = delete;
    
            virtual void SetContext() = 0;
            virtual void Clear(float red, float green, float blue, float alpha) = 0;
            virtual void DrawElements(unsigned int start, unsigned int count) = 0;

            GraphicsAPI GetGraphicsAPI(){return this->m_API;}
            Swapchain* GetSwapchain(){return this->m_Swapchain;}

            virtual Shader* CreateShader(const char* vertexShaderSource, const char* fragmentShaderSource) = 0;
            virtual VertexBuffer* CreateVertexBuffer(float vertices[], unsigned int size, unsigned int vertexStride, unsigned int vertexOffset, unsigned int vertexCount) = 0;
            
        protected:
            NexusEngine::Window* m_Window;
            GraphicsAPI m_API;
            Swapchain* m_Swapchain;
    };
}