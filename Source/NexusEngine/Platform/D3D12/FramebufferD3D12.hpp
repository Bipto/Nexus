#pragma once

#include "Nexus/Graphics/Framebuffer.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "D3D12Include.hpp"

namespace Nexus::Graphics
{
    class FramebufferD3D12 : public Framebuffer
    {
    public:
        FramebufferD3D12(GraphicsDeviceD3D12 *device, RenderPass *renderPass);
        virtual ~FramebufferD3D12();
        virtual void *GetColorAttachment(int index = 0) override;
        virtual void *GetDepthAttachment() override;
        virtual const FramebufferSpecification GetFramebufferSpecification() override;
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) override;

        const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &GetColorAttachmentHandles();
        D3D12_CPU_DESCRIPTOR_HANDLE GetDepthAttachmentHandle();

        DXGI_FORMAT GetColorAttachmentFormat(uint32_t index);

    private:
        void Recreate();

    private:
        GraphicsDeviceD3D12 *m_Device = nullptr;

        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource2>> m_ColorTextures;
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_DepthTexture = nullptr;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ColorDescriptorHeap = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DepthDescriptorHeap = nullptr;

        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_ColorAttachmentCPUHandles;
        D3D12_CPU_DESCRIPTOR_HANDLE m_DepthAttachmentCPUHandle;
    };
}