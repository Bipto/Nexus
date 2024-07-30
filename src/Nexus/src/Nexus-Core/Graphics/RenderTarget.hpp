#pragma once

#include "Framebuffer.hpp"
#include "Swapchain.hpp"

#include "Nexus-Core/Window.hpp"
#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    enum class RenderTargetType
    {
        Swapchain,
        Framebuffer,
        None
    };

    class RenderTarget
    {
    public:
        RenderTarget() = default;

        explicit RenderTarget(Swapchain *swapchain)
            : m_Target(swapchain),
              m_RenderTargetType(RenderTargetType::Swapchain)
        {
        }

        explicit RenderTarget(Ref<Framebuffer> framebuffer)
            : m_Target(framebuffer),
              m_RenderTargetType(RenderTargetType::Framebuffer)
        {
        }

        template <typename T>
        T GetData()
        {
            return std::get<T>(m_Target);
        }

        RenderTargetType GetType()
        {
            return m_RenderTargetType;
        }

        uint32_t GetColorAttachmentCount()
        {
            if (m_RenderTargetType == RenderTargetType::Swapchain)
            {
                return 1;
            }
            else if (m_RenderTargetType == RenderTargetType::Framebuffer)
            {
                auto framebuffer = GetData<Ref<Framebuffer>>();
                return framebuffer->GetColorTextureCount();
            }
            else
            {
                throw std::runtime_error("An invalid render target was selected");
            }
        }

        Nexus::Point2D<uint32_t> GetSize()
        {
            if (m_RenderTargetType == RenderTargetType::Swapchain)
            {
                auto swapchain = GetData<Swapchain *>();
                return swapchain->GetWindow()->GetWindowSize();
            }
            else if (m_RenderTargetType == RenderTargetType::Framebuffer)
            {
                auto framebuffer = GetData<Ref<Framebuffer>>();
                const auto &framebufferSpec = framebuffer->GetFramebufferSpecification();
                return {framebufferSpec.Width, framebufferSpec.Height};
            }
            else
            {
                throw std::runtime_error("An invalid render target was selected");
            }
        }

        bool HasDepthAttachment()
        {
            if (m_RenderTargetType == RenderTargetType::Swapchain)
            {
                return true;
            }
            else if (m_RenderTargetType == RenderTargetType::Framebuffer)
            {
                auto framebuffer = GetData<Ref<Framebuffer>>();
                const auto &framebufferSpec = framebuffer->GetFramebufferSpecification();
                return framebufferSpec.DepthAttachmentSpecification.DepthFormat != PixelFormat::None;
            }
            else
            {
                throw std::runtime_error("An invalid render target was selected");
            }
        }

        bool IsValid()
        {
            return m_RenderTargetType != RenderTargetType::None;
        }

        constexpr bool operator==(const RenderTarget &other)
        {
            return m_Target == other.m_Target;
        }

    private:
        std::variant<Swapchain *, Ref<Framebuffer>> m_Target;
        RenderTargetType m_RenderTargetType = RenderTargetType::None;
    };
}