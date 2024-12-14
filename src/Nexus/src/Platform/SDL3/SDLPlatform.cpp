#include "Nexus-Core/Platform.hpp"

#include "SDL3Include.hpp"
#include "SDL3Window.hpp"

#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/InputState.hpp"
#include "Nexus-Core/Platform.hpp"

std::vector<Nexus::IWindow *>					   m_Windows {};
std::vector<std::pair<Nexus::IWindow *, uint32_t>> m_WindowsToClose {};

std::optional<uint32_t> m_ActiveMouse	 = {};
std::optional<uint32_t> m_ActiveKeyboard = {};
std::optional<uint32_t> m_ActiveGamepad	 = {};

std::map<Nexus::Platform::Cursor, SDL_Cursor *> m_Cursors;

Nexus::IWindow *GetWindowFromHandle(uint32_t handle)
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

void CheckForClosingWindows()
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

void CloseWindows()
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

void PollEvents()
{
	for (auto window : m_Windows) { window->CacheInput(); }

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

				Nexus::KeyPressedEventArgs keyPressedEvent {.KeyCode	= nexusKeyCode,
															.ScanCode	= nexusScanCode,
															.Repeat		= event.key.repeat,
															.Unicode	= event.key.keysym.sym,
															.Mods		= mods,
															.KeyboardID = event.kdevice.which};

				m_ActiveKeyboard = event.kdevice.which;
				window->OnKeyPressed.Invoke(keyPressedEvent);
				break;
			}
			case SDL_EVENT_KEY_UP:
			{
				auto nexusKeyCode  = Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.keysym.sym);
				auto nexusScanCode = Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.keysym.scancode);

				Nexus::KeyReleasedEventArgs keyReleasedEvent {.KeyCode	  = nexusKeyCode,
															  .ScanCode	  = nexusScanCode,
															  .Unicode	  = event.key.keysym.sym,
															  .KeyboardID = event.kdevice.which};

				m_ActiveKeyboard = event.kdevice.which;
				window->OnKeyReleased.Invoke(keyReleasedEvent);
				break;
			}
			case SDL_EVENT_WINDOW_HIDDEN:
			{
				window->OnHide.Invoke();
				break;
			}
			case SDL_EVENT_WINDOW_SHOWN:
			{
				window->OnShow.Invoke();
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				auto [mouseType, mouseId]				 = Nexus::SDL3::GetMouseInfo(event.button.which);
				std::optional<Nexus::MouseButton> button = Nexus::SDL3::GetMouseButton(event.button.button);

				if (button.has_value())
				{
					Nexus::MouseButtonPressedEventArgs mousePressedEvent {.Button	= button.value(),
																		  .Position = {event.button.x, event.button.y},
																		  .Clicks	= event.button.clicks,
																		  .MouseID	= mouseId,
																		  .Type		= mouseType};

					m_ActiveMouse = mouseId;
					window->OnMousePressed.Invoke(mousePressedEvent);
				}

				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				auto [mouseType, mouseId]				 = Nexus::SDL3::GetMouseInfo(event.button.which);
				std::optional<Nexus::MouseButton> button = Nexus::SDL3::GetMouseButton(event.button.button);

				if (button.has_value())
				{
					Nexus::MouseButtonReleasedEventArgs mouseReleasedEvent {.Button	  = button.value(),
																			.Position = {event.button.x, event.button.y},
																			.MouseID  = mouseId,
																			.Type	  = mouseType};

					m_ActiveMouse = mouseId;
					window->OnMouseReleased.Invoke(mouseReleasedEvent);
				}

				break;
			}
			case SDL_EVENT_MOUSE_MOTION:
			{
				float mouseX	= event.motion.x;
				float mouseY	= event.motion.y;
				float movementX = event.motion.xrel;
				float movementY = event.motion.yrel;

#if defined(__EMSCRIPTEN__)
				float scale = window->GetDisplayScale();
				mouseX *= scale;
				mouseY *= scale;
				movementX *= scale;
				movementY *= scale;
#endif

				auto [mouseType, mouseId] = Nexus::SDL3::GetMouseInfo(event.motion.which);

				Nexus::MouseMovedEventArgs mouseMovedEvent {.Position = {mouseX, mouseY},
															.Movement = {movementX, movementY},
															.MouseID  = mouseId,
															.Type	  = mouseType};

				m_ActiveMouse = mouseId;
				window->OnMouseMoved.Invoke(mouseMovedEvent);
				break;
			}
			case SDL_EVENT_MOUSE_WHEEL:
			{
				auto [mouseType, mouseId]		 = Nexus::SDL3::GetMouseInfo(event.wheel.which);
				Nexus::ScrollDirection direction = Nexus::SDL3::GetScrollDirection(event.wheel.direction);

				Nexus::MouseScrolledEventArgs scrollEvent {.Scroll	  = {event.wheel.x, event.wheel.y},
														   .Position  = {event.wheel.mouse_x, event.wheel.mouse_y},
														   .MouseID	  = mouseId,
														   .Type	  = mouseType,
														   .Direction = direction};

				m_ActiveMouse = mouseId;
				window->OnScroll.Invoke(scrollEvent);
				break;
			}
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				window->Close();
				break;
			}
			case SDL_EVENT_TEXT_INPUT:
			{
				Nexus::TextInputEventArgs args {.Text = event.text.text};

				window->OnTextInput.Invoke(args);
				break;
			}
			case SDL_EVENT_TEXT_EDITING:
			{
				Nexus::TextEditEventArgs textEditArgs {.Text = event.edit.text, .Start = event.edit.start, .Length = event.edit.length};
				window->OnTextEdit.Invoke(textEditArgs);
				break;
			}
			case SDL_EVENT_WINDOW_RESIZED:
			{
				Nexus::WindowResizedEventArgs resizeEventArgs {.Size = {(uint32_t)event.window.data1, (uint32_t)event.window.data2}};
				window->OnResize.Invoke(resizeEventArgs);
				break;
			}
			case SDL_EVENT_WINDOW_MOVED:
			{
				Nexus::WindowMovedEventArgs movedEventArgs {.Position = {event.window.data1, event.window.data2}};
				window->OnMove.Invoke(movedEventArgs);
				break;
			}
			case SDL_EVENT_DROP_FILE:
			{
				Nexus::FileDropType type	   = Nexus::SDL3::GetFileDropType(event.drop.type);
				std::string			sourceApp  = {};
				std::string			sourceData = {};

				if (event.drop.source)
				{
					sourceApp = event.drop.source;
				}

				if (event.drop.data)
				{
					sourceData = event.drop.data;
				}

				Nexus::FileDropEventArgs fileDropEvent {.Type	   = type,
														.Position  = {event.drop.x, event.drop.y},
														.SourceApp = sourceApp,
														.Data	   = sourceData};

				window->OnFileDrop.Invoke(fileDropEvent);
				break;
			}
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
			{
				window->OnGainFocus.Invoke();
				break;
			}
			case SDL_EVENT_WINDOW_FOCUS_LOST:
			{
				window->OnLostFocus.Invoke();
				break;
			}
			case SDL_EVENT_WINDOW_MAXIMIZED:
			{
				window->OnMaximized.Invoke();
				break;
			}
			case SDL_EVENT_WINDOW_MINIMIZED:
			{
				window->OnMinimized.Invoke();
				break;
			}
			case SDL_EVENT_WINDOW_RESTORED:
			{
				window->OnRestored.Invoke();
				break;
			}
			case SDL_EVENT_WINDOW_MOUSE_ENTER:
			{
				window->OnMouseEnter.Invoke();
				break;
			}
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			{
				window->OnMouseLeave.Invoke();
				break;
			}
			case SDL_EVENT_KEYBOARD_ADDED:
			{
				Nexus::Platform::OnKeyboardAdded.Invoke(event.kdevice.which);
				break;
			}
			case SDL_EVENT_KEYBOARD_REMOVED:
			{
				Nexus::Platform::OnKeyboardRemoved.Invoke(event.kdevice.which);
				break;
			}
			case SDL_EVENT_MOUSE_ADDED:
			{
				Nexus::Platform::OnMouseAdded.Invoke(event.mdevice.which);
				break;
			}
			case SDL_EVENT_MOUSE_REMOVED:
			{
				Nexus::Platform::OnMouseRemoved.Invoke(event.mdevice.which);
				break;
			}
			case SDL_EVENT_GAMEPAD_ADDED:
			{
				Nexus::Platform::OnGamepadAdded.Invoke(event.cdevice.which);
				break;
			}
			case SDL_EVENT_GAMEPAD_REMOVED:
			{
				Nexus::Platform::OnGamepadRemoved.Invoke(event.cdevice.which);
				break;
			}
		}
	}
}

