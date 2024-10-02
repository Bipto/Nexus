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

#include "Nexus-Core/Input/Input.hpp"
#include "Nexus-Core/Logging/Log.hpp"

#include "Platform/SDL3/SDL3Include.hpp"

namespace Nexus
{
	Application::Application(const ApplicationSpecification &spec)
	{
		m_Specification = spec;

		m_Window = CreateApplicationWindow(spec.WindowProperties, spec.SwapchainSpecification);
		Nexus::Input::SetInputContext(m_Window->GetInput());

		Graphics::GraphicsDeviceSpecification graphicsDeviceCreateInfo;
		graphicsDeviceCreateInfo.API = spec.GraphicsAPI;

		m_GraphicsDevice = Nexus::CreateGraphicsDevice(graphicsDeviceCreateInfo, m_Window, spec.SwapchainSpecification);

		auto swapchain = m_Window->GetSwapchain();
		swapchain->SetVSyncState(spec.SwapchainSpecification.VSyncState);

		m_AudioDevice = Nexus::CreateAudioDevice(spec.AudioAPI);

		SDL_StartTextInput();

		m_Window->OnRender += [&](Nexus::TimeSpan time) { Render(time); };
		m_Window->OnUpdate += [&](Nexus::TimeSpan time) { Update(time); };
		m_Window->OnTick += [&](Nexus::TimeSpan time) { Tick(time); };
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
		CloseWindows();

		auto windowSize = m_Window->GetWindowSize();
		if (windowSize.X != m_PreviousWindowSize.X || windowSize.Y != m_PreviousWindowSize.Y)
		{
			OnResize(windowSize);
		}

		// Allow user to block closing events, for example to display save prompt
		if (m_Window->m_Closing)
			m_Window->m_Closing = this->OnClose();

		m_Clock.Tick();
		auto time = m_Clock.GetTime();

		UpdateWindowTimers();
		CheckForClosingWindows();
		m_PreviousWindowSize = windowSize;

		this->PollEvents();
		m_GlobalKeyboardState.CacheInput();
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

	Window *Application::CreateApplicationWindow(const WindowSpecification &windowProps, const Graphics::SwapchainSpecification &swapchainSpec)
	{
		Window *window = new Nexus::Window(windowProps, m_Specification.GraphicsAPI, swapchainSpec);
		m_Windows.push_back(window);
		return window;
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

	void Application::PollEvents()
	{
		// cache the previous frame's input
		Mouse::s_PreviousGlobalMousePosition = Mouse::s_GlobalMousePosition;

		for (auto window : m_Windows) { window->m_Input.CacheInput(); }

		float  x, y;
		Uint32 buttons = SDL_GetGlobalMouseState(&x, &y);

#if defined(__EMSCRIPTEN__)
		x *= GetPrimaryWindow()->GetDisplayScale();
		y *= GetPrimaryWindow()->GetDisplayScale();
#endif

		Mouse::s_GlobalMousePosition.X = x;
		Mouse::s_GlobalMousePosition.Y = y;

		if (buttons & SDL_BUTTON_LEFT)
		{
			Mouse::s_GlobalMouseState.LeftButton = MouseButtonState::Pressed;
		}
		else
		{
			Mouse::s_GlobalMouseState.LeftButton = MouseButtonState::Released;
		}

		if (buttons & SDL_BUTTON_RIGHT)
		{
			Mouse::s_GlobalMouseState.RightButton = MouseButtonState::Pressed;
		}
		else
		{
			Mouse::s_GlobalMouseState.RightButton = MouseButtonState::Released;
		}

		if (buttons & SDL_BUTTON_MIDDLE)
		{
			Mouse::s_GlobalMouseState.MiddleButton = MouseButtonState::Pressed;
		}
		else
		{
			Mouse::s_GlobalMouseState.MiddleButton = MouseButtonState::Released;
		}

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			auto window = GetWindowFromHandle(event.window.windowID);
			if (!window)
			{
				continue;
			}

			switch (event.type)
			{
				case SDL_EVENT_KEY_DOWN:
				{
					auto nexusKeyCode  = Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.keysym.sym);
					auto nexusScanCode = Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.keysym.scancode);
					auto mods		   = Nexus::SDL3::GetNexusModifiersFromSDLModifiers(event.key.keysym.mod);

					window->m_Input.m_Keyboard.m_CurrentKeys[nexusKeyCode] = true;
					m_GlobalKeyboardState.m_CurrentKeys[nexusKeyCode]	   = true;

					KeyPressedEvent keyPressedEvent {.KeyCode	 = nexusKeyCode,
													 .ScanCode	 = nexusScanCode,
													 .Repeat	 = event.key.repeat,
													 .Unicode	 = event.key.keysym.sym,
													 .Mods		 = mods,
													 .KeyboardID = event.kdevice.which};

					window->OnKeyPressed.Invoke(keyPressedEvent);
					break;
				}
				case SDL_EVENT_KEY_UP:
				{
					auto nexusKeyCode  = Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.keysym.sym);
					auto nexusScanCode = Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.keysym.scancode);

					window->m_Input.m_Keyboard.m_CurrentKeys[nexusKeyCode] = false;
					m_GlobalKeyboardState.m_CurrentKeys[nexusKeyCode]	   = false;

					KeyReleasedEvent keyReleasedEvent {.KeyCode	   = nexusKeyCode,
													   .ScanCode   = nexusScanCode,
													   .Unicode	   = event.key.keysym.sym,
													   .KeyboardID = event.kdevice.which};

					window->OnKeyReleased.Invoke(keyReleasedEvent);
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				{
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
						{
							window->m_Input.m_Mouse.m_CurrentState.LeftButton = MouseButtonState::Pressed;
							break;
						}
						case SDL_BUTTON_RIGHT:
						{
							window->m_Input.m_Mouse.m_CurrentState.RightButton = MouseButtonState::Pressed;
							break;
						}
						case SDL_BUTTON_MIDDLE:
						{
							window->m_Input.m_Mouse.m_CurrentState.MiddleButton = MouseButtonState::Pressed;
							break;
						}
					}

					auto [mouseType, mouseId] = SDL3::GetMouseInfo(event.button.which);

					MouseButtonPressedEvent mousePressedEvent {.MouseButton = event.button.button,
															   .Position	= {event.button.x, event.button.y},
															   .Clicks		= event.button.clicks,
															   .MouseID		= mouseId,
															   .Type		= mouseType};

					window->OnMousePressed.Invoke(mousePressedEvent);
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_UP:
				{
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
						{
							window->m_Input.m_Mouse.m_CurrentState.LeftButton = MouseButtonState::Released;
							break;
						}
						case SDL_BUTTON_RIGHT:
						{
							window->m_Input.m_Mouse.m_CurrentState.RightButton = MouseButtonState::Released;
							break;
						}
						case SDL_BUTTON_MIDDLE:
						{
							window->m_Input.m_Mouse.m_CurrentState.MiddleButton = MouseButtonState::Released;
							break;
						}
					}

					auto [mouseType, mouseId] = SDL3::GetMouseInfo(event.button.which);

					MouseButtonReleasedEvent mouseReleasedEvent {.MouseButton = event.button.button,
																 .Position	  = {event.button.x, event.button.y},
																 .MouseID	  = mouseId,
																 .Type		  = mouseType};

					window->OnMouseReleased.Invoke(mouseReleasedEvent);
					break;
				}
				case SDL_EVENT_MOUSE_MOTION:
				{
					float mouseX = event.motion.x;
					float mouseY = event.motion.y;

#if defined(__EMSCRIPTEN__)
					mouseX *= GetPrimaryWindow()->GetDisplayScale();
					mouseY *= GetPrimaryWindow()->GetDisplayScale();
#endif

					window->m_Input.m_Mouse.m_CurrentState.MousePosition = {mouseX, mouseY};

					float  xPos, yPos;
					Uint32 state = SDL_GetMouseState(&xPos, &yPos);

					float movementX = xPos - window->m_Input.m_Mouse.m_CurrentState.MousePosition.X;
					float movementY = yPos - window->m_Input.m_Mouse.m_CurrentState.MousePosition.Y;

					auto [mouseType, mouseId] = SDL3::GetMouseInfo(event.motion.which);

					MouseMovedEvent mouseMovedEvent {.Position = {event.motion.x, event.motion.y},
													 .Movement = {event.motion.xrel, event.motion.yrel},
													 .MouseID  = mouseId,
													 .Type	   = mouseType};

					window->OnMouseMoved.Invoke(mouseMovedEvent);

					break;
				}
				case SDL_EVENT_MOUSE_WHEEL:
				{
					auto &scroll = window->m_Input.m_Mouse.m_CurrentState.MouseWheel;
					scroll.X += event.wheel.x;
					scroll.Y += event.wheel.y;

					auto [mouseType, mouseId] = SDL3::GetMouseInfo(event.wheel.which);
					ScrollDirection direction = SDL3::GetScrollDirection(event.wheel.direction);

					MouseScrolledEvent scrollEvent {.Scroll	   = {event.wheel.x, event.wheel.y},
													.Position  = {event.wheel.mouse_x, event.wheel.mouse_y},
													.MouseID   = mouseId,
													.Type	   = mouseType,
													.Direction = direction};

					window->OnScroll.Invoke(scrollEvent);

					SDL_MouseWheelDirection;
					break;
				}
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				{
					window->Close();
					break;
				}

