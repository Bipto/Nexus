#include "RenderPassD3D11.hpp"

namespace Nexus::Graphics
{
    RenderPassD3D11::RenderPassD3D11(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec)
        : RenderPass(renderPassSpecification, spec)
    {
    }

    RenderPassD3D11::RenderPassD3D11(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
        : RenderPass(renderPassSpecification, swapchain)
    {
    }
}