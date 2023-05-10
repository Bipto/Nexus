#pragma once

#include "DX11.h"
#include "Core/Graphics/Framebuffer.h"

namespace Nexus
{
    struct FramebufferColorRenderTarget
    {
        ID3D11Texture2D* Texture                        = NULL;
        ID3D11RenderTargetView* RenderTargetView        = NULL;
        ID3D11ShaderResourceView* ShaderResourceView    = NULL;
    };

    struct FramebufferDepthRenderTarget
    {
        ID3D11Texture2D* Texture                        = NULL;
        ID3D11DepthStencilView* DepthStencilView        = NULL;
    };

    class FramebufferDX11 : public Framebuffer
    {
        public:
            FramebufferDX11(ID3D11Device* device, const Nexus::FramebufferSpecification& spec);
            ~FramebufferDX11();

            virtual void Resize() override;
            virtual int GetColorTextureCount() override;            
            virtual bool HasColorTexture() override;            
            virtual bool HasDepthTexture() override;

            virtual void* GetColorAttachment(int index = 0) override;
            virtual const FramebufferSpecification GetFramebufferSpecification() override;
            virtual void SetFramebufferSpecification(const FramebufferSpecification& spec) override;        
        
            const std::vector<FramebufferColorRenderTarget>& GetColorRenderTargets() { return m_ColorRenderTargets; }
            virtual void* GetDepthAttachment() override { return (void*)m_DepthTarget.DepthStencilView; }

            ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthTarget.DepthStencilView; }
        private:
            void CreateTextures();
            void DeleteTextures();

        private:
            std::vector<FramebufferColorRenderTarget> m_ColorRenderTargets;
            FramebufferDepthRenderTarget m_DepthTarget;

            ID3D11Device* m_Device;
            Nexus::FramebufferSpecification m_FramebufferSpecification;
    };
}