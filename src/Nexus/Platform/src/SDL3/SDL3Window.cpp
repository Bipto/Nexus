#include <stdexcept>

#include "SDL3Window.hpp"

#include "Platform/IWindow.hpp"
#include "Platform/Input/Input.hpp"
#include "Platform/Platform.hpp"
#include "Platform/Timings/Profiler.hpp"

namespace Nexus
{
	static uint32_t GetSDl3WindowFlags(uint32_t windowFlags)
	{
		uint32_t sdlFlags = 0;

		if (windowFlags & WindowFlags_Fullscreen)
		{
			sdlFlags |= SDL_WINDOW_RESIZABLE;
		}

		if (windowFlags & WindowFlags_Occluded)
		{
			sdlFlags |= SDL_WINDOW_OCCLUDED;
		}

		if (windowFlags & WindowFlags_Hidden)
		{
			sdlFlags |= SDL_WINDOW_HIDDEN;
		}

		if (windowFlags & WindowFlags_Borderless)
		{
			sdlFlags |= SDL_WINDOW_BORDERLESS;
		}

		if (windowFlags & WindowFlags_Resizable)
		{
			sdlFlags |= SDL_WINDOW_RESIZABLE;
		}

		if (windowFlags & WindowFlags_Minimized)
		{
			sdlFlags |= SDL_WINDOW_MINIMIZED;
		}

		if (windowFlags & WindowFlags_Maximized)
		{
			sdlFlags |= SDL_WINDOW_MAXIMIZED;
		}

		if (windowFlags & WindowFlags_MouseGrabbed)
		{
			sdlFlags |= SDL_WINDOW_MOUSE_GRABBED;
		}

		if (windowFlags & WindowFlags_InputFocus)
		{
			sdlFlags |= SDL_WINDOW_INPUT_FOCUS;
		}

		if (windowFlags & WindowFlags_MouseFocus)
		{
			sdlFlags |= SDL_WINDOW_MOUSE_FOCUS;
		}

		if (windowFlags & WindowFlags_Modal)
		{
			sdlFlags |= SDL_WINDOW_MODAL;
		}

		if (windowFlags & WindowFlags_HighPixelDensity)
		{
			sdlFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
		}

		if (windowFlags & WindowFlags_MouseCapture)
		{
			sdlFlags |= SDL_WINDOW_MOUSE_CAPTURE;
		}

		if (windowFlags & WindowFlags_MouseRelativeMode)
		{
			sdlFlags |= SDL_WINDOW_MOUSE_RELATIVE_MODE;
		}

		if (windowFlags & WindowFlags_AlwaysOnTop)
		{
			sdlFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
		}

		if (windowFlags & WindowFlags_Utility)
		{
			sdlFlags |= SDL_WINDOW_UTILITY;
		}

		if (windowFlags & WindowFlags_Tooltip)
		{
			sdlFlags |= SDL_WINDOW_TOOLTIP;
		}

		if (windowFlags & WindowFlags_PopupMenu)
		{
			sdlFlags |= SDL_WINDOW_POPUP_MENU;
		}

		if (windowFlags & WindowFlags_KeyboardGrabbed)
		{
			sdlFlags |= SDL_WINDOW_KEYBOARD_GRABBED;
		}

		if (windowFlags & WindowFlags_Transparent)
		{
			sdlFlags |= SDL_WINDOW_TRANSPARENT;
		}

		if (windowFlags & WindowFlags_NotFocusable)
		{
			sdlFlags |= SDL_WINDOW_NOT_FOCUSABLE;
		}

		return sdlFlags;
	}

	SDL3Window::SDL3Window(const WindowDescription &windowProps) : IWindow(windowProps), m_Description(windowProps)
	{
		std::string idSelector = "#" + windowProps.CanvasId;
		SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR, idSelector.c_str());
		uint32_t flags = GetSDl3WindowFlags(windowProps.Flags);

		m_Window = {SDL_CreateWindow(windowProps.Title.c_str(), windowProps.Width, windowProps.Height, flags), SDL_DestroyWindow};

		if (m_Window == nullptr)
		{
			std::string errorCode = {SDL_GetError()};
			throw std::runtime_error("Failed to create SDL3 window: " + errorCode);
		}

