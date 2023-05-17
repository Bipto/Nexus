#pragma once

#include "Core/Window.h"
#include "Core/Memory.h"
#include "SDL.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Core/Graphics/ShaderGenerator.h"
#include "Viewport.h"

namespace Nexus
{
    enum CoordinateSystem
    {
        LeftHanded,
        RightHanded
    };

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

    enum class PrimitiveType
    {
        Triangle
    };

    enum class CullMode
    {
        Clockwise,
        CounterClockwise,
        None
    };

    struct GraphicsDeviceCreateInfo
    {
        Window* GraphicsWindow;
        GraphicsAPI API;
        VSyncState VSyncStateSettings;
        Viewport GraphicsViewport;
    };

    struct ClearOptions
    {
        bool ClearColor = true;
        bool ClearDepthStencil = false;
    };

    class GraphicsDevice
    {
        public:
            GraphicsDevice(const GraphicsDeviceCreateInfo& createInfo)
            {
                m_Window = createInfo.GraphicsWindow;
                m_API = createInfo.API;
                m_Viewport = createInfo.GraphicsViewport;
            }

            virtual ~GraphicsDevice() {}

            GraphicsDevice(const GraphicsDevice&) = delete;
    
            virtual void SetContext() = 0;
            virtual void Clear(float red, float green, float blue, float alpha) = 0;
            virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) = 0;
            virtual void DrawElements(PrimitiveType type, uint32_t start, uint32_t count) = 0;
            virtual void DrawIndexed(PrimitiveType type, uint32_t count, uint32_t offset) = 0;
            virtual const char* GetAPIName() = 0;
            virtual const char* GetDeviceName() = 0;

            virtual CoordinateSystem GetCoordinateSystem() = 0;

            virtual void SetViewport(const Viewport& viewport) = 0;
            virtual const Viewport& GetViewport() = 0;

            virtual void InitialiseImGui() = 0;
            virtual void BeginImGuiRender() = 0;
            virtual void EndImGuiRender() = 0;

            virtual void* GetContext() = 0;

            GraphicsAPI GetGraphicsAPI(){return this->m_API;}

            virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;
            virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) = 0;
            virtual void SetShader(Ref<Shader> shader) = 0;

            virtual Ref<Shader> CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const VertexBufferLayout& layout) = 0;
            virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<float> vertices) = 0;
            virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) = 0;
            virtual Ref<UniformBuffer> CreateUniformBuffer(const UniformResourceBinding& binding) = 0;

            virtual Ref<Texture> CreateTexture(TextureSpecification spec) = 0;
            Ref<Texture> CreateTexture(const char* filepath);

            virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification& spec) = 0;

            virtual void Resize(Point size) = 0;

            virtual void SwapBuffers() = 0;
            virtual void SetVSyncState(VSyncState vSyncState) = 0;
            virtual VSyncState GetVsyncState() = 0;

            virtual ShaderFormat GetSupportedShaderFormat() = 0;
            Ref<Shader> CreateShaderFromSpirvFile(const std::string& filepath, const VertexBufferLayout& layout);
        protected:
            Nexus::Window* m_Window;
            GraphicsAPI m_API;
            Viewport m_Viewport;
    };
}