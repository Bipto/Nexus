#pragma once

#if defined(NX_PLATFORM_D3D11)
#include "Nexus/Graphics/Framebuffer.hpp"
#include "D3D11Include.hpp"

namespace Nexus::Graphics
{
    struct FramebufferColorRenderTarget
    {
        ID3D11Texture2D *Texture = NULL;
        ID3D11RenderTargetView *RenderTargetView = NULL;
        ID3D11ShaderResourceView *ShaderResourceView = NULL;
    };

    struct FramebufferDepthRenderTarget
    {
        ID3D11Texture2D *Texture = NULL;
        ID3D11DepthStencilView *DepthStencilView = NULL;
    };

    class FramebufferD3D11 : public Framebuffer
    {
    public:
        FramebufferD3D11(const FramebufferSpecification &spec, ID3D11Device *device);
        ~FramebufferD3D11();

        virtual void *GetColorAttachment(int index = 0) override;
        virtual const FramebufferSpecification GetFramebufferSpecification() override;
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) override;

        const std::vector<FramebufferColorRenderTarget> &GetColorRenderTargets() { return m_ColorRenderTargets; }
        virtual void *GetDepthAttachment() override { return (void *)m_DepthTarget.DepthStencilView; }

        ID3D11DepthStencilView *&GetDepthStencilView() { return m_DepthTarget.DepthStencilView; }

    private:
        virtual void Recreate() override;
        void CreateTextures();
        void DeleteTextures();

    private:
        std::vector<FramebufferColorRenderTarget> m_ColorRenderTargets;
        FramebufferDepthRenderTarget m_DepthTarget;

        ID3D11Device *m_Device;
    };
}

#endif