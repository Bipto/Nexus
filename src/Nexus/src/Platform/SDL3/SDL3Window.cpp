#include "SDL3Window.hpp"

#include "Nexus-Core/IWindow.hpp"

#include "Nexus-Core/Input/Input.hpp"
#include "Nexus-Core/Platform.hpp"

namespace Nexus
{
	SDL3Window::SDL3Window(const WindowSpecification &windowProps) : IWindow(windowProps), m_Specification(windowProps)
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

	void SDL3Window::Update()
	{
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

	void SDL3Window::SetResizeCallback(std::function<void(const WindowResizedEventArgs &)> func)
	{
		m_OnResizeCallback = func;
	}

	void SDL3Window::SetMoveCallback(std::function<void(const WindowMovedEventArgs &)> func)
	{
		m_OnMoveCallback = func;
	}

	void SDL3Window::SetFocusGainCallback(std::function<void()> func)
	{
		m_OnFocusGainCallback = func;
	}

	void SDL3Window::SetFocusLostCallback(std::function<void()> func)
	{
		m_OnFocusLostCallback = func;
	}

	void SDL3Window::SetMaximizedCallback(std::function<void()> func)
	{
		m_OnMaximizeCallback = func;
	}

	void SDL3Window::SetMinimizedCallback(std::function<void()> func)
	{
		m_OnMinimizeCallback = func;
	}

	void SDL3Window::SetRestoreCallback(std::function<void()> func)
	{
		m_OnRestoreCallback = func;
	}

	void SDL3Window::SetShowCallback(std::function<void()> func)
	{
		m_OnShowCallback = func;
	}

	void SDL3Window::SetHideCallback(std::function<void()> func)
	{
		m_OnHideCallback = func;
	}

	void SDL3Window::SetExposeCallback(std::function<void()> func)
	{
		m_OnExposeCallback = func;
	}

	void SDL3Window::SetKeyPressedCallback(std::function<void(const KeyPressedEventArgs &)> func)
	{
		m_OnKeyPressedCallback = func;
	}

	void SDL3Window::SetKeyReleasedCallback(std::function<void(const KeyReleasedEventArgs &)> func)
	{
		m_OnKeyReleasedCallback = func;
	}

	void SDL3Window::SetTextInputCallback(std::function<void(const TextInputEventArgs &)> func)
	{
		m_OnTextInputCallback = func;
	}

	void SDL3Window::SetTextEditCallback(std::function<void(const TextEditEventArgs &)> func)
	{
		m_OnTextEditCallback = func;
	}

	void SDL3Window::SetMousePressedCallback(std::function<void(const MouseButtonPressedEventArgs &)> func)
	{
		m_OnMouseButtonPressedCallback = func;
	}

	void SDL3Window::SetMouseReleasedCallback(std::function<void(const MouseButtonReleasedEventArgs &)> func)
	{
		m_OnMouseButtonReleasedCallback = func;
	}

	void SDL3Window::SetMouseMovedCallback(std::function<void(const MouseMovedEventArgs &)> func)
	{
		m_OnMouseMovedCallback = func;
	}

	void SDL3Window::SetMouseScrollCallback(std::function<void(const MouseScrolledEventArgs &)> func)
	{
		m_OnMouseScrolledCallback = func;
	}

	void SDL3Window::SetMouseEnterCallback(std::function<void()> func)
	{
		m_OnMouseEnterCallback = func;
	}

	void SDL3Window::SetMouseLeaveCallback(std::function<void()> func)
	{
		m_OnMouseLeaveCallback = func;
	}

	void SDL3Window::SetFileDropCallback(std::function<void(const FileDropEventArgs &)> func)
	{
		m_OnFileDropCallback = func;
	}

	void SDL3Window::InvokeResizeCallback(const WindowResizedEventArgs &args)
	{
		if (!m_OnResizeCallback)
			return;

		m_OnResizeCallback(args);
	}

	void SDL3Window::InvokeMoveCallback(const WindowMovedEventArgs &args)
	{
		if (!m_OnMoveCallback)
			return;

		m_OnMoveCallback(args);
	}

	void SDL3Window::InvokeFocusGainCallback()
	{
		if (!m_OnFocusGainCallback)
			return;

		m_OnFocusGainCallback();
	}

	void SDL3Window::InvokeFocusLostCallback()
	{
		if (!m_OnFocusLostCallback)
			return;

		m_OnFocusLostCallback();
	}

	void SDL3Window::InvokeMaximizedCallback()
	{
		if (!m_OnMaximizeCallback)
			return;

		m_OnMaximizeCallback();
	}

	void SDL3Window::InvokeMinimizedCallback()
	{
		if (!m_OnMinimizeCallback)
			return;

		m_OnMinimizeCallback();
	}

	void SDL3Window::InvokeRestoreCallback()
	{
		if (!m_OnRestoreCallback)
			return;

		m_OnRestoreCallback();
	}

	void SDL3Window::InvokeShowCallback()
	{
		if (!m_OnShowCallback)
			return;

		m_OnShowCallback();
	}

	void SDL3Window::InvokeHideCallback()
	{
		if (!m_OnHideCallback)
			return;

		m_OnHideCallback();
	}

	void SDL3Window::InvokeExposeCallback()
	{
		if (!m_OnExposeCallback)
			return;

		m_OnExposeCallback();
	}

	void SDL3Window::InvokeKeyPressedCallback(const KeyPressedEventArgs &args)
	{
		m_KeyboardStates[args.KeyboardID].Keys[args.ScanCode] = true;

		if (!m_OnKeyPressedCallback)
			return;

		m_OnKeyPressedCallback(args);
	}

	void SDL3Window::InvokeKeyReleasedCallback(const KeyReleasedEventArgs &args)
	{
		m_KeyboardStates[args.KeyboardID].Keys[args.ScanCode] = false;

		if (!m_OnKeyReleasedCallback)
			return;

		m_OnKeyReleasedCallback(args);
	}

	void SDL3Window::InvokeTextInputCallback(const TextInputEventArgs &args)
	{
		if (!m_OnTextInputCallback)
			return;

		m_OnTextInputCallback(args);
	}

	void SDL3Window::InvokeTextEditCallback(const TextEditEventArgs &args)
	{
		if (!m_OnTextEditCallback)
			return;

		m_OnTextEditCallback(args);
	}

	void SDL3Window::InvokeMousePressedCallback(const MouseButtonPressedEventArgs &args)
	{
		MouseState &state = m_MouseStates[args.MouseID];

		switch (args.Button)
		{
			case MouseButton::Left: state.LeftButton = MouseButtonState::Pressed; break;
			case MouseButton::Right: state.RightButton = MouseButtonState::Pressed; break;
			case MouseButton::Middle: state.MiddleButton = MouseButtonState::Pressed; break;
			case MouseButton::X1: state.X1Button = MouseButtonState::Pressed; break;
			case MouseButton::X2: state.X2Button = MouseButtonState::Pressed; break;
			default: throw std::runtime_error("Failed to find valid mouse button");
		}

		if (!m_OnMouseButtonPressedCallback)
			return;

		m_OnMouseButtonPressedCallback(args);
	}

	void SDL3Window::InvokeMouseReleasedCallback(const MouseButtonReleasedEventArgs &args)
	{
		MouseState &state = m_MouseStates[args.MouseID];

		switch (args.Button)
		{
			case MouseButton::Left: state.LeftButton = MouseButtonState::Released; break;
			case MouseButton::Right: state.RightButton = MouseButtonState::Released; break;
			case MouseButton::Middle: state.MiddleButton = MouseButtonState::Released; break;
			case MouseButton::X1: state.X1Button = MouseButtonState::Released; break;
			case MouseButton::X2: state.X2Button = MouseButtonState::Released; break;
			default: throw std::runtime_error("Failed to find valid mouse button");
		}

		if (!m_OnMouseButtonReleasedCallback)
			return;

		m_OnMouseButtonReleasedCallback(args);
	}

	void SDL3Window::InvokeMouseMovedCallback(const MouseMovedEventArgs &args)
	{
		m_MouseStates[args.MouseID].MousePosition = args.Position;
		m_MouseStates[args.MouseID].MouseMovement = args.Movement;

		m_MouseStates[args.MouseID].MousePosition = args.Position;

		if (!m_OnMouseMovedCallback)
			return;

		m_OnMouseMovedCallback(args);
	}

	void SDL3Window::InvokeMouseScrollCallback(const MouseScrolledEventArgs &args)
	{
		m_MouseStates[args.MouseID].MouseWheel.X = args.Scroll.X;
		m_MouseStates[args.MouseID].MouseWheel.Y = args.Scroll.Y;

		if (!m_OnMouseScrolledCallback)
			return;

		m_OnMouseScrolledCallback(args);
	}

	void SDL3Window::InvokeMouseEnterCallback()
	{
		if (!m_OnMouseEnterCallback)
			return;

		m_OnMouseEnterCallback();
	}

	void SDL3Window::InvokeMouseLeaveCallback()
	{
		if (!m_OnMouseLeaveCallback)
			return;

		m_OnMouseLeaveCallback();
	}

	void SDL3Window::InvokeFileDropCallback(const FileDropEventArgs &args)
	{
		if (!m_OnFileDropCallback)
			return;

		m_OnFileDropCallback(args);
	}

	bool SDL3Window::IsKeyDown(uint32_t keyboardId, ScanCode scancode)
	{
		return m_KeyboardStates[keyboardId].Keys[scancode];
	}

	bool SDL3Window::IsKeyDown(ScanCode scancode)
	{
		std::optional<uint32_t> id = Platform::GetActiveKeyboardId();
		if (!id.has_value())
			return false;

		return IsKeyDown(id.value(), scancode);
	}

	Point2D<float> SDL3Window::GetMousePosition(uint32_t mouseId)
	{
		return Point2D<float>();
	}

	Point2D<float> SDL3Window::GetMouseScroll(uint32_t mouseId)
	{
		return Point2D<float>();
	}

	Point2D<float> SDL3Window::GetMousePosition()
	{
		return Point2D<float>();
	}

	Point2D<float> SDL3Window::GetMouseScroll()
	{
		return Point2D<float>();
	}

	bool SDL3Window::IsMouseButtonPressed(uint32_t mouseId, MouseButton state)
	{
		return false;
	}

	bool SDL3Window::IsMouseButtonPressed(MouseButton state)
	{
		return false;
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

		if (!windowSpec.Shown)
		{
			flags |= SDL_WINDOW_HIDDEN;
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

				m_RenderFrameRateMonitor.Update();

				if (m_RenderFunc)
				{
					Nexus::Input::SetContext(this);
					m_RenderFunc(time);
				}
			},
			secondsPerRender);

		m_Timer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				m_UpdateFrameRateMonitor.Update();

				if (m_UpdateFunc)
				{
					Nexus::Input::SetContext(this);
					m_UpdateFunc(time);
				}
			},
			secondsPerUpdate);

		m_Timer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				m_TickFrameRateMonitor.Update();

				if (m_TickFunc)
				{
					Nexus::Input::SetContext(this);
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
