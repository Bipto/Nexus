#pragma once

#include "Framebuffer.hpp"
#include "Swapchain.hpp"

#include <variant>

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
        RenderTarget(Swapchain *swapchain)
        {
            m_Target = swapchain;
            m_RenderTargetType = RenderTargetType::Swapchain;
        }

        RenderTarget(Framebuffer *framebuffer)
        {
            m_Target = framebuffer;
            m_RenderTargetType = RenderTargetType::Framebuffer;
        }

        RenderTarget()
        {
            m_RenderTargetType = RenderTargetType::None;
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
                auto framebuffer = GetData<Framebuffer *>();
                return framebuffer->GetColorTextureCount();
            }
            else
            {
                throw std::runtime_error("An invalid render target was selected");
            }
        }

        constexpr bool operator==(const RenderTarget &other)
        {
            return m_Target == other.m_Target;
        }

    private:
        std::variant<Swapchain *, Framebuffer *> m_Target;
        RenderTargetType m_RenderTargetType;
    };
}