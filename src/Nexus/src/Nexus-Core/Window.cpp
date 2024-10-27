#include "Window.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "glad/glad.h"

#if defined(NX_PLATFORM_OPENGL)
	#include "Platform/OpenGL/SwapchainOpenGL.hpp"
#endif

#if defined(NX_PLATFORM_VULKAN)
	#include "Platform/Vulkan/SwapchainVk.hpp"
#endif

#if defined(NX_PLATFORM_D3D12)
	#include "Platform/D3D12/SwapchainD3D12.hpp"
#endif

#include "Nexus-Core/Application.hpp"

namespace Nexus
{

	Window::Window(const WindowSpecification &windowProps, Graphics::GraphicsAPI api, const Graphics::SwapchainSpecification &swapchainSpec)
		: m_Specification(windowProps)
	{
		// NOTE: Resizable flag MUST be set in order for Emscripten resizing to work
		// correctly
		uint32_t flags = GetFlags(api, windowProps, swapchainSpec);

		m_Window = SDL_CreateWindow(windowProps.Title.c_str(), windowProps.Width, windowProps.Height, flags);

		if (m_Window == nullptr)
		{
			std::string errorCode = {SDL_GetError()};
			NX_ERROR(errorCode);
		}

		m_WindowID = SDL_GetWindowID(m_Window);
		SetupTimer();
	}

	Window::~Window()
	{
		delete m_Swapchain;
		SDL_DestroyWindow(this->m_Window);
	}

	void Window::CacheInput()
	{
		m_Input.CacheInput();
	}

	void Window::Update()
	{
		m_Timer.Update();
	}

	void Window::SetResizable(bool isResizable)
	{
		SDL_SetWindowResizable(this->m_Window, (SDL_bool)isResizable);
	}

	void Window::SetTitle(const std::string &title)
	{
		SDL_SetWindowTitle(this->m_Window, title.c_str());
	}

	void Window::SetSize(Point2D<uint32_t> size)
	{
#if !defined(__EMSCRIPTEN__)
		SDL_SetWindowSize(m_Window, size.X, size.Y);
#else
		SDL_SetWindowSize(m_Window, size.X * GetDisplayScale(), size.Y * GetDisplayScale());
#endif
	}

	void Window::Close()
	{
		m_Closing = true;
	}

	bool Window::IsClosing()
	{
		return m_Closing;
	}

	SDL_Window *Window::GetSDLWindowHandle()
	{
		return m_Window;
	}

	Point2D<uint32_t> Window::GetWindowSize()
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

	Point2D<int> Window::GetWindowPosition()
	{
		Point2D<int> position {};
		SDL_GetWindowPosition(m_Window, &position.X, &position.Y);
		return position;
	}

	WindowState Window::GetCurrentWindowState()
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

	void Window::SetIsMouseVisible(bool visible)
	{
		switch (visible)
		{
			case true: SDL_ShowCursor(); break;
			case false: SDL_HideCursor(); break;
		}
	}

	void Window::SetCursor(Cursor cursor)
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

	InputState *Window::GetInput()
	{
		return &m_Input;
	}

	const Nexus::InputNew::InputContext &Window::GetInputContext() const
	{
		return m_InputContext;
	}

	bool Window::IsFocussed()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_INPUT_FOCUS;
	}

	bool Window::IsMinimized()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MINIMIZED;
	}

	bool Window::IsMaximized()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MAXIMIZED;
	}

	bool Window::IsFullscreen()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_FULLSCREEN;
	}

	void Window::Maximize()
	{
		SDL_MaximizeWindow(m_Window);
	}

	void Window::Minimize()
	{
		SDL_MinimizeWindow(m_Window);
	}

	void Window::Restore()
	{
		SDL_RestoreWindow(m_Window);
	}

	void Window::ToggleFullscreen()
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

	void Window::SetFullscreen()
	{
		SDL_SetWindowFullscreen(m_Window, SDL_TRUE);
	}

	void Window::UnsetFullscreen()
	{
		SDL_SetWindowFullscreen(m_Window, SDL_FALSE);
	}

	void Window::Show()
	{
		SDL_ShowWindow(m_Window);
	}

	void Window::Hide()
	{
		SDL_HideWindow(m_Window);
	}

	void Window::SetWindowPosition(int32_t x, int32_t y)
	{
		SDL_SetWindowPosition(m_Window, x, y);
	}

	void Window::Focus()
	{
		SDL_RaiseWindow(m_Window);
	}

	void Window::CreateSwapchain(Graphics::GraphicsDevice *device, const Graphics::SwapchainSpecification &swapchainSpec)
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

	Graphics::Swapchain *Window::GetSwapchain()
	{
		return m_Swapchain;
	}

	uint32_t Window::GetID()
	{
		return m_WindowID;
	}

	float Window::GetDisplayScale()
	{
		return SDL_GetWindowDisplayScale(m_Window);
	}

	void Window::SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect)
	{
		SDL_Rect r;
		r.x = rect.GetLeft();
		r.y = rect.GetTop();
		r.w = rect.GetWidth();
		r.h = rect.GetHeight();
		SDL_SetTextInputRect(&r);
	}

	void Window::StartTextInput()
	{
		SDL_StartTextInput();
	}

	void Window::StopTextInput()
	{
		SDL_StopTextInput();
	}

	void Window::SetRendersPerSecond(uint32_t amount)
	{
		m_Specification.RendersPerSecond = amount;
		SetupTimer();
	}

	void Window::SetUpdatesPerSecond(uint32_t amount)
	{
		m_Specification.UpdatesPerSecond = amount;
		SetupTimer();
	}

	void Window::SetTicksPerSecond(uint32_t amount)
	{
		m_Specification.TicksPerSecond = amount;
		SetupTimer();
	}

	void Window::SetRelativeMouseMode(bool enabled)
	{
		SDL_SetRelativeMouseMode(enabled);
	}

	void Window::OnEvent(const InputEvent &event)
	{
	}

