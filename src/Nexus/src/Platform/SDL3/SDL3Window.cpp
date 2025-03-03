#include "SDL3Window.hpp"

#include "Nexus-Core/IWindow.hpp"

#include "Nexus-Core/Input/Input.hpp"

namespace Nexus
{
	SDL3Window::SDL3Window(const WindowSpecification &windowProps) : IWindow(windowProps), m_Specification(windowProps), m_InputContext(this)
	{
		uint32_t flags = GetFlags(windowProps);

		m_Window = SDL_CreateWindow(windowProps.Title.c_str(), windowProps.Width, windowProps.Height, flags);

		if (m_Window == nullptr)
		{
			std::string errorCode = {SDL_GetError()};
			NX_ERROR(errorCode);
		}

		m_WindowID = SDL_GetWindowID(m_Window);
		SetupTimer();
	}

	SDL3Window::~SDL3Window()
	{
		SDL_DestroyWindow(this->m_Window);
	}

	void SDL3Window::CacheInput()
	{
		m_Input.CacheInput();
	}

	void SDL3Window::Update()
	{
		m_InputContext.Reset();
		m_Timer.Update();
	}

	void SDL3Window::SetResizable(bool isResizable)
	{
		SDL_SetWindowResizable(this->m_Window, isResizable);
	}

	void SDL3Window::SetTitle(const std::string &title)
	{
		SDL_SetWindowTitle(this->m_Window, title.c_str());
	}

	void SDL3Window::SetSize(Point2D<uint32_t> size)
	{
#if !defined(__EMSCRIPTEN__)
		SDL_SetWindowSize(m_Window, size.X, size.Y);
#else
		SDL_SetWindowSize(m_Window, size.X * GetDisplayScale(), size.Y * GetDisplayScale());
#endif
	}

	void SDL3Window::Close()
	{
		m_Closing = true;
	}

	bool SDL3Window::IsClosing()
	{
		return m_Closing;
	}

	SDL_Window *SDL3Window::GetSDLWindowHandle()
	{
		return m_Window;
	}

	Point2D<uint32_t> SDL3Window::GetWindowSize()
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

	Point2D<uint32_t> SDL3Window::GetWindowSizeInPixels()
	{
		int w, h;
		SDL_GetWindowSizeInPixels(m_Window, &w, &h);
		return {(uint32_t)w, (uint32_t)h};
	}

	Point2D<int> SDL3Window::GetWindowPosition()
	{
		Point2D<int> position {};
		SDL_GetWindowPosition(m_Window, &position.X, &position.Y);
		return position;
	}

	WindowState SDL3Window::GetCurrentWindowState()
	{
		Uint32 flags = SDL_GetWindowFlags(m_Window);

		if (flags & SDL_WINDOW_MAXIMIZED)
		{
			m_CurrentWindowState = WindowState::Maximized;
		}
		else if (flags & SDL_WINDOW_MINIMIZED)
		{
			m_CurrentWindowState = WindowState::Minimized;
		}
		else
		{
			m_CurrentWindowState = WindowState::Normal;
		}

		return m_CurrentWindowState;
	}

	void SDL3Window::SetIsMouseVisible(bool visible)
	{
		if (visible)
		{
			SDL_ShowCursor();
		}
		else
		{
			SDL_HideCursor();
		}
	}

	InputState *SDL3Window::GetInput()
	{
		return &m_Input;
	}

	Nexus::InputNew::InputContext *SDL3Window::GetInputContext()
	{
		return &m_InputContext;
	}

