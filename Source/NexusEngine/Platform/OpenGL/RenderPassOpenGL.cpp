#include "RenderPassOpenGL.hpp"

namespace Nexus::Graphics
{
    RenderPassOpenGL::RenderPassOpenGL(const RenderPassSpecification &renderPassSpecification)
    {
        m_RenderPassSpecification = renderPassSpecification;
    }
    LoadOperation RenderPassOpenGL::GetColorLoadOperation()
    {
        return m_RenderPassSpecification.ColorLoadOperation;
    }
    LoadOperation RenderPassOpenGL::GetDepthStencilLoadOperation()
    {
        return m_RenderPassSpecification.StencilDepthLoadOperation;
    }

    const RenderPassSpecification &RenderPassOpenGL::GetRenderPassSpecification()
    {
        return m_RenderPassSpecification;
    }
}
