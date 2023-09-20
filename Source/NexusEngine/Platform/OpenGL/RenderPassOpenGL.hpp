#pragma once

#include "Nexus/Graphics/RenderPass.hpp"

namespace Nexus::Graphics
{
    class RenderPassOpenGL : public RenderPass
    {
    public:
        RenderPassOpenGL(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec);
        RenderPassOpenGL(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain);
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
        friend class GraphicsDeviceOpenGL;
        friend class CommandListOpenGL;
    };
}