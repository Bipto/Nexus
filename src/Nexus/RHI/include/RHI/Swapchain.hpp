#pragma once

#include <expected>

#include "RHI/Framebuffer.hpp"
#include "RHI/PixelFormat.hpp"
#include "RHI/Structures.hpp"
#include "RHI/SwapchainDescription.hpp"

namespace Nexus::Graphics
{
	class ISwapchain
	{
	  public:
		ISwapchain(const SwapchainDescription &spec) : m_Description(spec)
		{
		}

		virtual ~ISwapchain()
		{
		}

		virtual void							 SetPresentMode(PresentMode presentMode)					 = 0;
		virtual std::pair<uint32_t, uint32_t>	 GetSize()													 = 0;
		virtual PixelFormat						 GetColourFormat()											 = 0;
		virtual PixelFormat						 GetDepthFormat()											 = 0;
		virtual void							 SwapBuffers(const SwapchainPresentDescription &presentDesc) = 0;
		virtual FramebufferHandle				 GetCurrentFramebuffer()									 = 0;
		virtual std::expected<void, std::string> Resize(uint32_t width, uint32_t height)					 = 0;

		const SwapchainDescription &GetDescription()
		{
			return m_Description;
		}

	  protected:
		SwapchainDescription m_Description;

	  private:
		friend class IGraphicsDevice;
	};
}	 // namespace Nexus::Graphics