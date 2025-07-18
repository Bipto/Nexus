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
		Swapchain(const SwapchainSpecification &spec) : m_Description(spec)
		{
		}

		virtual ~Swapchain()
		{
		}

		virtual void					 SwapBuffers()						  = 0;
		virtual VSyncState				 GetVsyncState()					  = 0;
		virtual void					 SetVSyncState(VSyncState vsyncState) = 0;
		virtual IWindow					*GetWindow()						  = 0;
		virtual void					 Prepare()							  = 0;
		virtual Nexus::Point2D<uint32_t> GetSize()							  = 0;
		virtual PixelFormat				 GetColourFormat()					  = 0;
		virtual PixelFormat				 GetDepthFormat()					  = 0;

		const SwapchainSpecification &GetDescription()
		{
			return m_Description;
		}

	  protected:
		SwapchainSpecification m_Description;

	  private:
		friend class GraphicsDevice;
	};
}	 // namespace Nexus::Graphics