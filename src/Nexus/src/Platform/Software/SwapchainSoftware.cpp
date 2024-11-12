#include "SwapchainSoftware.hpp"

namespace Nexus::Graphics
{
	SwapchainSoftware::SwapchainSoftware(Window *window, const SwapchainSpecification &swapchainSpec, GraphicsDevice *graphicsDevice)
		: Swapchain(swapchainSpec),
		  m_Window(window)
	{
		GetScreenSurface();
		m_Pixels = new uint8_t[m_SwapchainWidth * m_SwapchainHeight * 4];

		m_ResizeEvent = window->OnResize.Bind(
			[&](const WindowResizedEventArgs &args)
			{
				GetScreenSurface();
				delete[] m_Pixels;
				m_Pixels = new uint8_t[m_SwapchainWidth * m_SwapchainHeight * 4];
			});
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
		for (int y = 0; y < m_ScreenSurface->h; y++)
		{
			for (int x = 0; x < m_ScreenSurface->w * 4; x += 4)
			{
				int index = y * m_SwapchainWidth + x;

				if (x == 100)
				{
					m_Pixels[index + 0] = 255;
					m_Pixels[index + 1] = 0;
					m_Pixels[index + 2] = 0;
					m_Pixels[index + 3] = 255;
				}
				else
				{
					m_Pixels[index + 0] = 0;
					m_Pixels[index + 1] = 255;
					m_Pixels[index + 2] = 0;
					m_Pixels[index + 3] = 255;
				}
			}
		}

		SDL_LockSurface(m_ScreenSurface);

		Uint32 *pixels	   = (Uint32 *)m_ScreenSurface->pixels;
		int		pixelCount = m_ScreenSurface->w * m_ScreenSurface->h;

		for (int y = 0; y < m_ScreenSurface->h; y++)
		{
			for (int x = 0; x < m_ScreenSurface->w; x++)
			{
				int surfaceIndex	 = y * m_SwapchainWidth + x;
				int framebufferIndex = y * m_SwapchainWidth + (x * 4);

				uint8_t r = m_Pixels[framebufferIndex + 0];
				uint8_t g = m_Pixels[framebufferIndex + 1];
				uint8_t b = m_Pixels[framebufferIndex + 2];
				uint8_t a = m_Pixels[framebufferIndex + 3];

				pixels[surfaceIndex] = SDL_MapRGBA(m_ScreenSurface->format, r, g, b, a);
			}
		}

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
		m_ScreenSurface	  = SDL_GetWindowSurface(m_Window->GetSDLWindowHandle());
		m_SwapchainWidth  = m_ScreenSurface->w;
		m_SwapchainHeight = m_ScreenSurface->h;
	}
}	 // namespace Nexus::Graphics
