#pragma once

#include "Core/Memory.hpp"
#include "Framebuffer.hpp"

namespace Nexus::Graphics
{
    enum class LoadOperation
    {
        Clear,
        Load
    };

    struct RenderPassSpecification
    {
        LoadOperation ColorLoadOperation = LoadOperation::Clear;
        LoadOperation StencilDepthLoadOperation = LoadOperation::Clear;
        Ref<Framebuffer> Framebuffer;
    };

    class RenderPass
    {
    public:
        virtual LoadOperation GetColorLoadOperation() = 0;
        virtual LoadOperation GetDepthStencilLoadOperation() = 0;
        virtual const Ref<Framebuffer> &GetFramebuffer() = 0;
        virtual const RenderPassSpecification &GetSpecification() = 0;
    };
}