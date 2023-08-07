#pragma once

#include "Core/Graphics/RenderPass.hpp"

namespace Nexus::Graphics
{
    class RenderPassOpenGL : public RenderPass
    {
    public:
        RenderPassOpenGL(const RenderPassSpecification &spec);
        virtual LoadOperation GetColorLoadOperation() override;
        virtual LoadOperation GetDepthStencilLoadOperation() override;
        virtual const Ref<Framebuffer> &GetFramebuffer() override;
        virtual const RenderPassSpecification &GetSpecification() override;

    private:
        RenderPassSpecification m_Specification;
    };
}