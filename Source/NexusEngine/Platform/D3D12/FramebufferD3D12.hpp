#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "Nexus/Graphics/Framebuffer.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "D3D12Include.hpp"
#include "TextureD3D12.hpp"

namespace Nexus::Graphics
{
    class FramebufferD3D12 : public Framebuffer
    {
    public:
        FramebufferD3D12(const FramebufferSpecification &spec, GraphicsDeviceD3D12 *device);
        virtual ~FramebufferD3D12();
        virtual const FramebufferSpecification GetFramebufferSpecification() override;
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) override;
        virtual Texture *GetColorTexture(uint32_t index = 0) override;
        virtual Texture *GetDepthTexture() override;

        TextureD3D12 *GetD3D12ColorTexture(uint32_t index = 0);
        TextureD3D12 *GetD3D12DepthTexture();

        const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &GetColorAttachmentCPUHandles();
        D3D12_CPU_DESCRIPTOR_HANDLE GetDepthAttachmentCPUHandle();

        void CreateAttachments();
        void CreateRTVs();

    private:
        void Recreate();
        void Flush();

    private:
        GraphicsDeviceD3D12 *m_Device = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ColorDescriptorHeap = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DepthDescriptorHeap = nullptr;

        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_ColorAttachmentCPUHandles;
        D3D12_CPU_DESCRIPTOR_HANDLE m_DepthAttachmentCPUHandle;

        std::vector<TextureD3D12 *> m_ColorAttachments;
        TextureD3D12 *m_DepthAttachment = nullptr;
    };
}

#endif