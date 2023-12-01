#include "RenderPass.hpp"

namespace Nexus::Graphics
{
    Nexus::Graphics::RenderPass::RenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_RenderPassData = spec;
        m_RenderPassDataType = Nexus::Graphics::RenderPassDataType::Framebuffer;
    }

    RenderPass::RenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_RenderPassData = swapchain;
        m_RenderPassDataType = Nexus::Graphics::RenderPassDataType::Swapchain;

        swapchain->OnClose([&]()
                           { m_IsValid = false; });
    }

    RenderPass::~RenderPass()
    {
    }

    LoadOperation RenderPass::GetColorLoadOperation()
    {
        return m_RenderPassSpecification.ColorLoadOperation;
    }

    LoadOperation RenderPass::GetDepthStencilLoadOperation()
    {
        return m_RenderPassSpecification.StencilDepthLoadOperation;
    }

    const RenderPassSpecification &RenderPass::GetRenderPassSpecification()
    {
        return m_RenderPassSpecification;
    }

    const RenderPassData &RenderPass::GetRenderPassData()
    {
        return m_RenderPassData;
    }

    RenderPassDataType RenderPass::GetRenderPassDataType()
    {
        return m_RenderPassDataType;
    }
}
