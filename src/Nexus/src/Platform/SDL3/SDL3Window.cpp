/* #include "SDL3Window.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

#if defined(NX_PLATFORM_OPENGL)
	#include "Platform/OpenGL/SwapchainOpenGL.hpp"
#endif

#if defined(NX_PLATFORM_VULKAN)
	#include "Platform/Vulkan/SwapchainVk.hpp"
#endif

#if defined(NX_PLATFORM_D3D12)
	#include "Platform/D3D12/SwapchainD3D12.hpp"
#endif

namespace Nexus
{
	SDL3Window::SDL3Window(const WindowSpecification &windowProps, Graphics::GraphicsAPI api, const Graphics::SwapchainSpecification &swapchainSpec)
		: m_Specification(windowProps)
	{
		uint32_t flags = GetFlags(api, windowProps, swapchainSpec);

		m_Window = SDL_CreateWindow(windowProps.Title.c_str(), windowProps.Width, windowProps.Height, flags);
		NX_ASSERT(m_Window, "Could not create window");

		m_WindowID = SDL_GetWindowID(m_Window);
		SetupTimer();
	}

	SDL3Window::~SDL3Window()
	{
		delete m_Swapchain;
		SDL_DestroyWindow(m_Window);
	}

	void SDL3Window::SetPosition(int32_t x, int32_t y)
	{
		SDL_SetWindowPosition(m_Window, x, y);
	}

	void SDL3Window::SetResizable(bool resizable)
	{
		SDL_SetWindowResizable(m_Window, resizable);
	}

	void SDL3Window::SetTitle(const std::string &title)
	{
		SDL_SetWindowTitle(m_Window, title.c_str());
	}

	void SDL3Window::SetSize(const Point2D<uint32_t> &size)
	{
#if !defined(__EMSCRIPTEN__)
		SDL_SetWindowSize(m_Window, size.X, size.Y);
#else
		SDL_SetWindowSize(m_Window, size.X * GetDisplayScale(), size.Y * GetDisplayScale());
#endif
	}

	void SDL3Window::SetMouseVisible(bool enabled)
	{
		if (enabled)
		{
			SDL_ShowCursor();
		}
		else
		{
			SDL_HideCursor();
		}
	}

	void SDL3Window::SetRelativeMouse(bool enabled)
	{
		SDL_SetRelativeMouseMode(enabled);
	}

	void SDL3Window::SetCursor(Cursor cursor)
	{
		SDL_Cursor *sdlCursor;

		switch (cursor)
		{
			case Cursor::Arrow: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW); break;
			case Cursor::IBeam: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM); break;
			case Cursor::Wait: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT); break;
			case Cursor::Crosshair: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR); break;
			case Cursor::WaitArrow: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW); break;
			case Cursor::ArrowNWSE: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE); break;
			case Cursor::ArrowNESW: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW); break;
			case Cursor::ArrowWE: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE); break;
			case Cursor::ArrowNS: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS); break;
			case Cursor::ArrowAllDir: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL); break;
			case Cursor::No: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO); break;
			case Cursor::Hand: sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND); break;
		}

		SDL_SetCursor(sdlCursor);
	}

	void SDL3Window::SetRendersPerSecond(uint32_t time)
	{
		m_Specification.RendersPerSecond = time;
		SetupTimer();
	}

	void SDL3Window::SetUpdatesPerSecond(uint32_t time)
	{
		m_Specification.UpdatesPerSecond = time;
		SetupTimer();
	}

	void SDL3Window::SetTicksPerSecond(uint32_t time)
	{
		m_Specification.TicksPerSecond = time;
		SetupTimer();
	}

	InputState *SDL3Window::GetInput()
	{
		return m_InputState;
	}

	const InputNew::InputContext &SDL3Window::GetInputContext() const
	{
		return m_InputContext;
	}

	bool SDL3Window::IsFocussed() const
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_INPUT_FOCUS;
	}

	bool SDL3Window::IsFullscreen() const
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_FULLSCREEN;
	}

	float SDL3Window::GetDPI() const
	{
		return SDL_GetWindowDisplayScale(m_Window);
	}

	Point2D<uint32_t> SDL3Window::GetSize() const
	{
		int x, y;
		SDL_GetWindowSize(m_Window, &x, &y);

		Point2D<uint32_t> size {};
		size.X = x;
		size.Y = y;

#if defined(__EMSCRIPTEN__)
		size.X *= GetDisplayScale();
		size.Y *= GetDisplayScale();
#endif

		return size;
	}

	Point2D<int> SDL3Window::GetPosition() const
	{
		Point2D<int> position {};
		SDL_GetWindowPosition(m_Window, &position.X, &position.Y);
		return position;
	}

	WindowState SDL3Window::GetState() const
	{
		Uint32 flags = SDL_GetWindowFlags(m_Window);

		if (flags & SDL_WINDOW_MAXIMIZED)
		{
			return WindowState::Maximized;
		}
		else if (flags & SDL_WINDOW_MINIMIZED)
		{
			return WindowState::Minimized;
		}
		else
		{
			return WindowState::Normal;
		}
	}

	bool SDL3Window::IsClosing() const
	{
		return m_Closing;
	}

	void SDL3Window::Close()
	{
		m_Closing = true;
	}

	WindowHandle SDL3Window::GetHandle() const
	{
		return m_Window;
	}

	void SDL3Window::Maximise()
	{
		SDL_MaximizeWindow(m_Window);
	}

	void SDL3Window::Minimise()
	{
		SDL_MinimizeWindow(m_Window);
	}

	void SDL3Window::Restore()
	{
		SDL_RestoreWindow(m_Window);
	}

	void SDL3Window::SetFullscreen(bool enabled)
	{
		SDL_SetWindowFullscreen(m_Window, enabled);
	}

	void SDL3Window::Show()
	{
		SDL_ShowWindow(m_Window);
	}

	void SDL3Window::Hide()
	{
		SDL_HideWindow(m_Window);
	}

	void SDL3Window::Focus()
	{
		SDL_RaiseWindow(m_Window);
	}

	void SDL3Window::CreateSwapchain(Graphics::GraphicsDevice *device, const Graphics::SwapchainSpecification &swapchainSpec)
	{
		switch (device->GetGraphicsAPI())
		{
#if defined(NX_PLATFORM_OPENGL)
			case Graphics::GraphicsAPI::OpenGL:
			{
				m_Swapchain = new Graphics::SwapchainOpenGL(this, swapchainSpec, device);
				break;
			}
#endif
#if defined(NX_PLATFORM_VULKAN)
			case Graphics::GraphicsAPI::Vulkan:
			{
				m_Swapchain = new Graphics::SwapchainVk(this, device, swapchainSpec);
				break;
			}
#endif
#if defined(NX_PLATFORM_D3D12)
			case Graphics::GraphicsAPI::D3D12:
			{
				m_Swapchain = new Graphics::SwapchainD3D12(this, device, swapchainSpec);
				break;
			}
#endif

			default: NX_ERROR("Failed to create swapchain"); throw std::runtime_error("Failed to create swapchain");
		}
	}

	Graphics::Swapchain *SDL3Window::GetSwapchain()
	{
		return m_Swapchain;
	}

	uint32_t SDL3Window::GetFlags(Graphics::GraphicsAPI					  api,
								  const WindowSpecification				 &windowSpec,
								  const Graphics::SwapchainSpecification &swapchainSpec)
	{
		uint32_t flags = 0;
		flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

		if (windowSpec.Resizable)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (windowSpec.Borderless)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (windowSpec.Utility)
		{
			flags |= SDL_WINDOW_UTILITY;
		}

		if (api == Graphics::GraphicsAPI::Vulkan)
		{
			flags |= SDL_WINDOW_VULKAN;
		}

		return flags;
	}

	void SDL3Window::SetupTimer()
	{
		m_ExecutionTimer.Clear();

		double secondsPerRender = {};
		double secondsPerUpdate = {};
		double secondsPerTick	= {};

		if (m_Specification.RendersPerSecond.has_value())
		{
			secondsPerRender = 1.0 / m_Specification.RendersPerSecond.value();
		}

		if (m_Specification.UpdatesPerSecond.has_value())
		{
			secondsPerUpdate = 1.0 / m_Specification.UpdatesPerSecond.value();
		}

		if (m_Specification.TicksPerSecond.has_value())
		{
			secondsPerTick = 1.0 / m_Specification.TicksPerSecond.value();
		}

		m_ExecutionTimer.Every(
		[&](Nexus::TimeSpan time)
		{
			if (GetState() == WindowState::Minimized)
				return;

			m_RenderFrameRateMonitor.Update();
			OnRender.Invoke(time);
		},
		secondsPerRender);

		m_ExecutionTimer.Every(
		[&](Nexus::TimeSpan time)
		{
			if (GetState() == WindowState::Minimized)
				return;

			m_UpdateFrameRateMonitor.Update();
			OnUpdate.Invoke(time);
		},
		secondsPerUpdate);

		m_ExecutionTimer.Every(
		[&](Nexus::TimeSpan time)
		{
			if (GetState() == WindowState::Minimized)
				return;

			m_TickFrameRateMonitor.Update();
			OnTick.Invoke(time);
		},
		secondsPerTick);
	}

}	 // namespace Nexus
 */