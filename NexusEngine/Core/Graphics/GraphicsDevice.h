#pragma once

#include "Core/Window.h"
#include "Core/Memory.h"
#include "SDL.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Core/Graphics/ShaderGenerator.h"

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
            virtual void DrawElements(Ref<VertexBuffer> vertexBuffer, Ref<Shader> shader) = 0;
            virtual void DrawIndexed(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, Ref<Shader> shader) = 0;
            virtual const char* GetAPIName() = 0;
            virtual const char* GetDeviceName() = 0;

            virtual void InitialiseImGui() = 0;
            virtual void BeginImGuiRender() = 0;
            virtual void EndImGuiRender() = 0;

            virtual void* GetContext() = 0;

            GraphicsAPI GetGraphicsAPI(){return this->m_API;}
            //Swapchain* GetSwapchain(){return this->m_Swapchain;}

            virtual Ref<Shader> CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout) = 0;
            virtual Ref<Shader> CreateShaderFromFile(const std::string& filepath, const BufferLayout& layout) = 0;
            virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<float> vertices) = 0;
            virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) = 0;

            virtual Ref<Texture> CreateTexture(TextureSpecification spec) = 0;
            Ref<Texture> CreateTexture(const char* filepath);

            virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification& spec) = 0;

            virtual void Resize(Point size) = 0;
            virtual void SwapBuffers() = 0;
            virtual void SetVSyncState(VSyncState vSyncState) = 0;

            virtual ShaderFormat GetSupportedShaderFormat() = 0;
            Ref<Shader> CreateShaderFromSpirvFile(const std::string& filepath, const BufferLayout& layout);
        protected:
            Nexus::Window* m_Window;
            GraphicsAPI m_API;
    };
}