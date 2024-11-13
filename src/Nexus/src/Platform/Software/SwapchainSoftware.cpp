#include "SwapchainSoftware.hpp"

namespace Nexus::Graphics
{
	SwapchainSoftware::SwapchainSoftware(Window *window, const SwapchainSpecification &swapchainSpec, GraphicsDevice *graphicsDevice)
		: Swapchain(swapchainSpec),
		  m_Window(window)
	{
		GetScreenSurface();
		m_Pixels = new uint8_t[m_ScreenSurface->w * m_ScreenSurface->h * 4];

		m_ResizeEvent = window->OnResize.Bind(
			[&](const WindowResizedEventArgs &args)
			{
				GetScreenSurface();
				delete[] m_Pixels;
				m_Pixels = new uint8_t[m_ScreenSurface->w * m_ScreenSurface->h * 4];
			});
	}

	SwapchainSoftware::~SwapchainSoftware()
	{
		delete[] m_Pixels;
		m_Window->OnResize.Unbind(m_ResizeEvent);
	}

	void SwapchainSoftware::Initialise()
	{
	}

	void SwapchainSoftware::SwapBuffers()
	{
		for (int y = 0; y < m_ScreenSurface->h; y++)
		{
			for (int x = 0; x < m_ScreenSurface->w; x++)
			{
				int index = (y * m_ScreenSurface->w + x) * 4;

				if (x > 100 && x < 500 && y > 100 && y < 500)
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

		uint32_t total = 0;

		for (int y = 0; y < m_ScreenSurface->h; y++)
		{
			for (int x = 0; x < m_ScreenSurface->w; x++)
			{
				int		 index = (y * m_ScreenSurface->w + x) * 4;
				uint32_t col   = SDL_MapRGBA(m_ScreenSurface->format, m_Pixels[index], m_Pixels[index + 1], m_Pixels[index + 2], m_Pixels[index + 3]);
				pixels[y * m_ScreenSurface->w + x] = col;

				total++;
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
		m_ScreenSurface = SDL_GetWindowSurface(m_Window->GetSDLWindowHandle());
	}
}	 // namespace Nexus::Graphics
