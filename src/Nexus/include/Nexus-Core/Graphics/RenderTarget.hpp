#pragma once

#include "Framebuffer.hpp"
#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/Window.hpp"
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

		explicit RenderTarget(Swapchain *swapchain) : m_Target(swapchain), m_RenderTargetType(RenderTargetType::Swapchain)
		{
		}

		explicit RenderTarget(Ref<Framebuffer> framebuffer) : m_Target(framebuffer), m_RenderTargetType(RenderTargetType::Framebuffer)
		{
		}

		template<typename T>
		T GetData()
		{
			return std::get<T>(m_Target);
		}

		template<typename T>
		T *GetDataIf()
		{
			return std::get_if<T>(&m_Target);
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
				auto framebuffer = GetData<Ref<Framebuffer>>();
				return framebuffer->GetColorTextureCount();
			}
		}

		Nexus::Point2D<uint32_t> GetSize()
		{
			if (m_RenderTargetType == RenderTargetType::Swapchain)
			{
				auto swapchain = GetData<Swapchain *>();
				return swapchain->GetSize();
			}
			else
			{
				auto		framebuffer		= GetData<Ref<Framebuffer>>();
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
				auto		framebuffer		= GetData<Ref<Framebuffer>>();
				const auto &framebufferSpec = framebuffer->GetFramebufferSpecification();
				return framebufferSpec.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid;
			}
		}

		constexpr bool operator==(const RenderTarget &other)
		{
			return m_Target == other.m_Target;
		}

	  private:
		std::variant<Swapchain *, Ref<Framebuffer>> m_Target;
		RenderTargetType							m_RenderTargetType = {};
	};
}	 // namespace Nexus::Graphics