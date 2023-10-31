#include "RenderPassD3D12.hpp"

namespace Nexus::Graphics
{
    RenderPassD3D12::RenderPassD3D12(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_Data = spec;
        m_DataType = Nexus::Graphics::RenderPassDataType::Framebuffer;
    }

    RenderPassD3D12::RenderPassD3D12(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_Data = swapchain;
        m_DataType = Nexus::Graphics::RenderPassDataType::Swapchain;
    }

    LoadOperation RenderPassD3D12::GetColorLoadOperation()
    {
        return m_RenderPassSpecification.ColorLoadOperation;
    }

    LoadOperation RenderPassD3D12::GetDepthStencilLoadOperation()
    {
        return m_RenderPassSpecification.StencilDepthLoadOperation;
    }

    const RenderPassSpecification &RenderPassD3D12::GetRenderPassSpecification()
    {
        return m_RenderPassSpecification;
    }

    const RenderPassData &RenderPassD3D12::GetRenderPassData()
    {
        return m_Data;
    }

    RenderPassDataType RenderPassD3D12::GetRenderPassDataType()
    {
        return m_DataType;
    }
}
