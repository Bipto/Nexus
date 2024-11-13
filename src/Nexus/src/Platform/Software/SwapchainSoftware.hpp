#pragma once

#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Window.hpp"

#include "Platform/SDL3/SDL3Include.hpp"

namespace Nexus::Graphics
{
	class SwapchainSoftware : public Swapchain
	{
	  public:
		SwapchainSoftware(Window *window, const SwapchainSpecification &swapchainSpec, GraphicsDevice *graphicsDevice);
		virtual ~SwapchainSoftware();
		virtual void					 Initialise() override;
		virtual void					 SwapBuffers() override;
		virtual VSyncState				 GetVsyncState() override;
		virtual void					 SetVSyncState(VSyncState vsyncState) override;
		virtual Nexus::Point2D<uint32_t> GetSize() override;
		virtual Window					*GetWindow() override
		{
			return m_Window;
		}
		virtual void Prepare() override;

		void GetScreenSurface();

	  private:
		Window	  *m_Window;
		VSyncState m_VsyncState;

		SDL_Surface	  *m_ScreenSurface = nullptr;
		Nexus::EventID m_ResizeEvent   = 0;

		uint8_t *m_Pixels = nullptr;
	};
}	 // namespace Nexus::Graphics