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
        bool RenderToSwapchain = false;
    };

    class RenderPass
    {
    public:
        virtual ~RenderPass() {}
        virtual LoadOperation GetColorLoadOperation() = 0;
        virtual LoadOperation GetDepthStencilLoadOperation() = 0;
        virtual const RenderPassSpecification &GetRenderPassSpecification() = 0;
    };
}