namespace Nexus::Platform
{
	void SetCursor(Cursor cursor)
	{
		SDL_SetCursor(m_Cursors[cursor]);
	}

	std::vector<InputNew::Keyboard> GetKeyboards()
	{
		std::vector<InputNew::Keyboard> keyboards;

		int				count;
		SDL_KeyboardID *sdlKeyboards = SDL_GetKeyboards(&count);

		for (int i = 0; i < count; i++)
		{
			uint32_t id = sdlKeyboards[i];

			if (const char *instanceName = SDL_GetKeyboardInstanceName(id))
			{
				std::string name = instanceName;
				keyboards.emplace_back(sdlKeyboards[i], name);
			}
			else
			{
				NX_ERROR(SDL_GetError());
			}
		}

		SDL_free(sdlKeyboards);

		return keyboards;
	}

	std::vector<InputNew::Mouse> GetMice()
	{
		std::vector<InputNew::Mouse> mice;

		int			 count;
		SDL_MouseID *sdlMice = SDL_GetMice(&count);

		for (int i = 0; i < count; i++)
		{
			uint32_t id = sdlMice[i];

			if (const char *instanceName = SDL_GetMouseInstanceName(id))
			{
				std::string name = instanceName;
				mice.emplace_back(sdlMice[i], name);
			}
			else
			{
				NX_ERROR(SDL_GetError());
			}
		}

		SDL_free(sdlMice);

		return mice;
	}

