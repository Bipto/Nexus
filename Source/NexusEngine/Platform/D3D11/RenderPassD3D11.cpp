#include "RenderPassD3D11.hpp"

namespace Nexus::Graphics
{
    RenderPassD3D11::RenderPassD3D11(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_Data = spec;
        m_DataType = Nexus::Graphics::RenderPassDataType::Framebuffer;
    }

    RenderPassD3D11::RenderPassD3D11(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_Data = swapchain;
        m_DataType = Nexus::Graphics::RenderPassDataType::Swapchain;
    }

    LoadOperation RenderPassD3D11::GetColorLoadOperation()
    {
        return m_RenderPassSpecification.ColorLoadOperation;
    }

    LoadOperation RenderPassD3D11::GetDepthStencilLoadOperation()
    {
        return m_RenderPassSpecification.StencilDepthLoadOperation;
    }

    const RenderPassSpecification &RenderPassD3D11::GetRenderPassSpecification()
    {
        return m_RenderPassSpecification;
    }

    const RenderPassData &RenderPassD3D11::GetRenderPassData()
    {
        return m_Data;
    }

    RenderPassDataType RenderPassD3D11::GetRenderPassDataType()
    {
        return m_DataType;
    }
}