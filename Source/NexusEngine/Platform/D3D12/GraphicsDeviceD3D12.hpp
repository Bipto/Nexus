#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "D3D12Include.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceD3D12 : public GraphicsDevice
    {
    public:
        GraphicsDeviceD3D12(const GraphicsDeviceCreateInfo &createInfo, Window *window);
        ~GraphicsDeviceD3D12();

        virtual void SetContext() override;
        void SetFramebuffer(Framebuffer *framebuffer);
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

        IDXGIFactory7 *GetDXGIFactory();
        ID3D12CommandQueue *GetCommandQueue();

        void SignalAndWait();

    private:
        void InitCommandList();
        void DispatchCommandList();

    private:
#if defined(_DEBUG)
        ID3D12Debug6 *m_D3D12Debug = nullptr;
        IDXGIDebug1 *m_DXGIDebug = nullptr;
#endif

        ID3D12Device10 *m_Device = nullptr;
        ID3D12CommandQueue *m_CommandQueue = nullptr;

        ID3D12Fence1 *m_Fence = nullptr;
        uint64_t m_FenceValue = 0;
        HANDLE m_FenceEvent = nullptr;

        ID3D12CommandAllocator *m_CommandAllocator = nullptr;
        ID3D12GraphicsCommandList7 *m_CommandList = nullptr;

        IDXGIFactory7 *m_DxgiFactory = nullptr;
    };
}
#endif