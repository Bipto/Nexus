#include "Nexus-Core/Platform.hpp"

#include "FileDialogsSDL3.hpp"
#include "SDL3Include.hpp"
#include "SDL3Window.hpp"

#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Input/Events.hpp"
#include "Nexus-Core/Platform.hpp"

#include "Nexus-Core/Input/Input.hpp"

std::vector<Nexus::IWindow *> m_Windows {};

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

namespace Nexus::Platform
{
	void SetCursor(Cursor cursor)
	{
		SDL_SetCursor(m_Cursors[cursor]);
	}

	std::vector<Keyboard> GetKeyboards()
	{
		std::vector<Keyboard> keyboards;

		int				count;
		SDL_KeyboardID *sdlKeyboards = SDL_GetKeyboards(&count);

		for (int i = 0; i < count; i++)
		{
			uint32_t id = sdlKeyboards[i];

			if (const char *instanceName = SDL_GetKeyboardNameForID(id))
			{
				std::string name = instanceName;
				keyboards.push_back(Keyboard(sdlKeyboards[i], name));
			}
			else
			{
				NX_ERROR(SDL_GetError());
			}
		}

		SDL_free(sdlKeyboards);

		return keyboards;
	}

	std::vector<Mouse> GetMice()
	{
		std::vector<Mouse> mice;

		int			 count;
		SDL_MouseID *sdlMice = SDL_GetMice(&count);

		for (int i = 0; i < count; i++)
		{
			uint32_t id = sdlMice[i];

			if (const char *instanceName = SDL_GetMouseNameForID(id))
			{
				std::string name = instanceName;
				mice.push_back(Mouse(sdlMice[i], name));
			}
			else
			{
				NX_ERROR(SDL_GetError());
			}
		}

		SDL_free(sdlMice);

		return mice;
	}

	std::vector<Gamepad> GetGamepads()
	{
		std::vector<Gamepad> gamepads;

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
					gamepads.push_back(Gamepad(sdlGamepads[i], name));
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

	std::optional<Keyboard> GetKeyboardById(uint32_t id)
	{
		const std::vector<Keyboard> &keyboards = Platform::GetKeyboards();

		for (const auto &keyboard : keyboards)
		{
			if (keyboard.GetId() == id)
			{
				return keyboard;
			}
		}

		return {};
	}

	std::optional<Mouse> GetMouseById(uint32_t id)
	{
		const std::vector<Mouse> &mice = Platform::GetMice();

		for (const auto &mouse : mice)
		{
			if (mouse.GetId() == id)
			{
				return mouse;
			}
		}

		return {};
	}

	std::optional<Gamepad> GetGamepadById(uint32_t id)
	{
		const std::vector<Gamepad> &gamepads = Platform::GetGamepads();

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
		for (size_t i = 0; i < m_Windows.size(); i++)
		{
			IWindow *window = m_Windows[i];
			window->Update();
		}
	}

	void HandleEvent(SDL_Event &event, Application *app)
	{
		auto window = (SDL3Window *)GetWindowFromHandle(event.window.windowID);
		if (!window)
		{
			return;
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
				window->InvokeKeyPressedCallback(keyPressedEvent);
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
				window->InvokeKeyReleasedCallback(keyReleasedEvent);
				break;
			}
			case SDL_EVENT_WINDOW_HIDDEN:
			{
				window->InvokeHideCallback();
				break;
			}
			case SDL_EVENT_WINDOW_SHOWN:
			{
				window->InvokeShowCallback();
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
					window->InvokeMousePressedCallback(mousePressedEvent);
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
					window->InvokeMouseReleasedCallback(mouseReleasedEvent);
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
				window->InvokeMouseMovedCallback(mouseMovedEvent);
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
				window->InvokeMouseScrollCallback(scrollEvent);
				break;
			}
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				window->Close();

				if (m_Windows.size() <= 1)
				{
					app->Close();
				}

				break;
			}
			case SDL_EVENT_TEXT_INPUT:
			{
				Nexus::TextInputEventArgs args {.Text = event.text.text};
				window->InvokeTextInputCallback(args);
				break;
			}
			case SDL_EVENT_TEXT_EDITING:
			{
				Nexus::TextEditEventArgs textEditArgs {.Text = event.edit.text, .Start = event.edit.start, .Length = event.edit.length};
				window->InvokeTextEditCallback(textEditArgs);
				break;
			}
			case SDL_EVENT_WINDOW_RESIZED:
			{
				Nexus::WindowResizedEventArgs resizeEventArgs {.Size = {(uint32_t)event.window.data1, (uint32_t)event.window.data2}};
				window->InvokeResizeCallback(resizeEventArgs);
				break;
			}
			case SDL_EVENT_WINDOW_MOVED:
			{
				Nexus::WindowMovedEventArgs movedEventArgs {.Position = {event.window.data1, event.window.data2}};
				window->InvokeMoveCallback(movedEventArgs);
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

				window->InvokeFileDropCallback(fileDropEvent);
				break;
			}
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
			{
				window->InvokeFocusGainCallback();
				break;
			}
			case SDL_EVENT_WINDOW_FOCUS_LOST:
			{
				window->InvokeFocusLostCallback();
				break;
			}
			case SDL_EVENT_WINDOW_MAXIMIZED:
			{
				window->InvokeMaximizedCallback();
				break;
			}
			case SDL_EVENT_WINDOW_MINIMIZED:
			{
				window->InvokeMinimizedCallback();
				break;
			}
			case SDL_EVENT_WINDOW_RESTORED:
			{
				window->InvokeRestoreCallback();
				break;
			}
			case SDL_EVENT_WINDOW_MOUSE_ENTER:
			{
				window->InvokeMouseEnterCallback();
				break;
			}
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			{
				window->InvokeMouseLeaveCallback();
				break;
			}
			case SDL_EVENT_WINDOW_EXPOSED:
			{
				window->InvokeExposeCallback();
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

	void PollEvents(Application *app)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) { HandleEvent(event, app); }
	}

	void WaitEvent(Application *app)
	{
		SDL_Event event;
		// while (SDL_WaitEvent(&event)) { HandleEvent(event, app); }
		if (SDL_WaitEvent(&event))
		{
			HandleEvent(event, app);
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

	MouseState GetMouseState()
	{
		float  x, y;
		Uint32 buttons = SDL_GetGlobalMouseState(&x, &y);

		MouseState state;
		state.MousePosition.X = x;
		state.MousePosition.Y = y;
		state.MouseWheel	  = {0, 0};
		state.LeftButton	  = MouseButtonState::Released;
		state.MiddleButton	  = MouseButtonState::Released;
		state.RightButton	  = MouseButtonState::Released;
		state.X1Button		  = MouseButtonState::Released;
		state.X2Button		  = MouseButtonState::Released;

		if (buttons & SDL_BUTTON_LEFT)
		{
			state.LeftButton = MouseButtonState::Pressed;
		}

		if (buttons & SDL_BUTTON_MIDDLE)
		{
			state.MiddleButton = MouseButtonState::Pressed;
		}

		if (buttons & SDL_BUTTON_RIGHT)
		{
			state.RightButton = MouseButtonState::Pressed;
		}

		if (buttons & SDL_BUTTON_X1)
		{
			state.X1Button = MouseButtonState::Pressed;
		}

		if (buttons & SDL_BUTTON_X2)
		{
			state.X2Button = MouseButtonState::Pressed;
		}

		return state;
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