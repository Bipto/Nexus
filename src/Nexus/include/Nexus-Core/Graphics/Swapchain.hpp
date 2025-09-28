#pragma once

#include "Nexus-Core/ApplicationSpecification.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "PixelFormat.hpp"

namespace Nexus
{
	// forward declaration
	class IWindow;
}	 // namespace Nexus

namespace Nexus::Graphics
{
	class Swapchain
	{
	  public:
		Swapchain(const SwapchainDescription &spec) : m_Description(spec)
		{
		}

		virtual ~Swapchain()
		{
		}

		virtual void					 SetPresentMode(PresentMode presentMode) = 0;
		virtual IWindow					*GetWindow()							 = 0;
		virtual Nexus::Point2D<uint32_t> GetSize()								 = 0;
		virtual PixelFormat				 GetColourFormat()						 = 0;
		virtual PixelFormat				 GetDepthFormat()						 = 0;
		virtual void					 SwapBuffers()							 = 0;

		const SwapchainDescription &GetDescription()
		{
			return m_Description;
		}

	  protected:
		SwapchainDescription m_Description;

	  private:
		friend class GraphicsDevice;
	};
}	 // namespace Nexus::Graphics