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
		SDL_DestroyWindow(m_Window);
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
#elif defined(NX_PLATFORM_ANDROID)
		info.nativeWindow = (ANativeWindow *)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
#endif

		return info;
	}

	void SDL3Window::AddResizeCallback(std::function<void(const WindowResizedEventArgs &)> func)
	{
		m_OnResizeCallbacks.push_back(func);
	}

	void SDL3Window::AddMoveCallback(std::function<void(const WindowMovedEventArgs &)> func)
	{
		m_OnMoveCallbacks.push_back(func);
	}

	void SDL3Window::AddFocusGainCallback(std::function<void()> func)
	{
		m_OnFocusGainCallbacks.push_back(func);
	}

	void SDL3Window::AddFocusLostCallback(std::function<void()> func)
	{
		m_OnFocusLostCallbacks.push_back(func);
	}

	void SDL3Window::AddMaximizedCallback(std::function<void()> func)
	{
		m_OnMaximizeCallbacks.push_back(func);
	}

	void SDL3Window::AddMinimizedCallback(std::function<void()> func)
	{
		m_OnMinimizeCallbacks.push_back(func);
	}

	void SDL3Window::AddRestoreCallback(std::function<void()> func)
	{
		m_OnRestoreCallbacks.push_back(func);
	}

	void SDL3Window::AddShowCallback(std::function<void()> func)
	{
		m_OnShowCallbacks.push_back(func);
	}

	void SDL3Window::AddHideCallback(std::function<void()> func)
	{
		m_OnHideCallbacks.push_back(func);
	}

	void SDL3Window::AddExposeCallback(std::function<void()> func)
	{
		m_OnExposeCallbacks.push_back(func);
	}

	void SDL3Window::AddKeyPressedCallback(std::function<void(const KeyPressedEventArgs &)> func)
	{
		m_OnKeyPressedCallbacks.push_back(func);
	}

	void SDL3Window::AddKeyReleasedCallback(std::function<void(const KeyReleasedEventArgs &)> func)
	{
		m_OnKeyReleasedCallbacks.push_back(func);
	}

	void SDL3Window::AddTextInputCallback(std::function<void(const TextInputEventArgs &)> func)
	{
		m_OnTextInputCallbacks.push_back(func);
	}

	void SDL3Window::AddTextEditCallback(std::function<void(const TextEditEventArgs &)> func)
	{
		m_OnTextEditCallbacks.push_back(func);
	}

	void SDL3Window::AddMousePressedCallback(std::function<void(const MouseButtonPressedEventArgs &)> func)
	{
		m_OnMouseButtonPressedCallbacks.push_back(func);
	}

	void SDL3Window::AddMouseReleasedCallback(std::function<void(const MouseButtonReleasedEventArgs &)> func)
	{
		m_OnMouseButtonReleasedCallbacks.push_back(func);
	}

	void SDL3Window::AddMouseMovedCallback(std::function<void(const MouseMovedEventArgs &)> func)
	{
		m_OnMouseMovedCallbacks.push_back(func);
	}

	void SDL3Window::AddMouseScrollCallback(std::function<void(const MouseScrolledEventArgs &)> func)
	{
		m_OnMouseScrolledCallbacks.push_back(func);
	}

	void SDL3Window::AddMouseEnterCallback(std::function<void()> func)
	{
		m_OnMouseEnterCallbacks.push_back(func);
	}

	void SDL3Window::AddMouseLeaveCallback(std::function<void()> func)
	{
		m_OnMouseLeaveCallbacks.push_back(func);
	}

	void SDL3Window::AddFileDropCallback(std::function<void(const FileDropEventArgs &)> func)
	{
		m_OnFileDropCallbacks.push_back(func);
	}

	void SDL3Window::InvokeResizeCallback(const WindowResizedEventArgs &args)
	{
		for (auto &func : m_OnResizeCallbacks) { func(args); }
	}

	void SDL3Window::InvokeMoveCallback(const WindowMovedEventArgs &args)
	{
		for (auto &func : m_OnMoveCallbacks) { func(args); }
	}

	void SDL3Window::InvokeFocusGainCallback()
	{
		for (auto &func : m_OnFocusGainCallbacks) { func(); }
	}

	void SDL3Window::InvokeFocusLostCallback()
	{
		for (auto &func : m_OnFocusLostCallbacks) { func(); }
	}

	void SDL3Window::InvokeMaximizedCallback()
	{
		for (auto &func : m_OnMaximizeCallbacks) { func(); }
	}

	void SDL3Window::InvokeMinimizedCallback()
	{
		for (auto &func : m_OnMinimizeCallbacks) { func(); }
	}

	void SDL3Window::InvokeRestoreCallback()
	{
		for (auto &func : m_OnRestoreCallbacks) { func(); }
	}

	void SDL3Window::InvokeShowCallback()
	{
		for (auto &func : m_OnShowCallbacks) { func(); }
	}

	void SDL3Window::InvokeHideCallback()
	{
		for (auto &func : m_OnHideCallbacks) { func(); }
	}

	void SDL3Window::InvokeExposeCallback()
	{
		for (auto &func : m_OnExposeCallbacks) { func(); }
	}

	void SDL3Window::InvokeKeyPressedCallback(const KeyPressedEventArgs &args)
	{
		m_KeyboardStates[args.KeyboardID].Keys[args.ScanCode] = true;
		for (auto &func : m_OnKeyPressedCallbacks) { func(args); }
	}

	void SDL3Window::InvokeKeyReleasedCallback(const KeyReleasedEventArgs &args)
	{
		m_KeyboardStates[args.KeyboardID].Keys[args.ScanCode] = false;
		for (auto &func : m_OnKeyReleasedCallbacks) { func(args); }
	}

	void SDL3Window::InvokeTextInputCallback(const TextInputEventArgs &args)
	{
		for (auto &func : m_OnTextInputCallbacks) { func(args); }
	}

	void SDL3Window::InvokeTextEditCallback(const TextEditEventArgs &args)
	{
		for (auto &func : m_OnTextEditCallbacks) { func(args); }
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

		for (auto &func : m_OnMouseButtonPressedCallbacks) { func(args); }
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

		for (auto &func : m_OnMouseButtonReleasedCallbacks) { func(args); }
	}

	void SDL3Window::InvokeMouseMovedCallback(const MouseMovedEventArgs &args)
	{
		m_MouseStates[args.MouseID].MousePosition = args.Position;
		m_MouseStates[args.MouseID].MouseMovement = args.Movement;
		m_MouseStates[args.MouseID].MousePosition = args.Position;

		for (auto &func : m_OnMouseMovedCallbacks) { func(args); }
	}

	void SDL3Window::InvokeMouseScrollCallback(const MouseScrolledEventArgs &args)
	{
		m_MouseStates[args.MouseID].MouseWheel.X = args.Scroll.X;
		m_MouseStates[args.MouseID].MouseWheel.Y = args.Scroll.Y;

		for (auto &func : m_OnMouseScrolledCallbacks) { func(args); }
	}

	void SDL3Window::InvokeMouseEnterCallback()
	{
		for (auto &func : m_OnMouseEnterCallbacks) { func(); }
	}

	void SDL3Window::InvokeMouseLeaveCallback()
	{
		for (auto &func : m_OnMouseLeaveCallbacks) { func(); }
	}

	void SDL3Window::InvokeFileDropCallback(const FileDropEventArgs &args)
	{
		for (auto &func : m_OnFileDropCallbacks) { func(args); }
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
