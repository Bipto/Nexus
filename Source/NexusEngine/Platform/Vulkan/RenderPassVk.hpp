#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Nexus/Graphics/RenderPass.hpp"
#include "FramebufferVk.hpp"
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

        VkRenderPass GetVkRenderPass();
        uint32_t GetColorAttachmentCount();

    private:
        void SetupForFramebuffer();
        void SetupForSwapchain();

    private:
        VkRenderPass m_RenderPass;
        GraphicsDeviceVk *m_GraphicsDevice;
        uint32_t m_ColorAttachmentCount;

        FramebufferVk *m_Framebuffer = nullptr;

        friend class CommandListVk;
        friend class FramebufferVk;
    };
}

#endif