		m_WindowID = SDL_GetWindowID(m_Window.get());
		SetupTimer();
	}

	SDL3Window::~SDL3Window()
	{
	}

	void SDL3Window::Update()
	{
		m_RenderTimer.Update();
		m_UpdateTimer.Update();
		m_TickTimer.Update();
	}

	void SDL3Window::SetResizable(bool isResizable)
	{
		SDL_SetWindowResizable(m_Window.get(), isResizable);
	}

	void SDL3Window::SetTitle(const std::string &title)
	{
		SDL_SetWindowTitle(m_Window.get(), title.c_str());
	}

	void SDL3Window::SetSize(uint32_t width, uint32_t height)
	{
		/* #if !defined(__EMSCRIPTEN__)
				SDL_SetWindowSize(m_Window, size.X, size.Y);
		#else
				SDL_SetWindowSize(m_Window, size.X * GetDisplayScale(), size.Y * GetDisplayScale());
		#endif */

		SDL_SetWindowSize(m_Window.get(), width, height);
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
		return m_Window.get();
	}

	std::pair<uint32_t, uint32_t> SDL3Window::GetWindowSize()
	{
		int x, y;
		SDL_GetWindowSize(m_Window.get(), &x, &y);

		/* #if defined(__EMSCRIPTEN__)
				size.X *= GetDisplayScale();
				size.Y *= GetDisplayScale();
		#endif */

		return {static_cast<uint32_t>(x), static_cast<uint32_t>(y)};
	}

	std::pair<uint32_t, uint32_t> SDL3Window::GetWindowSizeInPixels()
	{
		int w, h;
		SDL_GetWindowSizeInPixels(m_Window.get(), &w, &h);
		return {static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
	}

	std::pair<int32_t, int32_t> SDL3Window::GetWindowPosition()
	{
		int32_t x, y;
		SDL_GetWindowPosition(m_Window.get(), &x, &y);
		return {x, y};
	}

	WindowState SDL3Window::GetCurrentWindowState()
	{
		Uint32 flags = SDL_GetWindowFlags(m_Window.get());

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
		return SDL_GetWindowFlags(m_Window.get()) & SDL_WINDOW_INPUT_FOCUS;
	}

	bool SDL3Window::IsMinimized()
	{
		return SDL_GetWindowFlags(m_Window.get()) & SDL_WINDOW_MINIMIZED;
	}

	bool SDL3Window::IsMaximized()
	{
		return SDL_GetWindowFlags(m_Window.get()) & SDL_WINDOW_MAXIMIZED;
	}

	bool SDL3Window::IsFullscreen()
	{
		return SDL_GetWindowFlags(m_Window.get()) & SDL_WINDOW_FULLSCREEN;
	}

	void SDL3Window::Maximize()
	{
		SDL_MaximizeWindow(m_Window.get());
	}

	void SDL3Window::Minimize()
	{
		SDL_MinimizeWindow(m_Window.get());
	}

	void SDL3Window::Restore()
	{
		SDL_RestoreWindow(m_Window.get());
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
		SDL_SetWindowFullscreen(m_Window.get(), true);
	}

	void SDL3Window::UnsetFullscreen()
	{
		SDL_SetWindowFullscreen(m_Window.get(), false);
	}

	void SDL3Window::Show()
	{
		SDL_ShowWindow(m_Window.get());
	}

	void SDL3Window::Hide()
	{
		SDL_HideWindow(m_Window.get());
	}

	void SDL3Window::SetWindowPosition(int32_t x, int32_t y)
	{
		SDL_SetWindowPosition(m_Window.get(), x, y);
	}

	void SDL3Window::Focus()
	{
		SDL_RaiseWindow(m_Window.get());
	}

	uint32_t SDL3Window::GetID()
	{
		return m_WindowID;
	}

	float SDL3Window::GetDisplayScale()
	{
		return SDL_GetWindowDisplayScale(m_Window.get());
	}

	void SDL3Window::SetTextInputRect(int32_t x, int32_t y, int32_t width, int32_t height)
	{
		SDL_Rect r {
			.x = x,
			.y = y,
			.w = width,
			.h = height,
		};

		SDL_SetTextInputArea(m_Window.get(), &r, 0);
	}

	void SDL3Window::StartTextInput()
	{
		SDL_StartTextInput(m_Window.get());
	}

	void SDL3Window::StopTextInput()
	{
		SDL_StopTextInput(m_Window.get());
	}

	void SDL3Window::SetRendersPerSecond(std::optional<uint32_t> amount)
	{
		m_Description.RendersPerSecond = amount;
		SetupTimer();
	}

	void SDL3Window::SetUpdatesPerSecond(std::optional<uint32_t> amount)
	{
		m_Description.UpdatesPerSecond = amount;
		SetupTimer();
	}

	void SDL3Window::SetTicksPerSecond(std::optional<uint32_t> amount)
	{
		m_Description.TicksPerSecond = amount;
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
		SDL_SetWindowRelativeMouseMode(m_Window.get(), enabled);
	}

	NativeWindowInfo SDL3Window::GetNativeWindowInfo()
	{
		NativeWindowInfo info = {};

		SDL_PropertiesID properties = SDL_GetWindowProperties(m_Window.get());

#if defined(NX_PLATFORM_WINDOWS)
		info.hwnd	  = (HWND)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
		info.hdc	  = (HDC)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HDC_POINTER, nullptr);
		info.instance = (HINSTANCE)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
#elif defined(NX_PLATFORM_LINUX)
		info.display = (Display *)SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
		info.screen	 = (int)SDL_GetNumberProperty(properties, SDL_PROP_WINDOW_X11_SCREEN_NUMBER, 0);
		info.window	 = (Window)(unsigned long)SDL_GetNumberProperty(properties, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
#elif defined(NX_PLATFORM_WEB)
		info.canvasId = m_Description.CanvasId;
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
		m_MouseStates[args.MouseID].MouseWheel = {args.Scroll.first, args.Scroll.second};

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

	std::pair<float, float> SDL3Window::GetMousePosition(uint32_t mouseId)
	{
		return m_MouseStates[mouseId].MousePosition;
	}

	std::pair<float, float> SDL3Window::GetMouseScroll(uint32_t mouseId)
	{
		return m_MouseStates[mouseId].MouseWheel;
	}

	std::optional<std::pair<float, float>> SDL3Window::GetMousePosition()
	{
		std::optional<uint32_t> id = Platform::GetActiveMouseId();
		if (!id.has_value())
			return std::nullopt;

		auto mousePos = GetMousePosition(id.value());
		return mousePos;
	}

	std::optional<std::pair<float, float>> SDL3Window::GetMouseScroll()
	{
		std::optional<uint32_t> id = Platform::GetActiveMouseId();
		if (!id.has_value())
			return std::nullopt;

		return GetMouseScroll(id.value());
	}

	bool SDL3Window::IsMouseButtonPressed(uint32_t mouseId, MouseButton state)
	{
		switch (state)
		{
			case MouseButton::Left: return m_MouseStates[mouseId].LeftButton == MouseButtonState::Pressed;
			case MouseButton::Right: return m_MouseStates[mouseId].RightButton == MouseButtonState::Pressed;
			case MouseButton::Middle: return m_MouseStates[mouseId].MiddleButton == MouseButtonState::Pressed;
			case MouseButton::X1: return m_MouseStates[mouseId].X1Button == MouseButtonState::Pressed;
			case MouseButton::X2: return m_MouseStates[mouseId].X2Button == MouseButtonState::Pressed;
			default: throw std::runtime_error("Failed to find a valid button");
		}
	}

	bool SDL3Window::IsMouseButtonPressed(MouseButton state)
	{
		std::optional<uint32_t> id = Platform::GetActiveMouseId();
		if (!id.has_value())
			return false;

		return IsMouseButtonPressed(id.value(), state);
	}

	void SDL3Window::SetupTimer()
	{
		m_RenderTimer.Clear();
		m_UpdateTimer.Clear();
		m_TickTimer.Clear();

		std::optional<double> secondsPerRender = {};
		std::optional<double> secondsPerUpdate = {};
		std::optional<double> secondsPerTick   = {};

		if (m_Description.RendersPerSecond.has_value())
		{
			secondsPerRender = 1.0 / m_Description.RendersPerSecond.value();
		}

		if (m_Description.UpdatesPerSecond.has_value())
		{
			secondsPerUpdate = 1.0 / m_Description.UpdatesPerSecond.value();
		}

		if (m_Description.TicksPerSecond.has_value())
		{
			secondsPerTick = 1.0 / m_Description.TicksPerSecond.value();
		}

		m_RenderTimer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				m_RenderFrameRateMonitor.Update();

				if (m_RenderFunc)
				{
					NX_PROFILE_SCOPE("OnRender");
					Nexus::Input::SetContext(this);
					m_RenderFunc(time);
				}
			},
			secondsPerRender);

		m_UpdateTimer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				m_UpdateFrameRateMonitor.Update();

				if (m_UpdateFunc)
				{
					NX_PROFILE_SCOPE("OnUpdate");
					Nexus::Input::SetContext(this);
					m_UpdateFunc(time);
				}
			},
			secondsPerUpdate);

		m_TickTimer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				m_TickFrameRateMonitor.Update();

				if (m_TickFunc)
				{
					NX_PROFILE_SCOPE("OnTick");
					Nexus::Input::SetContext(this);
					m_TickFunc(time);
				}
			},
			secondsPerTick);
	}

	const WindowDescription &SDL3Window::GetDescription() const
	{
		return m_Description;
	}

}	 // namespace Nexus
