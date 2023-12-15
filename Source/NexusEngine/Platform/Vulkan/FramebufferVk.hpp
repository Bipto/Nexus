#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "Nexus/Graphics/Framebuffer.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class FramebufferVk : public Framebuffer
    {
    public:
        FramebufferVk(RenderPass *renderPass, GraphicsDeviceVk *device);
        FramebufferVk(const FramebufferSpecification &spec, GraphicsDeviceVk *device);
        ~FramebufferVk();

        virtual void *GetColorAttachment(int index = 0) override;
        virtual const FramebufferSpecification GetFramebufferSpecification() override;
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) override;
        virtual void *GetDepthAttachment() override;
        VkFramebuffer GetVkFramebuffer();

        VkImageView GetColorTextureImageView(uint32_t index);
        VkSampler GetColorTextureSampler(uint32_t index);

        VkRenderPass GetRenderPass();

    private:
        virtual void Recreate() override;

        void CreateColorTargets();
        void CreateDepthTargets();
        void CreateFramebuffer();
        void CreateRenderPass();

    private:
        std::vector<VkImage> m_Images;
        std::vector<VkDeviceMemory> m_ImageMemory;
        std::vector<VkSampler> m_Samplers;
        std::vector<VkImageView> m_ImageViews;

        VkImage m_DepthImage;
        VkDeviceMemory m_DepthMemory;
        VkImageView m_DepthImageView;

        VkFramebuffer m_Framebuffer;

        GraphicsDeviceVk *m_Device;
        RenderPassVk *m_RenderPass;

        VkRenderPass m_FramebufferRenderPass;
    };
}

#endif