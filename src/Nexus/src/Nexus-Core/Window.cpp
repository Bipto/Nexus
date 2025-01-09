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

#include "Nexus-Core/Input/Input.hpp"

namespace Nexus
{
	IWindow::IWindow(const WindowSpecification &windowProps, Graphics::GraphicsAPI api, const Graphics::SwapchainSpecification &swapchainSpec)
		: m_Specification(windowProps),
		  m_InputContext(this)
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

	IWindow::~IWindow()
	{
		SDL_DestroyWindow(this->m_Window);
	}

	void IWindow::CacheInput()
	{
		m_Input.CacheInput();
	}

	void IWindow::Update()
	{
		m_InputContext.Reset();
		m_Timer.Update();
	}

	void IWindow::SetResizable(bool isResizable)
	{
		SDL_SetWindowResizable(this->m_Window, isResizable);
	}

	void IWindow::SetTitle(const std::string &title)
	{
		SDL_SetWindowTitle(this->m_Window, title.c_str());
	}

	void IWindow::SetSize(Point2D<uint32_t> size)
	{
#if !defined(__EMSCRIPTEN__)
		SDL_SetWindowSize(m_Window, size.X, size.Y);
#else
		SDL_SetWindowSize(m_Window, size.X * GetDisplayScale(), size.Y * GetDisplayScale());
#endif
	}

	void IWindow::Close()
	{
		m_Closing = true;
	}

	bool IWindow::IsClosing()
	{
		return m_Closing;
	}

	SDL_Window *IWindow::GetSDLWindowHandle()
	{
		return m_Window;
	}

	Point2D<uint32_t> IWindow::GetWindowSize()
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

	Point2D<int> IWindow::GetWindowPosition()
	{
		Point2D<int> position {};
		SDL_GetWindowPosition(m_Window, &position.X, &position.Y);
		return position;
	}

	WindowState IWindow::GetCurrentWindowState()
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

	void IWindow::SetIsMouseVisible(bool visible)
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

	InputState *IWindow::GetInput()
	{
		return &m_Input;
	}

	Nexus::InputNew::InputContext *IWindow::GetInputContext()
	{
		return &m_InputContext;
	}

	LayerStack &IWindow::GetLayerStack()
	{
		return m_LayerStack;
	}

	bool IWindow::IsFocussed()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_INPUT_FOCUS;
	}

	bool IWindow::IsMinimized()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MINIMIZED;
	}

	bool IWindow::IsMaximized()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MAXIMIZED;
	}

	bool IWindow::IsFullscreen()
	{
		return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_FULLSCREEN;
	}

	void IWindow::Maximize()
	{
		SDL_MaximizeWindow(m_Window);
	}

	void IWindow::Minimize()
	{
		SDL_MinimizeWindow(m_Window);
	}

	void IWindow::Restore()
	{
		SDL_RestoreWindow(m_Window);
	}

	void IWindow::ToggleFullscreen()
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

	void IWindow::SetFullscreen()
	{
		SDL_SetWindowFullscreen(m_Window, true);
	}

	void IWindow::UnsetFullscreen()
	{
		SDL_SetWindowFullscreen(m_Window, false);
	}

	void IWindow::Show()
	{
		SDL_ShowWindow(m_Window);
	}

	void IWindow::Hide()
	{
		SDL_HideWindow(m_Window);
	}

	void IWindow::SetWindowPosition(int32_t x, int32_t y)
	{
		SDL_SetWindowPosition(m_Window, x, y);
	}

	void IWindow::Focus()
	{
		SDL_RaiseWindow(m_Window);
	}

	uint32_t IWindow::GetID()
	{
		return m_WindowID;
	}

	float IWindow::GetDisplayScale()
	{
		return SDL_GetWindowDisplayScale(m_Window);
	}

	void IWindow::SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect)
	{
		SDL_Rect r;
		r.x = rect.GetLeft();
		r.y = rect.GetTop();
		r.w = rect.GetWidth();
		r.h = rect.GetHeight();
		SDL_SetTextInputArea(m_Window, &r, 0);
	}

	void IWindow::StartTextInput()
	{
		SDL_StartTextInput(m_Window);
	}

	void IWindow::StopTextInput()
	{
		SDL_StopTextInput(m_Window);
	}

	void IWindow::SetRendersPerSecond(uint32_t amount)
	{
		m_Specification.RendersPerSecond = amount;
		SetupTimer();
	}

	void IWindow::SetUpdatesPerSecond(uint32_t amount)
	{
		m_Specification.UpdatesPerSecond = amount;
		SetupTimer();
	}

	void IWindow::SetTicksPerSecond(uint32_t amount)
	{
		m_Specification.TicksPerSecond = amount;
		SetupTimer();
	}

	void IWindow::SetRelativeMouseMode(bool enabled)
	{
		SDL_SetWindowRelativeMouseMode(m_Window, enabled);
	}

	NativeWindowInfo IWindow::GetNativeWindowInfo()
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

	uint32_t IWindow::GetFlags(Graphics::GraphicsAPI				   api,
							   const WindowSpecification			  &windowSpec,
							   const Graphics::SwapchainSpecification &swapchainSpec)
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

	void IWindow::SetupTimer()
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
				OnRender.Invoke(time);
				m_LayerStack.OnRender(time);
			},
			secondsPerRender);

		m_Timer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				Input::SetContext(&m_InputContext);
				m_UpdateFrameRateMonitor.Update();
				OnUpdate.Invoke(time);
				m_LayerStack.OnUpdate(time);
			},
			secondsPerUpdate);

		m_Timer.Every(
			[&](Nexus::TimeSpan time)
			{
				if (IsMinimized())
					return;

				Input::SetContext(&m_InputContext);
				m_TickFrameRateMonitor.Update();
				OnTick.Invoke(time);
				m_LayerStack.OnTick(time);
			},
			secondsPerTick);
	}

	const WindowSpecification &IWindow::GetSpecification() const
	{
		return m_Specification;
	}

}	 // namespace Nexus
