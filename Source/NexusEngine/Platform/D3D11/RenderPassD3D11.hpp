#pragma once

#include "Nexus/Graphics/RenderPass.hpp"

namespace Nexus::Graphics
{
    class RenderPassD3D11 : public RenderPass
    {
    public:
        RenderPassD3D11(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec);
        RenderPassD3D11(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain);
        virtual LoadOperation GetColorLoadOperation() override;
        virtual LoadOperation GetDepthStencilLoadOperation() override;
        virtual const RenderPassSpecification &GetRenderPassSpecification() override;
        virtual const RenderPassData &GetRenderPassData() override;
        virtual RenderPassDataType GetRenderPassDataType() override;

    private:
        RenderPassSpecification m_RenderPassSpecification;
        RenderPassData m_Data;
        RenderPassDataType m_DataType;
        Framebuffer *m_Framebuffer;
        friend class GraphicsDeviceD3D11;
        friend class CommandListD3D11;
    };
}