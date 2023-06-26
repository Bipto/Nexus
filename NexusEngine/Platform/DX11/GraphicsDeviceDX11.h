#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "SDL_syswm.h"

#include "FramebufferDX11.h"

#if defined(WIN32)
#include "DX11.h"
#endif

namespace Nexus
{
    class GraphicsDeviceDX11 : public GraphicsDevice
    {
    public:
        GraphicsDeviceDX11(const GraphicsDeviceCreateInfo &createInfo);
        virtual void SetContext() override;
        virtual void Clear(float red, float green, float blue, float alpha) override;
        virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) override;
        virtual void SetPipeline(Ref<Pipeline> pipeline) override;
        virtual void DrawElements(PrimitiveType type, uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(PrimitiveType type, uint32_t count, uint32_t offset) override;
        virtual void SubmitCommandList(Ref<CommandList> commandList) override;

        virtual void SetViewport(const Viewport &viewport) override;
        virtual const Viewport &GetViewport() override;

        virtual const char *GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetShader(Ref<Shader> shader) override;

        virtual Ref<Shader> CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<Vertex> vertices) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) override;
        virtual Ref<Texture> CreateTexture(TextureSpecification spec) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const UniformResourceBinding &binding) override;
        virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification &spec) override;
        virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) override;
        virtual Ref<CommandList> CreateCommandList(Ref<Pipeline> pipeline) override;

        virtual void InitialiseImGui() override;
        virtual void BeginImGuiRender() override;
        virtual void EndImGuiRender() override;

        virtual void Resize(Point<int> size) override;
        virtual void SwapBuffers() override;
        virtual void SetVSyncState(VSyncState vSyncState) override;
        virtual VSyncState GetVsyncState() override;

        virtual ShaderFormat GetSupportedShaderFormat() override { return ShaderFormat::HLSL; }
        virtual float GetUVCorrection() { return -1.0f; }

#if defined(NX_PLATFORM_DX11)

        ID3D11Device *
        GetDevice()
        {
            return m_DevicePtr;
        }
        ID3D11DeviceContext *GetDeviceContext() { return m_DeviceContextPtr; }
        std::vector<ID3D11RenderTargetView *> &GetActiveRenderTargetViews() { return m_ActiveRenderTargetviews; }
        ID3D11DepthStencilView *GetActiveDepthStencilView() { return m_ActiveDepthStencilView; }

    private:
        ID3D11Device *m_DevicePtr = NULL;
        ID3D11DeviceContext *m_DeviceContextPtr = NULL;
        IDXGISwapChain *m_SwapChainPtr = NULL;
        ID3D11RenderTargetView *m_RenderTargetViewPtr = NULL;
        std::vector<ID3D11RenderTargetView *> m_ActiveRenderTargetviews;
        ID3D11DepthStencilView *m_ActiveDepthStencilView = NULL;
        unsigned int m_VsyncValue = 1;
        bool m_Initialised = false;
        Ref<Shader> m_ActiveShader = NULL;
        std::string m_AdapterName;

        ID3D11Texture2D *m_SwapchainDepthTexture = NULL;
        ID3D11DepthStencilView *m_SwapchainStencilView = NULL;
#endif
        VSyncState m_VsyncState = VSyncState::Enabled;
    };
}