#include "Application.hpp"

// audio headers
#if defined(NX_PLATFORM_OPENAL)
	#include "Platform/OpenAL/AudioDeviceOpenAL.hpp"
#endif

#include "Nexus-Core/Input/Input.hpp"
#include "Nexus-Core/Logging/Log.hpp"

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

		m_GraphicsDevice = Nexus::Graphics::GraphicsDevice::CreateGraphicsDevice(graphicsDeviceCreateInfo);

		m_Swapchain = m_GraphicsDevice->CreateSwapchain(m_Window, spec.SwapchainSpecification);
		m_Swapchain->Initialise();
		m_Swapchain->SetVSyncState(spec.SwapchainSpecification.VSyncState);

		m_AudioDevice = Nexus::CreateAudioDevice(spec.AudioAPI);

		m_Window->OnRender.Bind([&](Nexus::TimeSpan time) { Render(time); });
		m_Window->OnUpdate.Bind([&](Nexus::TimeSpan time) { Update(time); });
		m_Window->OnTick.Bind([&](Nexus::TimeSpan time) { Tick(time); });
	}

	Application::~Application()
	{
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

	Nexus::IWindow *Application::GetPrimaryWindow()
	{
		return m_Window;
	}

	Nexus::Graphics::Swapchain *Application::GetPrimarySwapchain()
	{
		return m_Swapchain;
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