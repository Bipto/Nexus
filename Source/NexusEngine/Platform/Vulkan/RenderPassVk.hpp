#pragma once

#include "Core/Graphics/RenderPass.hpp"
#include "GraphicsDeviceVk.hpp"
#include "Vk.hpp"

namespace Nexus::Graphics
{
    class RenderPassVk : public RenderPass
    {
    public:
        RenderPassVk(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification, GraphicsDeviceVk *graphicsDevice);
        RenderPassVk(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain, GraphicsDeviceVk *graphicsDevice);
        virtual ~RenderPassVk();
        virtual LoadOperation GetColorLoadOperation() override;
        virtual LoadOperation GetDepthStencilLoadOperation() override;
        virtual const RenderPassSpecification &GetRenderPassSpecification() override;
        virtual const RenderPassData &GetRenderPassData() override;
        virtual RenderPassDataType GetRenderPassDataType() override;

    private:
        void SetupForFramebuffer();
        void SetupForSwapchain();

    private:
        RenderPassSpecification m_RenderPassSpecification;
        VkRenderPass m_RenderPass;
        GraphicsDeviceVk *m_GraphicsDevice;
        RenderPassData m_Data;
        RenderPassDataType m_DataType;
        friend class CommandListVk;
    };
}