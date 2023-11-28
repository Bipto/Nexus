#include "RenderPassOpenGL.hpp"

namespace Nexus::Graphics
{
    RenderPassOpenGL::RenderPassOpenGL(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &spec)
        : RenderPass(renderPassSpecification, spec)
    {
    }

    RenderPassOpenGL::RenderPassOpenGL(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
        : RenderPass(renderPassSpecification, swapchain)
    {
    }
}
