#pragma once

#include "DX11.h"
#include "Core/Graphics/Framebuffer.h"

namespace Nexus
{
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
            virtual void* GetDepthAttachment() override;
            virtual const FramebufferSpecification GetFramebufferSpecification() override;
            virtual void SetFramebufferSpecification(const FramebufferSpecification& spec) override;
        
            ID3D11RenderTargetView* GetRenderTargetView() { return m_RenderTargetViewMap; }
            ID3D11ShaderResourceView* GetShaderResourceView() { return m_ShaderResourceView; }
       
        private:
            ID3D11Device* m_Device;
            ID3D11Texture2D* m_RenderTargetTextureMap;
            ID3D11RenderTargetView* m_RenderTargetViewMap;
            ID3D11ShaderResourceView* m_ShaderResourceView;
            Nexus::FramebufferSpecification m_FramebufferSpecification;
    };
}