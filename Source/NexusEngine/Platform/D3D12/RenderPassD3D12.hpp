#pragma once

#include "Nexus/Graphics/RenderPass.hpp"
#include "FramebufferD3D12.hpp"

namespace Nexus::Graphics
{
    class RenderPassD3D12 : public RenderPass
    {
    public:
        RenderPassD3D12(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec);
        RenderPassD3D12(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain);

    private:
        FramebufferD3D12 *m_Framebuffer = nullptr;

        friend class GraphicsDeviceD3D12;
        friend class CommandListD3D12;
    };
}