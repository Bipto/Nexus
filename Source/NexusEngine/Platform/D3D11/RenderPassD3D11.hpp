#pragma once

#if defined(NX_PLATFORM_D3D11)

#include "Nexus/Graphics/RenderPass.hpp"
#include "FramebufferD3D11.hpp"

namespace Nexus::Graphics
{
    class RenderPassD3D11 : public RenderPass
    {
    public:
        RenderPassD3D11(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec);
        RenderPassD3D11(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain);

        FramebufferD3D11 *GetFramebuffer() { return m_Framebuffer; }

    private:
        FramebufferD3D11 *m_Framebuffer = nullptr;

    private:
        friend class GraphicsDeviceD3D11;
    };
}
#endif