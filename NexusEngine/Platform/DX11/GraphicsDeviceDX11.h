#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "SDL_syswm.h"
#include "DX11.h"

#include "BufferDX11.h"
#include "ShaderDX11.h"
#include "TextureDX11.h"
#include "FramebufferDX11.h"

namespace Nexus
{
    class GraphicsDeviceDX11 : public GraphicsDevice
    {
        public:
            GraphicsDeviceDX11(Nexus::Window* window, GraphicsAPI api, Viewport viewport);
            virtual void SetContext() override;
            virtual void Clear(float red, float green, float blue, float alpha) override;
            virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) override;
            virtual void DrawElements(Ref<VertexBuffer> vertexBuffer, Ref<Shader> shader) override;
            virtual void DrawIndexed(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, Ref<Shader> shader) override;

            virtual void SetViewport(const Viewport& viewport) override;
            virtual const Viewport& GetViewport() override;

            virtual const char* GetAPIName() override;
            virtual const char* GetDeviceName() override;
            virtual void* GetContext() override;

            virtual Ref<Shader> CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout) override;            
            virtual Ref<Shader> CreateShaderFromFile(const std::string& filepath, const BufferLayout& layout) override;  
            virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<float> vertices) override;
            virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) override;
            virtual Ref<Texture> CreateTexture(TextureSpecification spec) override;            
            virtual Ref<UniformBuffer> CreateUniformBuffer(uint32_t size, uint32_t binding) override;
            virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification& spec);

            virtual void InitialiseImGui() override;
            virtual void BeginImGuiRender() override;
            virtual void EndImGuiRender() override;

            virtual void Resize(Point size) override;
            virtual void SwapBuffers() override;
            virtual void SetVSyncState(VSyncState vSyncState) override;

            virtual ShaderFormat GetSupportedShaderFormat() { return ShaderFormat::HLSL; }


        #if defined(WIN32)
        private:
            ID3D11Device* m_DevicePtr                                       = NULL;
            ID3D11DeviceContext* m_DeviceContextPtr                         = NULL;
            IDXGISwapChain* m_SwapChainPtr                                  = NULL;
            ID3D11RenderTargetView* m_RenderTargetViewPtr                   = NULL;
            std::vector<ID3D11RenderTargetView*> m_ActiveRenderTargetviews;
            unsigned int m_VsyncValue                                       = 1;
            bool m_Initialised                                              = false;
            std::string m_AdapterName;
        #endif
    };
}