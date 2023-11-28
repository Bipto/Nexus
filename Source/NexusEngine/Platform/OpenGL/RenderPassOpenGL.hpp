#pragma once

#include "Nexus/Graphics/RenderPass.hpp"
#include "FramebufferOpenGL.hpp"

namespace Nexus::Graphics
{
    class RenderPassOpenGL : public RenderPass
    {
    public:
        RenderPassOpenGL(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec);
        RenderPassOpenGL(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain);

    private:
        FramebufferOpenGL *m_Framebuffer = nullptr;

        friend class GraphicsDeviceOpenGL;
        friend class CommandListOpenGL;
    };
}