	bool SDL3Window::IsFocussed()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_INPUT_FOCUS;
	}

	bool SDL3Window::IsMinimized()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MINIMIZED;
	}

	bool SDL3Window::IsMaximized()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MAXIMIZED;
	}

	bool SDL3Window::IsFullscreen()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_FULLSCREEN;
	}

	void SDL3Window::Maximize()
	{
		SDL_MaximizeWindow(m_Window);
	}

	void SDL3Window::Minimize()
	{
		SDL_MinimizeWindow(m_Window);
	}

	void SDL3Window::Restore()
	{
		SDL_RestoreWindow(m_Window);
	}

	void SDL3Window::ToggleFullscreen()
	{
		if (IsFullscreen())
		{
			UnsetFullscreen();
		}
		else
		{
			SetFullscreen();
		}
	}

	void SDL3Window::SetFullscreen()
	{
		SDL_SetWindowFullscreen(m_Window, true);
	}

	void SDL3Window::UnsetFullscreen()
	{
		SDL_SetWindowFullscreen(m_Window, false);
	}

	void SDL3Window::Show()
	{
		SDL_ShowWindow(m_Window);
	}

	void SDL3Window::Hide()
	{
		SDL_HideWindow(m_Window);
	}

	void SDL3Window::SetWindowPosition(int32_t x, int32_t y)
	{
		SDL_SetWindowPosition(m_Window, x, y);
	}

	void SDL3Window::Focus()
	{
		SDL_RaiseWindow(m_Window);
	}

	uint32_t SDL3Window::GetID()
	{
		return m_WindowID;
	}

	float SDL3Window::GetDisplayScale()
	{
		return SDL_GetWindowDisplayScale(m_Window);
	}

	void SDL3Window::SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect)
	{
		SDL_Rect r;
		r.x = rect.GetLeft();
		r.y = rect.GetTop();
		r.w = rect.GetWidth();
		r.h = rect.GetHeight();
		SDL_SetTextInputArea(m_Window, &r, 0);
	}

	void SDL3Window::StartTextInput()
	{
		SDL_StartTextInput(m_Window);
	}

	void SDL3Window::StopTextInput()
	{
		SDL_StopTextInput(m_Window);
	}

	void SDL3Window::SetRendersPerSecond(std::optional<uint32_t> amount)
	{
		m_Specification.RendersPerSecond = amount;
		SetupTimer();
	}

	void SDL3Window::SetUpdatesPerSecond(std::optional<uint32_t> amount)
	{
		m_Specification.UpdatesPerSecond = amount;
		SetupTimer();
	}

	void SDL3Window::SetTicksPerSecond(std::optional<uint32_t> amount)
	{
		m_Specification.TicksPerSecond = amount;
		SetupTimer();
	}

	void SDL3Window::SetRenderFunction(std::function<void(Nexus::TimeSpan time)> func)
	{
		m_RenderFunc = func;
	}

	void SDL3Window::SetUpdateFunction(std::function<void(Nexus::TimeSpan time)> func)
	{
		m_UpdateFunc = func;
	}

	void SDL3Window::SetTickFunction(std::function<void(Nexus::TimeSpan time)> func)
	{
		m_TickFunc = func;
	}

	void SDL3Window::SetRelativeMouseMode(bool enabled)
	{
		SDL_SetWindowRelativeMouseMode(m_Window, enabled);
	}

	NativeWindowInfo SDL3Window::GetNativeWindowInfo()
	{
		NativeWindowInfo info = {};

		SDL_PropertiesID properties = SDL_GetWindowProperties(m_Window);

#if defined(NX_PLATFORM_WINDOWS)
		info.hwnd	  = (HWND)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
		info.hdc	  = (HDC)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HDC_POINTER, nullptr);
		info.instance = (HINSTANCE)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
#elif defined(NX_PLATFORM_LINUX)
		info.display = (Display *)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
		info.screen	 = (int)SDL_GetNumberProperty(properties, SDL_PROP_WINDOW_X11_SCREEN_NUMBER, 0);
		info.window	 = (Window)(unsigned long)SDL_GetNumberProperty(properties, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
#elif defined(NX_PLATFORM_WEB)
		info.canvasId = m_Specification.CanvasId;
#endif

		return info;
	}

	uint32_t SDL3Window::GetFlags(const WindowSpecification &windowSpec)
	{
		// required for emscripten to handle resizing correctly
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

		return flags;
	}

	void SDL3Window::SetupTimer()
	{
		m_Timer.Clear();

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

		m_Timer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				Input::SetContext(&m_InputContext);
				m_RenderFrameRateMonitor.Update();

				if (m_RenderFunc)
				{
					m_RenderFunc(time);
				}
			},
			secondsPerRender);

		m_Timer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				Input::SetContext(&m_InputContext);
				m_UpdateFrameRateMonitor.Update();

				if (m_UpdateFunc)
				{
					m_UpdateFunc(time);
				}
			},
			secondsPerUpdate);

		m_Timer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				Input::SetContext(&m_InputContext);
				m_TickFrameRateMonitor.Update();

				if (m_TickFunc)
				{
					m_TickFunc(time);
				}
			},
			secondsPerTick);
	}

	const WindowSpecification &SDL3Window::GetSpecification() const
	{
		return m_Specification;
	}

}	 // namespace Nexus
