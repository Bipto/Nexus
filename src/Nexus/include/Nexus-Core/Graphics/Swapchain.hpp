#pragma once

#include "Framebuffer.hpp"
#include "Nexus-Core/ApplicationDescription.hpp"
#include "Nexus-Core/Graphics/Rectangle.hpp"
#include "Nexus-Core/Graphics/Structures.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "PixelFormat.hpp"

namespace Nexus
{
	// forward declaration
	class IWindow;
}	 // namespace Nexus

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

		virtual void						  SetPresentMode(PresentMode presentMode)					  = 0;
		virtual IWindow						 *GetWindow()												  = 0;
		virtual std::pair<uint32_t, uint32_t> GetSize()													  = 0;
		virtual PixelFormat					  GetColourFormat()											  = 0;
		virtual PixelFormat					  GetDepthFormat()											  = 0;
		virtual void						  SwapBuffers(const SwapchainPresentDescription &presentDesc) = 0;
		virtual Ref<IFramebuffer>			  GetCurrentFramebuffer()									  = 0;

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