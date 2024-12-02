#include "Application.hpp"

// graphics headers
#if defined(NX_PLATFORM_OPENGL)
	#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"
#endif

#if defined(NX_PLATFORM_D3D12)
	#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"
#endif

#if defined(NX_PLATFORM_VULKAN)
	#include "Platform/Vulkan/GraphicsDeviceVk.hpp"
#endif

// audio headers
#if defined(NX_PLATFORM_OPENAL)
	#include "Platform/OpenAL/AudioDeviceOpenAL.hpp"
#endif

#include "Platform/Software/GraphicsDeviceSoftware.hpp"

#include "Nexus-Core/Input/Input.hpp"
#include "Nexus-Core/Logging/Log.hpp"

#include "Platform/SDL3/SDL3Include.hpp"
#include "Platform/SDL3/SDL3Window.hpp"

#include "Platform.hpp"

namespace Nexus
{
	Application::Application(const ApplicationSpecification &spec)
	{
		m_Specification = spec;

		m_Window = Platform::CreatePlatformWindow(spec.WindowProperties, spec.GraphicsAPI, spec.SwapchainSpecification);
		Nexus::Input::SetContext(m_Window->GetInputContext());

		Graphics::GraphicsDeviceSpecification graphicsDeviceCreateInfo;
		graphicsDeviceCreateInfo.API = spec.GraphicsAPI;

		m_GraphicsDevice = Nexus::CreateGraphicsDevice(graphicsDeviceCreateInfo, m_Window, spec.SwapchainSpecification);

		auto swapchain = m_Window->GetSwapchain();
		swapchain->SetVSyncState(spec.SwapchainSpecification.VSyncState);

		m_AudioDevice = Nexus::CreateAudioDevice(spec.AudioAPI);

		SDL_StartTextInput();

		m_Window->OnRender.Bind([&](Nexus::TimeSpan time) { Render(time); });
		m_Window->OnUpdate.Bind([&](Nexus::TimeSpan time) { Update(time); });
		m_Window->OnTick.Bind([&](Nexus::TimeSpan time) { Tick(time); });
	}

	Application::~Application()
	{
		SDL_StopTextInput();
		delete m_Window;
		delete m_AudioDevice;
		delete m_GraphicsDevice;
	}

	void Application::MainLoop()
	{
		m_GlobalKeyboardState.CacheInput();

		// cache the previous frame's input
		Nexus::Mouse::s_PreviousGlobalMousePosition = Nexus::Mouse::s_GlobalMousePosition;

		float  x, y;
		Uint32 buttons = SDL_GetGlobalMouseState(&x, &y);

#if defined(__EMSCRIPTEN__)
		x *= GetPrimaryWindow()->GetDisplayScale();
		y *= GetPrimaryWindow()->GetDisplayScale();
#endif

		Nexus::Mouse::s_GlobalMousePosition.X = x;
		Nexus::Mouse::s_GlobalMousePosition.Y = y;

		if (buttons & SDL_BUTTON_LEFT)
		{
			Nexus::Mouse::s_GlobalMouseState.LeftButton = Nexus::MouseButtonState::Pressed;
		}
		else
		{
			Nexus::Mouse::s_GlobalMouseState.LeftButton = Nexus::MouseButtonState::Released;
		}

		if (buttons & SDL_BUTTON_RIGHT)
		{
			Nexus::Mouse::s_GlobalMouseState.RightButton = Nexus::MouseButtonState::Pressed;
		}
		else
		{
			Nexus::Mouse::s_GlobalMouseState.RightButton = Nexus::MouseButtonState::Released;
		}

		if (buttons & SDL_BUTTON_MIDDLE)
		{
			Nexus::Mouse::s_GlobalMouseState.MiddleButton = Nexus::MouseButtonState::Pressed;
		}
		else
		{
			Nexus::Mouse::s_GlobalMouseState.MiddleButton = Nexus::MouseButtonState::Released;
		}

		Platform::Update();
	}

	Nexus::Window *Application::GetPrimaryWindow()
	{
		return m_Window;
	}

	Point2D<uint32_t> Application::GetWindowSize()
	{
		return this->m_Window->GetWindowSize();
	}

	Point2D<int> Application::GetWindowPosition()
	{
		return this->m_Window->GetWindowPosition();
	}

	bool Application::IsWindowFocussed()
	{
		return m_Window->IsFocussed();
	}

	WindowState Application::GetCurrentWindowState()
	{
		return m_Window->GetCurrentWindowState();
	}

	void Application::SetIsMouseVisible(bool visible)
	{
		m_Window->SetIsMouseVisible(visible);
	}

	void Application::SetCursor(Cursor cursor)
	{
		m_Window->SetCursor(cursor);
	}

	void Application::Close()
	{
		m_Window->Close();
	}

	bool Application::ShouldClose()
	{
		return m_Window->IsClosing();
	}

	const InputState *Application::GetCoreInputState() const
	{
		return m_Window->GetInput();
	}

	Graphics::GraphicsDevice *Application::GetGraphicsDevice()
	{
		return m_GraphicsDevice;
	}

	Audio::AudioDevice *Application::GetAudioDevice()
	{
		return m_AudioDevice;
	}

	const Keyboard &Application::GetGlobalKeyboardState() const
	{
		return m_GlobalKeyboardState;
	}

	Graphics::GraphicsDevice *CreateGraphicsDevice(const Graphics::GraphicsDeviceSpecification &createInfo,
												   Window									   *window,
												   const Graphics::SwapchainSpecification	   &swapchainSpec)
	{
		switch (createInfo.API)
		{
#if defined(NX_PLATFORM_D3D12)
			case Graphics::GraphicsAPI::D3D12: return new Graphics::GraphicsDeviceD3D12(createInfo, window, swapchainSpec);
#endif

#if defined(NX_PLATFORM_OPENGL)
			case Graphics::GraphicsAPI::OpenGL: return new Graphics::GraphicsDeviceOpenGL(createInfo, window, swapchainSpec);
#endif

#if defined(NX_PLATFORM_VULKAN)
			case Graphics::GraphicsAPI::Vulkan: return new Graphics::GraphicsDeviceVk(createInfo, window, swapchainSpec);
#endif

			case Graphics::GraphicsAPI::Software: return new Graphics::GraphicsDeviceSoftware(createInfo, window, swapchainSpec);

			default: throw std::runtime_error("Attempting to run application with unsupported graphics API"); return nullptr;
		}
	}

	Audio::AudioDevice *CreateAudioDevice(Audio::AudioAPI api)
	{
		switch (api)
		{
#if defined(NX_PLATFORM_OPENAL)
			case Audio::AudioAPI::OpenAL: return new Audio::AudioDeviceOpenAL();
#endif

			default: throw std::runtime_error("Attempting to run application with unsupported audio API"); return nullptr;
		}
	}
}	 // namespace Nexus