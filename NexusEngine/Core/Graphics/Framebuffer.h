#pragma once

#include <vector>
#include <initializer_list>
#include <array>

#include "Core/Graphics/TextureFormat.h"
#include "Core/Graphics/DepthFormat.h"

namespace Nexus
{
    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(TextureFormat format) 
            : TextureFormat(format) {}

        TextureFormat TextureFormat;
    };

    struct FramebufferColorAttachmentSpecification
    {
        FramebufferColorAttachmentSpecification() = default;
        FramebufferColorAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
            : Attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> Attachments;
    };

    struct FramebufferDepthAttachmentSpecification
    {
        FramebufferDepthAttachmentSpecification() = default;
        FramebufferDepthAttachmentSpecification(DepthFormat format)
            : DepthFormat(format) {}

        DepthFormat DepthFormat = DepthFormat::None;
    };

    struct FramebufferSpecification
    {
        int Width = 1280;
        int Height = 720;
        FramebufferColorAttachmentSpecification ColorAttachmentSpecification;
        FramebufferDepthAttachmentSpecification DepthAttachmentSpecification;
    };

    class Framebuffer
    {
        public:
            virtual ~Framebuffer() {};
            virtual void Resize() = 0;

            virtual int GetColorTextureCount() = 0;
            virtual bool HasColorTexture() = 0;
            virtual bool HasDepthTexture() = 0;

            virtual void* GetColorAttachment(int index = 0) = 0;

            virtual const FramebufferSpecification GetFramebufferSpecification() = 0;
            virtual void SetFramebufferSpecification(const FramebufferSpecification& spec) = 0;
    };
}