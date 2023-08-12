#include "RenderPassDX11.hpp"

namespace Nexus::Graphics
{
    RenderPassDX11::RenderPassDX11(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_Data = spec;
        m_DataType = Nexus::Graphics::RenderPassDataType::Framebuffer;
    }

    RenderPassDX11::RenderPassDX11(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_Data = swapchain;
        m_DataType = Nexus::Graphics::RenderPassDataType::Swapchain;
    }

    LoadOperation RenderPassDX11::GetColorLoadOperation()
    {
        return m_RenderPassSpecification.ColorLoadOperation;
    }

    LoadOperation RenderPassDX11::GetDepthStencilLoadOperation()
    {
        return m_RenderPassSpecification.StencilDepthLoadOperation;
    }

    const RenderPassSpecification &RenderPassDX11::GetRenderPassSpecification()
    {
        return m_RenderPassSpecification;
    }

    const RenderPassData &RenderPassDX11::GetRenderPassData()
    {
        return m_Data;
    }

    RenderPassDataType RenderPassDX11::GetRenderPassDataType()
    {
        return m_DataType;
    }
}