	std::vector<InputNew::Gamepad> GetGamepads()
	{
		std::vector<InputNew::Gamepad> gamepads;

		int				count;
		SDL_JoystickID *sdlGamepads = SDL_GetGamepads(&count);

		for (int i = 0; i < count; i++)
		{
			if (SDL_IsGamepad(sdlGamepads[i]))
			{
				uint32_t id = sdlGamepads[i];

				if (const char *instanceName = SDL_GetGamepadInstanceName(id))
				{
					std::string name = instanceName;
					gamepads.emplace_back(sdlGamepads[i], name);
				}
			}
		}

		SDL_free(sdlGamepads);

		return gamepads;
	}

	std::vector<Monitor> GetMonitors()
	{
		std::vector<Monitor> monitors;

		int			   displayCount;
		SDL_DisplayID *displays = SDL_GetDisplays(&displayCount);

		for (int i = 0; i < displayCount; i++)
		{
			SDL_DisplayID id = displays[i];

			// Monitor monitor;
			float		dpi	 = SDL_GetDisplayContentScale(id);
			std::string name = SDL_GetDisplayName(id);

			SDL_Rect displayBounds;
			SDL_GetDisplayBounds(id, &displayBounds);

			SDL_Rect usableBounds;
			SDL_GetDisplayUsableBounds(id, &usableBounds);

			const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(id);

			Monitor monitor {.Position	   = {displayBounds.x, displayBounds.y},
							 .Size		   = {displayBounds.w, displayBounds.h},
							 .WorkPosition = {usableBounds.x, usableBounds.y},
							 .WorkSize	   = {usableBounds.w, usableBounds.h},
							 .DPI		   = dpi,
							 .RefreshRate  = displayMode->refresh_rate,
							 .Name		   = name};

			monitors.push_back(monitor);
		}

		SDL_free(displays);

		return monitors;
	}

	std::optional<InputNew::Keyboard> GetKeyboardById(uint32_t id)
	{
		const std::vector<InputNew::Keyboard> &keyboards = Platform::GetKeyboards();

		for (const auto &keyboard : keyboards)
		{
			if (keyboard.GetId() == id)
			{
				return keyboard;
			}
		}

		return {};
	}

	std::optional<InputNew::Mouse> GetMouseById(uint32_t id)
	{
		const std::vector<InputNew::Mouse> &mice = Platform::GetMice();

		for (const auto &mouse : mice)
		{
			if (mouse.GetId() == id)
			{
				return mouse;
			}
		}

		return {};
	}

	std::optional<InputNew::Gamepad> GetGamepadById(uint32_t id)
	{
		const std::vector<InputNew::Gamepad> &gamepads = Platform::GetGamepads();

		for (const auto &gamepad : gamepads)
		{
			if (gamepad.GetId() == id)
			{
				return gamepad;
			}
		}

		return {};
	}

