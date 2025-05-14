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

		Ref<Swapchain> GetSwapchain()
		{
			Ref<Swapchain> *swapchain = std::get_if<Ref<Swapchain>>(&m_Target);
			if (swapchain)
			{
				return *swapchain;
			}
			else
			{
				return nullptr;
			}
		}

		Ref<Framebuffer> GetFramebuffer()
		{
			Ref<Framebuffer> *framebuffer = std::get_if<Ref<Framebuffer>>(&m_Target);

			if (framebuffer)
			{
				return *framebuffer;
			}
			else
			{
				return nullptr;
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
				auto framebuffer = GetFramebuffer();
				return framebuffer->GetColorTextureCount();
			}
		}

		Nexus::Point2D<uint32_t> GetSize()
		{
			if (m_RenderTargetType == RenderTargetType::Swapchain)
			{
				auto swapchain = GetSwapchain();
				return swapchain->GetSize();
			}
			else
			{
				auto		framebuffer		= GetFramebuffer();
				const auto &framebufferSpec = framebuffer->GetFramebufferSpecification();
				return {framebufferSpec.Width, framebufferSpec.Height};
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
				auto		framebuffer		= GetFramebuffer();
				const auto &framebufferSpec = framebuffer->GetFramebufferSpecification();
				return framebufferSpec.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid;
			}
		}

		constexpr bool operator==(const RenderTarget &other)
		{
			return m_Target == other.m_Target;
		}

	  private:
		std::variant<Ref<Swapchain>, Ref<Framebuffer>> m_Target;
		RenderTargetType							m_RenderTargetType = {};
	};
}	 // namespace Nexus::Graphics