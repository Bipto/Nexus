#pragma once

#include "Framebuffer.hpp"
#include "Nexus-Core/IWindow.hpp"
#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Swapchain.hpp"

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
		RenderTarget() = default;

		explicit RenderTarget(Ref<Swapchain> swapchain) : m_Target(swapchain), m_RenderTargetType(RenderTargetType::Swapchain)
		{
		}

		explicit RenderTarget(Ref<Framebuffer> framebuffer) : m_Target(framebuffer), m_RenderTargetType(RenderTargetType::Framebuffer)
		{
		}

		WeakRef<Swapchain> GetSwapchain()
		{
			WeakRef<Swapchain> *swapchain = std::get_if<WeakRef<Swapchain>>(&m_Target);
			if (swapchain)
			{
				return *swapchain;
			}
			else
			{
				return {};
			}
		}

		WeakRef<Framebuffer> GetFramebuffer()
		{
			WeakRef<Framebuffer> *framebuffer = std::get_if<WeakRef<Framebuffer>>(&m_Target);

			if (framebuffer)
			{
				return *framebuffer;
			}
			else
			{
				return {};
			}
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
			else
			{
				WeakRef<Framebuffer> framebuffer = GetFramebuffer();
				if (auto fb = framebuffer.lock())
				{
					return fb->GetColorTextureCount();
				}
				else
				{
					return 0;
				}
			}
		}

		Nexus::Point2D<uint32_t> GetSize()
		{
			if (m_RenderTargetType == RenderTargetType::Swapchain)
			{
				WeakRef<Swapchain> swapchain = GetSwapchain();
				if (auto sc = swapchain.lock())
				{
					return sc->GetSize();
				}
				else
				{
					return 0;
				}
			}
			else
			{
				WeakRef<Framebuffer> framebuffer = GetFramebuffer();
				if (auto fb = framebuffer.lock())
				{
					const auto &framebufferSpec = fb->GetFramebufferSpecification();
					return {framebufferSpec.Width, framebufferSpec.Height};
				}
				else
				{
					return 0;
				}
			}
		}

		bool HasDepthAttachment()
		{
			if (m_RenderTargetType == RenderTargetType::Swapchain)
			{
				return true;
			}
			else
			{
				WeakRef<Framebuffer> framebuffer = GetFramebuffer();
				if (auto fb = framebuffer.lock())
				{
					const auto &framebufferSpec = fb->GetFramebufferSpecification();
					return framebufferSpec.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid;
				}
				else
				{
					return false;
				}
			}
		}

	  private:
		std::variant<WeakRef<Swapchain>, WeakRef<Framebuffer>> m_Target;
		RenderTargetType							m_RenderTargetType = {};
	};
}	 // namespace Nexus::Graphics