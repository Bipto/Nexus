#include "RenderPassD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

namespace Nexus::Graphics
{
    RenderPassD3D12::RenderPassD3D12(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec)
        : RenderPass(renderPassSpecification, spec)
    {
    }

    RenderPassD3D12::RenderPassD3D12(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
        : RenderPass(renderPassSpecification, swapchain)
    {
    }
}

#endif