#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "Nexus/Graphics/Framebuffer.hpp"
#include "GraphicsDeviceVk.hpp"
#include "TextureVk.hpp"

namespace Nexus::Graphics
{
    class FramebufferVk : public Framebuffer
    {
    public:
        FramebufferVk(const FramebufferSpecification &spec, GraphicsDeviceVk *device);
        ~FramebufferVk();

        virtual const FramebufferSpecification GetFramebufferSpecification() override;
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) override;
        VkFramebuffer GetVkFramebuffer();

        virtual Texture *GetColorTexture(uint32_t index = 0) override;
        virtual Texture *GetDepthTexture() override;

        TextureVk *GetVulkanColorTexture(uint32_t index = 0);
        TextureVk *GetVulkanDepthTexture();

        VkRenderPass GetRenderPass();

    private:
        virtual void Recreate() override;

        void CreateColorTargets();
        void CreateDepthTargets();
        void CreateFramebuffer();
        void CreateRenderPass();

    private:
        VkFramebuffer m_Framebuffer;
        GraphicsDeviceVk *m_Device;
        VkRenderPass m_FramebufferRenderPass;

        std::vector<TextureVk *> m_ColorAttachments;
        TextureVk *m_DepthAttachment = nullptr;
    };
}

#endif