#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "D3D12Include.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceD3D12 : public GraphicsDevice
    {
    public:
        GraphicsDeviceD3D12(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification& swapchainSpec);
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

        virtual Framebuffer *CreateFramebuffer(const FramebufferSpecification &spec) override;

        virtual ShaderLanguage GetSupportedShaderFormat() override { return ShaderLanguage::HLSL; }
        virtual float GetUVCorrection() { return -1.0f; }

        IDXGIFactory7 *GetDXGIFactory() const;
        ID3D12CommandQueue *GetCommandQueue() const;
        ID3D12Device10 *GetDevice() const;
        ID3D12GraphicsCommandList7 *GetUploadCommandList();
        ID3D12GraphicsCommandList7 *GetImGuiCommandList();
        ID3D12DescriptorHeap *GetImGuiDescriptorHeap();

        void SignalAndWait();
        void ImmediateSubmit(std::function<void(ID3D12GraphicsCommandList7 *cmd)> &&function);

        void BeginImGuiFrame();
        void EndImGuiFrame();
        uint32_t GetNextTextureOffset();

    private:
        void InitUploadCommandList();
        void DispatchUploadCommandList();
        void CreateImGuiCommandStructures();

    private:
#if defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug6> m_D3D12Debug = nullptr;
        Microsoft::WRL::ComPtr<IDXGIDebug1> m_DXGIDebug = nullptr;
#endif

        Microsoft::WRL::ComPtr<ID3D12Device10> m_Device = nullptr;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;

        Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence = nullptr;
        uint64_t m_FenceValue = 0;
        HANDLE m_FenceEvent = nullptr;

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_UploadCommandAllocator = nullptr;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_UploadCommandList = nullptr;

        Microsoft::WRL::ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_ImGuiCommandAllocator = nullptr;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_ImGuiCommandList = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ImGuiDescriptorHeap = nullptr;

        Microsoft::WRL::ComPtr<ID3DBlob> m_ImGuiRootSignatureBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_ImGuiRootSignature = nullptr;

        // first texture slot is used by font atlas
        uint32_t m_DescriptorHandleOffset = 1;
    };
}
#endif