				case SDL_EVENT_GAMEPAD_ADDED:
				{
					if (SDL_IsGamepad(event.cdevice.which))
					{
						Nexus::Input::AddController(event.cdevice.which);
						break;
					}
					break;
				}
				case SDL_EVENT_GAMEPAD_REMOVED:
				{
					Nexus::Input::RemoveController(event.cdevice.which);
					break;
				}
				case SDL_EVENT_TEXT_INPUT:
				{
					window->m_Input.TextInput.Invoke(event.text.text);
					OnTextInput.Invoke(event.text.text);
					break;
				}
				case SDL_EVENT_WINDOW_RESIZED:
				{
					window->OnResize.Invoke({event.window.data1, event.window.data2});
					break;
				}
				case SDL_EVENT_DROP_FILE:
				{
					FileDropType type		= SDL3::GetFileDropType(event.drop.type);
					std::string	 sourceApp	= {};
					std::string	 sourceData = {};

					if (event.drop.source)
					{
						sourceApp = event.drop.source;
					}

					if (event.drop.data)
					{
						sourceData = event.drop.data;
					}

					FileDropEvent fileDropEvent {.Type = type, .Position = {event.drop.x, event.drop.y}, .SourceApp = sourceApp, .Data = sourceData};

					window->OnFileDrop.Invoke(fileDropEvent);
					break;
				}
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
				{
					window->OnWindowGainFocus.Invoke();
					break;
				}
				case SDL_EVENT_WINDOW_FOCUS_LOST:
				{
					window->OnWindowLostFocus.Invoke();
					break;
				}
				case SDL_EVENT_WINDOW_MAXIMIZED:
				{
					window->OnWindowMaximized.Invoke();
					break;
				}
				case SDL_EVENT_WINDOW_MINIMIZED:
				{
					window->OnWindowMinimized.Invoke();
					break;
				}
				case SDL_EVENT_WINDOW_RESTORED:
				{
					window->OnWindowRestored.Invoke();
					break;
				}
			}
		}
	}

	Window *Application::GetWindowFromHandle(uint32_t handle)
	{
		for (int i = 0; i < m_Windows.size(); i++)
		{
			if (m_Windows[i]->GetID() == handle)
			{
				return m_Windows[i];
			}
		}
		return nullptr;
	}

	void Application::CheckForClosingWindows()
	{
		uint32_t indexToRemove = 0;
		for (uint32_t i = 0; i < m_Windows.size(); i++)
		{
			if (m_Windows[i]->IsClosing())
			{
				auto window = m_Windows[i];

				auto pair = std::make_pair(window, 0);
				m_WindowsToClose.push_back(pair);
			}
		}
	}

	void Application::CloseWindows()
	{
		// this is required because the swapchain's framebuffer may still be in use on
		// the GPU we need to wait until we are certain that the swapchain is no
		// longer in use before we delete it
		for (uint32_t i = 0; i < m_WindowsToClose.size(); i++) { m_WindowsToClose[i].second++; }

		for (uint32_t closingWindowIndex = 0; closingWindowIndex < m_WindowsToClose.size(); closingWindowIndex++)
		{
			auto pair = m_WindowsToClose[closingWindowIndex];

			auto window = pair.first;
			auto count	= pair.second;

			if (count > 10)
			{
				for (uint32_t windowIndex = 0; windowIndex < m_Windows.size(); windowIndex++)
				{
					if (window == m_Windows[windowIndex])
					{
						delete window;
						window = nullptr;

						m_WindowsToClose.erase(m_WindowsToClose.begin() + closingWindowIndex);
						m_Windows.erase(m_Windows.begin() + windowIndex);
					}
				}
			}
		}
	}

	void Application::UpdateWindowTimers()
	{
		for (auto window : m_Windows) { window->m_Timer.Update(); }
	}

	Graphics::GraphicsDevice *CreateGraphicsDevice(const Graphics::GraphicsDeviceSpecification &createInfo,
												   Window									   *window,
												   const Graphics::SwapchainSpecification	   &swapchainSpec)
	{
		switch (createInfo.API)
		{
#if defined(NX_PLATFORM_D3D12)
			case Graphics::GraphicsAPI::D3D12: return new Graphics::GraphicsDeviceD3D12(createInfo, window, swapchainSpec); break;
#endif

#if defined(NX_PLATFORM_OPENGL)
			case Graphics::GraphicsAPI::OpenGL: return new Graphics::GraphicsDeviceOpenGL(createInfo, window, swapchainSpec); break;
#endif

#if defined(NX_PLATFORM_VULKAN)
			case Graphics::GraphicsAPI::Vulkan: return new Graphics::GraphicsDeviceVk(createInfo, window, swapchainSpec); break;
#endif
			default:
				throw std::runtime_error("Attempting to run application with unsupported graphics API");
				return nullptr;
				break;
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