#if defined(NX_PLATFORM_WINDOWS)
	const HWND Window::GetHwnd() const
	{
		HWND hwnd = (HWND)SDL_GetProperty(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
		return hwnd;
	}

#endif

	uint32_t Window::GetFlags(Graphics::GraphicsAPI api, const WindowSpecification &windowSpec, const Graphics::SwapchainSpecification &swapchainSpec)
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

		switch (api)
		{
			case Graphics::GraphicsAPI::Vulkan:
			{
				flags |= SDL_WINDOW_VULKAN;
				return flags;
			}
			default: return flags;
		}
	}

	void Window::SetupTimer()
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
			OnRender.Invoke(time);
		},
		secondsPerRender);

		m_Timer.Every(
		[&](Nexus::TimeSpan time)
		{
			if (IsMinimized())
				return;

			m_UpdateFrameRateMonitor.Update();
			OnUpdate.Invoke(time);
		},
		secondsPerUpdate);

		m_Timer.Every(
		[&](Nexus::TimeSpan time)
		{
			if (IsMinimized())
				return;

			m_TickFrameRateMonitor.Update();
			OnTick.Invoke(time);
		},
		secondsPerTick);
	}

	const WindowSpecification &Window::GetSpecification() const
	{
		return m_Specification;
	}

	void Window::HandleEvent(SDL_Event &event)
	{
		switch (event.type)
		{
			case SDL_EVENT_KEY_DOWN:
			{
				auto nexusKeyCode  = Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.keysym.sym);
				auto nexusScanCode = Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.keysym.scancode);
				auto mods		   = Nexus::SDL3::GetNexusModifiersFromSDLModifiers(event.key.keysym.mod);

				KeyPressedEvent keyPressedEvent {.KeyCode	 = nexusKeyCode,
												 .ScanCode	 = nexusScanCode,
												 .Repeat	 = event.key.repeat,
												 .Unicode	 = event.key.keysym.sym,
												 .Mods		 = mods,
												 .KeyboardID = event.kdevice.which};

				OnKeyPressed.Invoke(keyPressedEvent);

				break;
			}
			case SDL_EVENT_KEY_UP:
			{
				auto nexusKeyCode  = Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.keysym.sym);
				auto nexusScanCode = Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.keysym.scancode);

				KeyReleasedEvent keyReleasedEvent {.KeyCode	   = nexusKeyCode,
												   .ScanCode   = nexusScanCode,
												   .Unicode	   = event.key.keysym.sym,
												   .KeyboardID = event.kdevice.which};

				OnKeyReleased.Invoke(keyReleasedEvent);

				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				auto [mouseType, mouseId] = SDL3::GetMouseInfo(event.button.which);
				MouseButton button		  = SDL3::GetMouseButton(event.button.button);

				MouseButtonPressedEvent mousePressedEvent {.Button	 = button,
														   .Position = {event.button.x, event.button.y},
														   .Clicks	 = event.button.clicks,
														   .MouseID	 = mouseId,
														   .Type	 = mouseType};

				OnMousePressed.Invoke(mousePressedEvent);
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				auto [mouseType, mouseId] = SDL3::GetMouseInfo(event.button.which);
				MouseButton button		  = SDL3::GetMouseButton(event.button.button);

				MouseButtonReleasedEvent mouseReleasedEvent {.Button   = button,
															 .Position = {event.button.x, event.button.y},
															 .MouseID  = mouseId,
															 .Type	   = mouseType};

				OnMouseReleased.Invoke(mouseReleasedEvent);
				break;
			}
			case SDL_EVENT_MOUSE_MOTION:
			{
				auto [mouseType, mouseId] = SDL3::GetMouseInfo(event.motion.which);

				MouseMovedEvent mouseMovedEvent {.Position = {event.motion.x, event.motion.y},
												 .Movement = {event.motion.xrel, event.motion.yrel},
												 .MouseID  = mouseId,
												 .Type	   = mouseType};

				OnMouseMoved.Invoke(mouseMovedEvent);
				break;
			}
			case SDL_EVENT_MOUSE_WHEEL:
			{
				auto [mouseType, mouseId] = SDL3::GetMouseInfo(event.wheel.which);
				ScrollDirection direction = SDL3::GetScrollDirection(event.wheel.direction);

				MouseScrolledEvent scrollEvent {.Scroll	   = {event.wheel.x, event.wheel.y},
												.Position  = {event.wheel.mouse_x, event.wheel.mouse_y},
												.MouseID   = mouseId,
												.Type	   = mouseType,
												.Direction = direction};

				OnScroll.Invoke(scrollEvent);
				break;
			}
		}
	}
}	 // namespace Nexus