	void CreateCursors(std::map<Cursor, SDL_Cursor *> &cursors)
	{
		m_Cursors[Cursor::Arrow]	   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		m_Cursors[Cursor::IBeam]	   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		m_Cursors[Cursor::Wait]		   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
		m_Cursors[Cursor::Crosshair]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
		m_Cursors[Cursor::WaitArrow]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
		m_Cursors[Cursor::ArrowNWSE]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
		m_Cursors[Cursor::ArrowNESW]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
		m_Cursors[Cursor::ArrowWE]	   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
		m_Cursors[Cursor::ArrowNS]	   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
		m_Cursors[Cursor::ArrowAllDir] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		m_Cursors[Cursor::No]		   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
		m_Cursors[Cursor::Hand]		   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	}

	void ReleaseCursors(std::map<Cursor, SDL_Cursor *> &cursors)
	{
		for (auto &pair : cursors) { SDL_DestroyCursor(pair.second); }
		cursors.clear();
	}

	void Initialise()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD) != 0)
		{
			NX_LOG("Could not initialize SDL");
		}

		SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
		SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
		SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_PLAYER_LED, "1");

		CreateCursors(m_Cursors);
	}

	void Shutdown()
	{
		ReleaseCursors(m_Cursors);
		SDL_Quit();
	}

	void Update()
	{
		CloseWindows();
		for (auto window : m_Windows) { window->Update(); }
		CheckForClosingWindows();
		PollEvents();
	}

	IWindow *CreatePlatformWindow(const WindowSpecification				 &windowProps,
								  Graphics::GraphicsAPI					  api,
								  const Graphics::SwapchainSpecification &swapchainSpec)
	{
		IWindow *window = new IWindow(windowProps, api, swapchainSpec);
		m_Windows.push_back(window);
		return window;
	}

	InputNew::MouseInfo GetGlobalMouseInfo()
	{
		InputNew::MouseInfo state = {};

		float  x, y;
		Uint32 buttons = SDL_GetGlobalMouseState(&x, &y);

		state.Position.X = (int32_t)x;
		state.Position.Y = (int32_t)y;

		state.Buttons[MouseButton::Left]   = buttons & SDL_BUTTON_LEFT ? Nexus::MouseButtonState::Pressed : Nexus::MouseButtonState::Released;
		state.Buttons[MouseButton::Middle] = buttons & SDL_BUTTON_MIDDLE ? Nexus::MouseButtonState::Pressed : Nexus::MouseButtonState::Released;
		state.Buttons[MouseButton::Right]  = buttons & SDL_BUTTON_RIGHT ? Nexus::MouseButtonState::Pressed : Nexus::MouseButtonState::Released;
		state.Buttons[MouseButton::X1]	   = buttons & SDL_BUTTON_X1 ? Nexus::MouseButtonState::Pressed : Nexus::MouseButtonState::Released;
		state.Buttons[MouseButton::X2]	   = buttons & SDL_BUTTON_X2 ? Nexus::MouseButtonState::Pressed : Nexus::MouseButtonState::Released;
		return state;
	}

	std::optional<IWindow *> GetMouseFocus()
	{
		SDL_Window *focusWindow = SDL_GetMouseFocus();
		if (focusWindow == nullptr)
		{
			return {};
		}

		for (auto window : m_Windows)
		{
			if (window->GetSDLWindowHandle() == focusWindow)
			{
				return window;
			}
		}

		return {};
	}

	std::optional<uint32_t> GetActiveMouseId()
	{
		return m_ActiveMouse;
	}

	std::optional<uint32_t> GetActiveKeyboardId()
	{
		return m_ActiveKeyboard;
	}

	std::optional<uint32_t> GetActiveGamepadId()
	{
		return m_ActiveGamepad;
	}

	std::optional<IWindow *> GetKeyboardFocus()
	{
		SDL_Window *focusWindow = SDL_GetKeyboardFocus();
		if (focusWindow == nullptr)
		{
			return {};
		}

		for (auto window : m_Windows)
		{
			if (window->GetSDLWindowHandle() == focusWindow)
			{
				return window;
			}
		}

		return {};
	}
}	 // namespace Nexus::Platform