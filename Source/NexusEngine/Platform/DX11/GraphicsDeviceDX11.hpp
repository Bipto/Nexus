#pragma once

#include "Core/Graphics/GraphicsDevice.hpp"
#include "FramebufferDX11.hpp"
#include "SwapchainDX11.hpp"
#include "DX11.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceDX11 : public GraphicsDevice
    {
    public:
        GraphicsDeviceDX11(const GraphicsDeviceCreateInfo &createInfo, Window *window);
        virtual void SetContext() override;
        void SetFramebuffer(Ref<Framebuffer> framebuffer);
        virtual void SubmitCommandList(Ref<CommandList> commandList) override;

        virtual void SetViewport(const Viewport &viewport) override;

        virtual const std::string GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        virtual Ref<Shader> CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Ref<Texture> CreateTexture(TextureSpecification spec) override;
        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) override;
        virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) override;
        virtual Ref<CommandList> CreateCommandList() override;

        virtual Ref<VertexBuffer> CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const BufferDescription &description, const void *data) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) override;
        virtual Ref<Graphics::RenderPass> CreateRenderPass(const RenderPassSpecification &renderPassSpecification) override;

        virtual void Resize(Point<int> size) override;
        virtual ShaderLanguage GetSupportedShaderFormat() override { return ShaderLanguage::HLSL; }
        virtual float GetUVCorrection() { return -1.0f; }

        virtual Swapchain *GetSwapchain() override;

#if defined(NX_PLATFORM_DX11)

        ID3D11Device *GetDevice()
        {
            return m_DevicePtr;
        }
        ID3D11DeviceContext *GetDeviceContext();
        std::vector<ID3D11RenderTargetView *> &GetActiveRenderTargetViews();
        ID3D11DepthStencilView *&GetActiveDepthStencilView();
#endif

    private:
#if defined(NX_PLATFORM_DX11)
        ID3D11Device *m_DevicePtr = NULL;
        ID3D11DeviceContext *m_DeviceContextPtr = NULL;
        std::vector<ID3D11RenderTargetView *> m_ActiveRenderTargetviews;
        ID3D11DepthStencilView *m_ActiveDepthStencilView = NULL;
        bool m_Initialised = false;
        Ref<Shader> m_ActiveShader = NULL;
        std::string m_AdapterName;
        SwapchainDX11 *m_Swapchain;
#endif
        VSyncState m_VsyncState = VSyncState::Enabled;
    };
}