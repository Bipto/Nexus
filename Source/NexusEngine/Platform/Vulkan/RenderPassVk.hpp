#pragma once

#include "Core/Graphics/RenderPass.hpp"
#include "GraphicsDeviceVk.hpp"
#include "Vk.hpp"

namespace Nexus::Graphics
{
    class RenderPassVk : public RenderPass
    {
    public:
        RenderPassVk(const RenderPassSpecification &renderPassSpecification, GraphicsDeviceVk *graphicsDevice);
        virtual ~RenderPassVk();
        virtual LoadOperation GetColorLoadOperation() override;
        virtual LoadOperation GetDepthStencilLoadOperation() override;
        virtual const RenderPassSpecification &GetRenderPassSpecification() override;

    private:
        void SetupForFramebuffer();
        void SetupForSwapchain();

    private:
        RenderPassSpecification m_RenderPassSpecification;
        VkRenderPass m_RenderPass;
        GraphicsDeviceVk *m_GraphicsDevice;
    };
}