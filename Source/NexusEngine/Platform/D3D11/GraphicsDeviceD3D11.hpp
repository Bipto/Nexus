#pragma once

#if defined(NX_PLATFORM_D3D11)

#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "FramebufferD3D11.hpp"
#include "SwapchainD3D11.hpp"
#include "D3D11Include.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceD3D11 : public GraphicsDevice
    {
    public:
        GraphicsDeviceD3D11(const GraphicsDeviceCreateInfo &createInfo, Window *window);
        ~GraphicsDeviceD3D11();

        virtual void SetContext() override;
        void SetFramebuffer(Framebuffer *framebuffer);
        void SetSwapchain(Swapchain *swapchain);
        virtual void SubmitCommandList(CommandList *commandList) override;

        virtual const std::string GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        virtual Shader *CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Texture *CreateTexture(const TextureSpecification &spec) override;
        virtual Framebuffer *CreateFramebuffer(RenderPass *renderPass) override;
        virtual Pipeline *CreatePipeline(const PipelineDescription &description) override;
        virtual CommandList *CreateCommandList() override;

        virtual VertexBuffer *CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout) override;
        virtual IndexBuffer *CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format = IndexBufferFormat::UInt32) override;
        virtual UniformBuffer *CreateUniformBuffer(const BufferDescription &description, const void *data) override;

        virtual RenderPass *CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification) override;
        virtual RenderPass *CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain) override;
        virtual ResourceSet *CreateResourceSet(const ResourceSetSpecification &spec) override;

        virtual ShaderLanguage GetSupportedShaderFormat() override { return ShaderLanguage::HLSL; }
        virtual float GetUVCorrection() { return -1.0f; }

        ID3D11Device *GetDevice()
        {
            return m_DevicePtr;
        }
        ID3D11DeviceContext *GetDeviceContext();
        std::vector<ID3D11RenderTargetView *> &GetActiveRenderTargetViews();
        ID3D11DepthStencilView *&GetActiveDepthStencilView();

    private:
        ID3D11Device *m_DevicePtr = NULL;
        ID3D11DeviceContext *m_DeviceContextPtr = NULL;
        std::vector<ID3D11RenderTargetView *> m_ActiveRenderTargetviews;
        ID3D11DepthStencilView *m_ActiveDepthStencilView = NULL;
        bool m_Initialised = false;
        Ref<Shader> m_ActiveShader = NULL;
        std::string m_AdapterName;
        VSyncState m_VsyncState = VSyncState::Enabled;
    };
}

#endif