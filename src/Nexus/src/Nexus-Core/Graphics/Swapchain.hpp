#pragma once

#include "Nexus-Core/ApplicationSpecification.hpp"
#include "Nexus-Core/nxpch.hpp"

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
		Swapchain(const SwapchainSpecification &spec) : m_Specification(spec)
		{
		}
		virtual ~Swapchain()
		{
		}

		virtual void					 Initialise()						  = 0;
		virtual void					 SwapBuffers()						  = 0;
		virtual VSyncState				 GetVsyncState()					  = 0;
		virtual void					 SetVSyncState(VSyncState vsyncState) = 0;
		virtual IWindow					*GetWindow()						  = 0;
		virtual void					 Prepare()							  = 0;
		virtual Nexus::Point2D<uint32_t> GetSize()							  = 0;

		const SwapchainSpecification &GetSpecification()
		{
			return m_Specification;
		}

	  protected:
		SwapchainSpecification m_Specification;

	  private:
		friend class GraphicsDevice;
	};
}	 // namespace Nexus::Graphics