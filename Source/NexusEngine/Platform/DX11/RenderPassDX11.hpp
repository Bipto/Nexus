#pragma once

#include "Core/Graphics/RenderPass.hpp"

namespace Nexus::Graphics
{
    class RenderPassDX11 : public RenderPass
    {
    public:
        RenderPassDX11(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec);
        RenderPassDX11(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain);
        virtual LoadOperation GetColorLoadOperation() override;
        virtual LoadOperation GetDepthStencilLoadOperation() override;
        virtual const RenderPassSpecification &GetRenderPassSpecification() override;
        virtual const RenderPassData &GetRenderPassData() override;
        virtual RenderPassDataType GetRenderPassDataType() override;

    private:
        RenderPassSpecification m_RenderPassSpecification;
        RenderPassData m_Data;
        RenderPassDataType m_DataType;
        Ref<Framebuffer> m_Framebuffer;
        friend class GraphicsDeviceDX11;
        friend class CommandListDX11;
    };
}