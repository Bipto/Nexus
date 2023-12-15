#pragma once

#include "Framebuffer.hpp"
#include "Swapchain.hpp"

#include <variant>

namespace Nexus::Graphics
{
    enum class RenderTargetType
    {
        Swapchain,
        Framebuffer
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

        template <typename T>
        T GetData()
        {
            return std::get<T>(m_Target);
        }

        RenderTargetType GetType()
        {
            return m_RenderTargetType;
        }

    private:
        std::variant<Swapchain *, Framebuffer *> m_Target;
        RenderTargetType m_RenderTargetType;
    };
}