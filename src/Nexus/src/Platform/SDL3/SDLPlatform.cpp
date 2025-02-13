#include "Nexus-Core/Platform.hpp"

#include "FileDialogsSDL3.hpp"
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

void CloseWindows()
{
	for (uint32_t i = 0; i < m_Windows.size(); i++)
	{
		Nexus::IWindow *window = m_Windows[i];
		if (window->IsClosing())
		{
			std::vector<Nexus::IWindow *>::iterator position = std::find(m_Windows.begin(), m_Windows.end(), window);
			if (position != m_Windows.end())
			{
				m_Windows.erase(position);
				delete window;
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

			if (const char *instanceName = SDL_GetKeyboardNameForID(id))
			{
				std::string name = instanceName;
				keyboards.push_back(InputNew::Keyboard(sdlKeyboards[i], name));
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

			if (const char *instanceName = SDL_GetMouseNameForID(id))
			{
				std::string name = instanceName;
				mice.push_back(InputNew::Mouse(sdlMice[i], name));
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

				if (const char *instanceName = SDL_GetGamepadNameForID(id))
				{
					std::string name = instanceName;
					gamepads.push_back(InputNew::Gamepad(sdlGamepads[i], name));
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

	std::vector<IWindow *> &GetWindows()
	{
		return m_Windows;
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
		m_Cursors[Cursor::Arrow]	   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
		m_Cursors[Cursor::IBeam]	   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
		m_Cursors[Cursor::Wait]		   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
		m_Cursors[Cursor::Crosshair]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
		m_Cursors[Cursor::WaitArrow]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
		m_Cursors[Cursor::ArrowNWSE]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
		m_Cursors[Cursor::ArrowNESW]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
		m_Cursors[Cursor::ArrowWE]	   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
		m_Cursors[Cursor::ArrowNS]	   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
		m_Cursors[Cursor::ArrowAllDir] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
		m_Cursors[Cursor::No]		   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
		m_Cursors[Cursor::Hand]		   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
	}

	void ReleaseCursors(std::map<Cursor, SDL_Cursor *> &cursors)
	{
		for (auto &pair : cursors) { SDL_DestroyCursor(pair.second); }
		cursors.clear();
	}

	void Initialise()
	{
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD))
		{
			NX_LOG("Could not initialize SDL");
		}

		SDL_SetHint(SDL_HINT_JOYSTICK_ENHANCED_REPORTS, "1");
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

		for (size_t i = 0; i < m_Windows.size(); i++)
		{
			IWindow *window = m_Windows[i];
			window->Update();
		}

		PollEvents();
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
					auto nexusKeyCode  = Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.key);
					auto nexusScanCode = Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.scancode);
					auto mods		   = Nexus::SDL3::GetNexusModifiersFromSDLModifiers(event.key.mod);

					Nexus::KeyPressedEventArgs keyPressedEvent {.KeyCode	= nexusKeyCode,
																.ScanCode	= nexusScanCode,
																.Repeat		= event.key.repeat,
																.Unicode	= event.key.raw,
																.Mods		= mods,
																.KeyboardID = event.kdevice.which};

					m_ActiveKeyboard = event.kdevice.which;
					window->OnKeyPressed.Invoke(keyPressedEvent);
					break;
				}
				case SDL_EVENT_KEY_UP:
				{
					auto nexusKeyCode  = Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.key);
					auto nexusScanCode = Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.scancode);

					Nexus::KeyReleasedEventArgs keyReleasedEvent {.KeyCode	  = nexusKeyCode,
																  .ScanCode	  = nexusScanCode,
																  .Unicode	  = event.key.raw,
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

	IWindow *CreatePlatformWindow(const WindowSpecification &windowProps)
	{
		IWindow *window = new SDL3Window(windowProps);
		m_Windows.push_back(window);
		return window;
}

OpenFileDialog *CreateOpenFileDialog(IWindow *window, const std::vector<FileDialogFilter> &filters, const char *defaultLocation, bool allowMany)
{
	return new OpenFileDialogSDL3(window, filters, defaultLocation, allowMany);
}

	SaveFileDialog *CreateSaveFileDialog(IWindow *window, const std::vector<FileDialogFilter> &filters, const char *defaultLocation)
	{
		return new SaveFileDialogSDL3(window, filters, defaultLocation);
	}

	OpenFolderDialog *CreateOpenFolderDialog(IWindow *window, const char *defaultLocation, bool allowMany)
	{
		return new OpenFolderDialogSDL3(window, defaultLocation, allowMany);
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
			SDL3Window *sdlWindow = (SDL3Window *)window;
			if (sdlWindow->GetSDLWindowHandle() == focusWindow)
			{
				return window;
			}
		}

		return {};
	}

	std::optional<IWindow *> GetActiveWindow()
	{
		for (auto window : m_Windows)
		{
			if (window->IsFocussed())
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
			SDL3Window *sdlWindow = (SDL3Window *)window;
			if (sdlWindow->GetSDLWindowHandle() == focusWindow)
			{
				return window;
			}
		}

		return {};
	}
}	 // namespace Nexus::Platform