#pragma once

#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Window.hpp"

#include "PixelBuffer.hpp"

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
		void Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		void DrawQuad(uint32_t xPos, uint32_t yPos, uint32_t width, uint32_t height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	  private:
		Window	  *m_Window;
		VSyncState m_VsyncState;

		SDL_Surface	  *m_ScreenSurface = nullptr;
		Nexus::EventID m_ResizeEvent   = 0;

		PixelBuffer m_PixelBuffer = {};
	};
}	 // namespace Nexus::Graphics