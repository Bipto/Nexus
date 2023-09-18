#pragma once

#if defined(NX_PLATFORM_VULKAN)

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
        VkRenderPass GetVkRenderPass();
        uint32_t GetColorAttachmentCount();

    private:
        void SetupForFramebuffer();
        void SetupForSwapchain();

    private:
        RenderPassSpecification m_RenderPassSpecification;
        VkRenderPass m_RenderPass;
        GraphicsDeviceVk *m_GraphicsDevice;
        RenderPassData m_Data;
        RenderPassDataType m_DataType;
        uint32_t m_ColorAttachmentCount;
        Framebuffer *m_Framebuffer = nullptr;
        friend class CommandListVk;
        friend class FramebufferVk;
    };
}

#endif