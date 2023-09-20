#pragma once

#if defined(WIN32)
#include "Nexus/Graphics/Framebuffer.hpp"
#include "DX11.hpp"

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

    class FramebufferDX11 : public Framebuffer
    {
    public:
        FramebufferDX11(ID3D11Device *&device, Ref<RenderPass> renderPass);
        ~FramebufferDX11();

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