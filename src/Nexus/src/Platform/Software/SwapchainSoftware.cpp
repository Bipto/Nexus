#include "SwapchainSoftware.hpp"

namespace Nexus::Graphics
{
	SwapchainSoftware::SwapchainSoftware(Window *window, const SwapchainSpecification &swapchainSpec, GraphicsDevice *graphicsDevice)
		: Swapchain(swapchainSpec),
		  m_Window(window)
	{
		GetScreenSurface();

		m_ResizeEvent = window->OnResize.Bind([&](const WindowResizedEventArgs &args) { GetScreenSurface(); });
	}

	SwapchainSoftware::~SwapchainSoftware()
	{
		m_Window->OnResize.Unbind(m_ResizeEvent);
	}

	void SwapchainSoftware::Initialise()
	{
	}

	void SwapchainSoftware::SwapBuffers()
	{
		SDL_LockSurface(m_ScreenSurface);

		Uint32 *pixels	   = (Uint32 *)m_ScreenSurface->pixels;
		int		pixelCount = m_ScreenSurface->w * m_ScreenSurface->h;

		Uint32 red = SDL_MapRGB(m_ScreenSurface->format, 255, 0, 0);
		for (int i = 0; i < pixelCount; i++) { pixels[i] = red; }

		SDL_UnlockSurface(m_ScreenSurface);

		SDL_UpdateWindowSurface(m_Window->GetSDLWindowHandle());
	}

	VSyncState SwapchainSoftware::GetVsyncState()
	{
		return VSyncState();
	}

	void SwapchainSoftware::SetVSyncState(VSyncState vsyncState)
	{
	}

	Nexus::Point2D<uint32_t> SwapchainSoftware::GetSize()
	{
		return Nexus::Point2D<uint32_t>();
	}

	void SwapchainSoftware::Prepare()
	{
	}

	void SwapchainSoftware::GetScreenSurface()
	{
		m_ScreenSurface = SDL_GetWindowSurface(m_Window->GetSDLWindowHandle());
	}
}	 // namespace Nexus::Graphics
