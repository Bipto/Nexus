#include "RenderPassOpenGL.hpp"

namespace Nexus::Graphics
{
    RenderPassOpenGL::RenderPassOpenGL(const RenderPassSpecification &spec)
    {
        m_Specification = spec;
    }
    LoadOperation RenderPassOpenGL::GetColorLoadOperation()
    {
        return m_Specification.ColorLoadOperation;
    }
    LoadOperation RenderPassOpenGL::GetDepthStencilLoadOperation()
    {
        return m_Specification.StencilDepthLoadOperation;
    }
    const Ref<Framebuffer> &RenderPassOpenGL::GetFramebuffer()
    {
        return m_Specification.Framebuffer;
    }
    const RenderPassSpecification &RenderPassOpenGL::GetSpecification()
    {
        return m_Specification;
    }
}
