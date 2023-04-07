#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "SDL_syswm.h"
#include "DX11.h"

#include "BufferDX11.h"
#include "ShaderDX11.h"

namespace Nexus
{
    class GraphicsDeviceDX11 : public GraphicsDevice
    {
        public:
            GraphicsDeviceDX11(Nexus::Window* window, GraphicsAPI api);
            virtual void SetContext() override;
            virtual void Clear(float red, float green, float blue, float alpha) override;
            virtual void DrawElements(Ref<VertexBuffer> vertexBuffer, Ref<Shader> shader) override;
            virtual void DrawIndexed(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, Ref<Shader> shader) override;
            virtual const char* GetAPIName() override;

            virtual const char* GetDeviceName() override;
            virtual void* GetContext() override;

            virtual Ref<Shader> CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout) override;            
            virtual Ref<Shader> CreateShaderFromFile(const std::string& filepath, const BufferLayout& layout) override;  
            virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<float> vertices) override;
            virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) override;
            virtual Ref<Texture> CreateTexture(const char* filepath) override;            
            virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification& spec);

            virtual void Resize(Point size) override;
            virtual void SwapBuffers() override;
            virtual void SetVSyncState(VSyncState vSyncState) override;

        #if defined(WIN32)
        private:
            ID3D11Device* m_DevicePtr                       = NULL;
            ID3D11DeviceContext* m_DeviceContextPtr         = NULL;
            IDXGISwapChain* m_SwapChainPtr                  = NULL;
            ID3D11RenderTargetView* m_RenderTargetViewPtr   = NULL;
            unsigned int m_VsyncValue                       = 1;
        #endif
    };
}