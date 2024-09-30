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

namespace Nexus
{
	static KeyCode SDLToNexusKeycode(SDL_Keycode keycode)
	{
		switch (keycode)
		{
			case SDLK_ESCAPE: return KeyCode::Escape;

			case SDLK_F1: return KeyCode::F1;
			case SDLK_F2: return KeyCode::F2;
			case SDLK_F3: return KeyCode::F3;
			case SDLK_F4: return KeyCode::F4;
			case SDLK_F5: return KeyCode::F5;
			case SDLK_F6: return KeyCode::F6;
			case SDLK_F7: return KeyCode::F7;
			case SDLK_F8: return KeyCode::F8;
			case SDLK_F9: return KeyCode::F9;
			case SDLK_F10: return KeyCode::F10;
			case SDLK_F11: return KeyCode::F11;
			case SDLK_F12: return KeyCode::F12;

			case SDLK_PRINTSCREEN: return KeyCode::PrintScreen;
			case SDLK_SCROLLLOCK: return KeyCode::ScrollLock;
			case SDLK_PAUSE: return KeyCode::Pause;

			case SDLK_BACKQUOTE: return KeyCode::BackQuote;
			case SDLK_1: return KeyCode::One;
			case SDLK_2: return KeyCode::Two;
			case SDLK_3: return KeyCode::Three;
			case SDLK_4: return KeyCode::Four;
			case SDLK_5: return KeyCode::Five;
			case SDLK_6: return KeyCode::Six;
			case SDLK_7: return KeyCode::Seven;
			case SDLK_8: return KeyCode::Eight;
			case SDLK_9: return KeyCode::Nine;
			case SDLK_0: return KeyCode::Zero;
			case SDLK_UNDERSCORE: return KeyCode::Underscore;
			case SDLK_EQUALS: return KeyCode::Equals;
			case SDLK_BACKSPACE: return KeyCode::Backspace;

			case SDLK_TAB: return KeyCode::Tab;
			case SDLK_q: return KeyCode::Q;
			case SDLK_w: return KeyCode::W;
			case SDLK_e: return KeyCode::E;
			case SDLK_r: return KeyCode::R;
			case SDLK_t: return KeyCode::T;
			case SDLK_y: return KeyCode::Y;
			case SDLK_u: return KeyCode::U;
			case SDLK_i: return KeyCode::I;
			case SDLK_o: return KeyCode::O;
			case SDLK_p: return KeyCode::P;
			case SDLK_LEFTBRACKET: return KeyCode::LeftBracket;
			case SDLK_RIGHTBRACKET: return KeyCode::RightBracket;
			case SDLK_RETURN: return KeyCode::Return;

			case SDLK_CAPSLOCK: return KeyCode::CapsLock;
			case SDLK_a: return KeyCode::A;
			case SDLK_s: return KeyCode::S;
			case SDLK_d: return KeyCode::D;
			case SDLK_f: return KeyCode::F;
			case SDLK_g: return KeyCode::G;
			case SDLK_h: return KeyCode::H;
			case SDLK_j: return KeyCode::J;
			case SDLK_k: return KeyCode::K;
			case SDLK_l: return KeyCode::L;
			case SDLK_SEMICOLON: return KeyCode::Semicolon;
			case SDLK_QUOTE: return KeyCode::Quote;
			case SDLK_HASH: return KeyCode::Hash;

			case SDLK_LSHIFT: return KeyCode::LeftShift;
			case SDLK_BACKSLASH: return KeyCode::BackSlash;
			case SDLK_z: return KeyCode::Z;
			case SDLK_x: return KeyCode::X;
			case SDLK_c: return KeyCode::C;
			case SDLK_v: return KeyCode::V;
			case SDLK_b: return KeyCode::B;
			case SDLK_n: return KeyCode::N;
			case SDLK_m: return KeyCode::M;
			case SDLK_COMMA: return KeyCode::Comma;
			case SDLK_PERIOD: return KeyCode::Period;
			case SDLK_SLASH: return KeyCode::Slash;
			case SDLK_RSHIFT: return KeyCode::RightShift;

			case SDLK_LCTRL: return KeyCode::LeftControl;
			case SDLK_LGUI: return KeyCode::LeftGUI;
			case SDLK_LALT: return KeyCode::LeftAlt;
			case SDLK_SPACE: return KeyCode::Space;
			case SDLK_RALT: return KeyCode::RightAlt;
			case SDLK_RGUI: return KeyCode::PrintScreen;
			case SDLK_RCTRL: return KeyCode::RightControl;

			case SDLK_INSERT: return KeyCode::Insert;
			case SDLK_HOME: return KeyCode::Home;
			case SDLK_PAGEUP: return KeyCode::PageUp;
			case SDLK_DELETE: return KeyCode::Delete;
			case SDLK_END: return KeyCode::End;
			case SDLK_PAGEDOWN: return KeyCode::PageDown;

			case SDLK_LEFT: return KeyCode::Left;
			case SDLK_UP: return KeyCode::Up;
			case SDLK_DOWN: return KeyCode::Down;
			case SDLK_RIGHT: return KeyCode::Right;

			case SDLK_NUMLOCKCLEAR: return KeyCode::NumLockClear;
			case SDLK_KP_DIVIDE: return KeyCode::KeyPad_Divide;
			case SDLK_KP_MULTIPLY: return KeyCode::KeyPad_Multiply;
			case SDLK_KP_MEMSUBTRACT: return KeyCode::Keypad_MemSubtract;
			case SDLK_KP_7: return KeyCode::KeyPad_Seven;
			case SDLK_KP_8: return KeyCode::KeyPad_Eight;
			case SDLK_KP_9: return KeyCode::KeyPad_Nine;
			case SDLK_KP_PLUS: return KeyCode::KeyPad_Plus;
			case SDLK_KP_4: return KeyCode::KeyPad_Four;
			case SDLK_KP_5: return KeyCode::KeyPad_Five;
			case SDLK_KP_6: return KeyCode::KeyPad_Six;
			case SDLK_KP_1: return KeyCode::KeyPad_One;
			case SDLK_KP_2: return KeyCode::KeyPad_Two;
			case SDLK_KP_3: return KeyCode::KeyPad_Three;
			case SDLK_RETURN2: return KeyCode::Return2;
			case SDLK_KP_0: return KeyCode::KeyPad_Zero;
			case SDLK_KP_PERIOD: return KeyCode::Period;

			default: throw std::runtime_error("Failed to find a valid keycode");
		}
	}

	static ScanCode SDLToNexusScancode(SDL_Scancode scancode)
	{
		switch (scancode)
		{
			case SDL_SCANCODE_A: break;
			case SDL_SCANCODE_B: break;
			case SDL_SCANCODE_C: break;
			case SDL_SCANCODE_D: break;
			case SDL_SCANCODE_E: break;
			case SDL_SCANCODE_F: break;
			case SDL_SCANCODE_G: break;
			case SDL_SCANCODE_H: break;
			case SDL_SCANCODE_I: break;
			case SDL_SCANCODE_J: break;
			case SDL_SCANCODE_K: break;
			case SDL_SCANCODE_L: break;
			case SDL_SCANCODE_M: break;
			case SDL_SCANCODE_N: break;
			case SDL_SCANCODE_O: break;
			case SDL_SCANCODE_P: break;
			case SDL_SCANCODE_Q: break;
			case SDL_SCANCODE_R: break;
			case SDL_SCANCODE_S: break;
			case SDL_SCANCODE_T: break;
			case SDL_SCANCODE_U: break;
			case SDL_SCANCODE_V: break;
			case SDL_SCANCODE_W: break;
			case SDL_SCANCODE_X: break;
			case SDL_SCANCODE_Y: break;
			case SDL_SCANCODE_Z: break;
			case SDL_SCANCODE_1: break;
			case SDL_SCANCODE_2: break;
			case SDL_SCANCODE_3: break;
			case SDL_SCANCODE_4: break;
			case SDL_SCANCODE_5: break;
			case SDL_SCANCODE_6: break;
			case SDL_SCANCODE_7: break;
			case SDL_SCANCODE_8: break;
			case SDL_SCANCODE_9: break;
			case SDL_SCANCODE_0: break;
			case SDL_SCANCODE_RETURN: break;
			case SDL_SCANCODE_ESCAPE: break;
			case SDL_SCANCODE_BACKSPACE: break;
			case SDL_SCANCODE_TAB: break;
			case SDL_SCANCODE_SPACE: break;
			case SDL_SCANCODE_MINUS: break;
			case SDL_SCANCODE_EQUALS: break;
			case SDL_SCANCODE_LEFTBRACKET: break;
			case SDL_SCANCODE_RIGHTBRACKET: break;
			case SDL_SCANCODE_BACKSLASH: break;
			case SDL_SCANCODE_NONUSHASH: break;

			case SDL_SCANCODE_SEMICOLON: break;
			case SDL_SCANCODE_APOSTROPHE: break;
			case SDL_SCANCODE_GRAVE: break;
			case SDL_SCANCODE_COMMA: break;
			case SDL_SCANCODE_PERIOD: break;
			case SDL_SCANCODE_SLASH: break;
			case SDL_SCANCODE_CAPSLOCK: break;
			case SDL_SCANCODE_F1: break;
			case SDL_SCANCODE_F2: break;
			case SDL_SCANCODE_F3: break;
			case SDL_SCANCODE_F4: break;
			case SDL_SCANCODE_F5: break;
			case SDL_SCANCODE_F6: break;
			case SDL_SCANCODE_F7: break;
			case SDL_SCANCODE_F8: break;
			case SDL_SCANCODE_F9: break;
			case SDL_SCANCODE_F10: break;
			case SDL_SCANCODE_F11: break;
			case SDL_SCANCODE_F12: break;
			case SDL_SCANCODE_PRINTSCREEN: break;
			case SDL_SCANCODE_SCROLLLOCK: break;
			case SDL_SCANCODE_PAUSE: break;
			case SDL_SCANCODE_INSERT: break;
			case SDL_SCANCODE_HOME: break;
			case SDL_SCANCODE_PAGEUP: break;
			case SDL_SCANCODE_DELETE: break;
			case SDL_SCANCODE_END: break;
			case SDL_SCANCODE_PAGEDOWN: break;
			case SDL_SCANCODE_RIGHT: break;
			case SDL_SCANCODE_LEFT: break;
			case SDL_SCANCODE_DOWN: break;
			case SDL_SCANCODE_UP: break;
			case SDL_SCANCODE_NUMLOCKCLEAR: break;

			case SDL_SCANCODE_KP_DIVIDE: break;
			case SDL_SCANCODE_KP_MULTIPLY: break;
			case SDL_SCANCODE_KP_MINUS: break;
			case SDL_SCANCODE_KP_PLUS: break;
			case SDL_SCANCODE_KP_ENTER: break;
			case SDL_SCANCODE_KP_1: break;
			case SDL_SCANCODE_KP_2: break;
			case SDL_SCANCODE_KP_3: break;
			case SDL_SCANCODE_KP_4: break;
			case SDL_SCANCODE_KP_5: break;
			case SDL_SCANCODE_KP_6: break;
			case SDL_SCANCODE_KP_7: break;
			case SDL_SCANCODE_KP_8: break;
			case SDL_SCANCODE_KP_9: break;
			case SDL_SCANCODE_KP_0: break;
			case SDL_SCANCODE_KP_PERIOD: break;
			case SDL_SCANCODE_NONUSBACKSLASH: break;
			case SDL_SCANCODE_APPLICATION: break;
			case SDL_SCANCODE_KP_EQUALS: break;
			case SDL_SCANCODE_F13: break;
			case SDL_SCANCODE_F14: break;
			case SDL_SCANCODE_F15: break;
			case SDL_SCANCODE_F16: break;
			case SDL_SCANCODE_F17: break;
			case SDL_SCANCODE_F18: break;
			case SDL_SCANCODE_F19: break;
			case SDL_SCANCODE_F20: break;
			case SDL_SCANCODE_F21: break;
			case SDL_SCANCODE_F22: break;
			case SDL_SCANCODE_F23: break;
			case SDL_SCANCODE_F24: break;
			case SDL_SCANCODE_EXECUTE: break;
			case SDL_SCANCODE_HELP: break;
			case SDL_SCANCODE_MENU: break;
			case SDL_SCANCODE_SELECT: break;
			case SDL_SCANCODE_STOP: break;
			case SDL_SCANCODE_AGAIN: break;
			case SDL_SCANCODE_UNDO: break;
			case SDL_SCANCODE_CUT: break;
			case SDL_SCANCODE_COPY: break;
			case SDL_SCANCODE_PASTE: break;
			case SDL_SCANCODE_FIND: break;
			case SDL_SCANCODE_MUTE: break;
			case SDL_SCANCODE_VOLUMEUP: break;
			case SDL_SCANCODE_VOLUMEDOWN: break;
			case SDL_SCANCODE_KP_COMMA: break;
			case SDL_SCANCODE_KP_EQUALSAS400: break;
			case SDL_SCANCODE_INTERNATIONAL1: break;
			case SDL_SCANCODE_INTERNATIONAL2: break;
			case SDL_SCANCODE_INTERNATIONAL3: break;
			case SDL_SCANCODE_INTERNATIONAL4: break;
			case SDL_SCANCODE_INTERNATIONAL5: break;
			case SDL_SCANCODE_INTERNATIONAL6: break;
			case SDL_SCANCODE_INTERNATIONAL7: break;
			case SDL_SCANCODE_INTERNATIONAL8: break;
			case SDL_SCANCODE_INTERNATIONAL9: break;
			case SDL_SCANCODE_LANG1: break;
			case SDL_SCANCODE_LANG2: break;
			case SDL_SCANCODE_LANG3: break;
			case SDL_SCANCODE_LANG4: break;
			case SDL_SCANCODE_LANG5: break;
			case SDL_SCANCODE_LANG6: break;
			case SDL_SCANCODE_LANG7: break;
			case SDL_SCANCODE_LANG8: break;
			case SDL_SCANCODE_LANG9: break;
			case SDL_SCANCODE_ALTERASE: break;
			case SDL_SCANCODE_SYSREQ: break;
			case SDL_SCANCODE_CANCEL: break;
			case SDL_SCANCODE_CLEAR: break;
			case SDL_SCANCODE_PRIOR: break;
			case SDL_SCANCODE_RETURN2: break;
			case SDL_SCANCODE_SEPARATOR: break;
			case SDL_SCANCODE_OUT: break;
			case SDL_SCANCODE_OPER: break;
			case SDL_SCANCODE_CLEARAGAIN: break;
			case SDL_SCANCODE_CRSEL: break;
			case SDL_SCANCODE_EXSEL: break;
			case SDL_SCANCODE_KP_00: break;
			case SDL_SCANCODE_KP_000: break;
			case SDL_SCANCODE_THOUSANDSSEPARATOR: break;
			case SDL_SCANCODE_DECIMALSEPARATOR: break;
			case SDL_SCANCODE_CURRENCYUNIT: break;
			case SDL_SCANCODE_CURRENCYSUBUNIT: break;
			case SDL_SCANCODE_KP_LEFTPAREN: break;
			case SDL_SCANCODE_KP_RIGHTPAREN: break;
			case SDL_SCANCODE_KP_LEFTBRACE: break;
			case SDL_SCANCODE_KP_RIGHTBRACE: break;
			case SDL_SCANCODE_KP_TAB: break;
			case SDL_SCANCODE_KP_BACKSPACE: break;
			case SDL_SCANCODE_KP_A: break;
			case SDL_SCANCODE_KP_B: break;
			case SDL_SCANCODE_KP_C: break;
			case SDL_SCANCODE_KP_D: break;
			case SDL_SCANCODE_KP_E: break;
			case SDL_SCANCODE_KP_F: break;
			case SDL_SCANCODE_KP_XOR: break;
			case SDL_SCANCODE_KP_POWER: break;
			case SDL_SCANCODE_KP_PERCENT: break;
			case SDL_SCANCODE_KP_LESS: break;
			case SDL_SCANCODE_KP_GREATER: break;
			case SDL_SCANCODE_KP_AMPERSAND: break;
			case SDL_SCANCODE_KP_DBLAMPERSAND: break;
			case SDL_SCANCODE_KP_VERTICALBAR: break;
			case SDL_SCANCODE_KP_DBLVERTICALBAR: break;
			case SDL_SCANCODE_KP_COLON: break;
			case SDL_SCANCODE_KP_HASH: break;
			case SDL_SCANCODE_KP_SPACE: break;
			case SDL_SCANCODE_KP_AT: break;
			case SDL_SCANCODE_KP_EXCLAM: break;
			case SDL_SCANCODE_KP_MEMSTORE: break;
			case SDL_SCANCODE_KP_MEMRECALL: break;
			case SDL_SCANCODE_KP_MEMCLEAR: break;
			case SDL_SCANCODE_KP_MEMADD: break;
			case SDL_SCANCODE_KP_MEMSUBTRACT: break;
			case SDL_SCANCODE_KP_MEMMULTIPLY: break;
			case SDL_SCANCODE_KP_MEMDIVIDE: break;
			case SDL_SCANCODE_KP_PLUSMINUS: break;
			case SDL_SCANCODE_KP_CLEAR: break;
			case SDL_SCANCODE_KP_CLEARENTRY: break;
			case SDL_SCANCODE_KP_BINARY: break;
			case SDL_SCANCODE_KP_OCTAL: break;
			case SDL_SCANCODE_KP_DECIMAL: break;
			case SDL_SCANCODE_KP_HEXADECIMAL: break;
			case SDL_SCANCODE_LCTRL: break;
			case SDL_SCANCODE_LSHIFT: break;
			case SDL_SCANCODE_LALT: break;
			case SDL_SCANCODE_LGUI: break;
			case SDL_SCANCODE_RCTRL: break;
			case SDL_SCANCODE_RSHIFT: break;
			case SDL_SCANCODE_RALT: break;
			case SDL_SCANCODE_RGUI: break;
			case SDL_SCANCODE_MODE: break;

			case SDL_SCANCODE_AUDIONEXT: break;
			case SDL_SCANCODE_AUDIOPREV: break;
			case SDL_SCANCODE_AUDIOSTOP: break;
			case SDL_SCANCODE_AUDIOPLAY: break;
			case SDL_SCANCODE_AUDIOMUTE: break;
			case SDL_SCANCODE_MEDIASELECT: break;
			case SDL_SCANCODE_WWW: break;
			case SDL_SCANCODE_MAIL: break;
			case SDL_SCANCODE_CALCULATOR: break;
			case SDL_SCANCODE_COMPUTER: break;
			case SDL_SCANCODE_AC_SEARCH: break;
			case SDL_SCANCODE_AC_HOME: break;
			case SDL_SCANCODE_AC_BACK: break;
			case SDL_SCANCODE_AC_FORWARD: break;
			case SDL_SCANCODE_AC_STOP: break;
			case SDL_SCANCODE_AC_REFRESH: break;
			case SDL_SCANCODE_AC_BOOKMARKS: break;

			case SDL_SCANCODE_BRIGHTNESSDOWN: break;
			case SDL_SCANCODE_BRIGHTNESSUP: break;
			case SDL_SCANCODE_DISPLAYSWITCH: break;

			case SDL_SCANCODE_KBDILLUMTOGGLE: break;
			case SDL_SCANCODE_KBDILLUMDOWN: break;
			case SDL_SCANCODE_KBDILLUMUP: break;
			case SDL_SCANCODE_EJECT: break;
			case SDL_SCANCODE_SLEEP: break;
			case SDL_SCANCODE_APP1: break;
			case SDL_SCANCODE_APP2: break;

			case SDL_SCANCODE_AUDIOREWIND: break;
			case SDL_SCANCODE_AUDIOFASTFORWARD: break;

			case SDL_SCANCODE_SOFTLEFT: break;

			case SDL_SCANCODE_SOFTRIGHT: break;

			case SDL_SCANCODE_CALL: break;
			case SDL_SCANCODE_ENDCALL: break;
			default: throw std::runtime_error("Failed to find a valid scancode");
		}
	}

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

	std::vector<Monitor> Application::GetMonitors()
	{
		std::vector<Monitor> monitors;

		int					 displayCount;
		const SDL_DisplayID *displays = SDL_GetDisplays(&displayCount);

		for (int i = 0; i < displayCount; i++)
		{
			SDL_DisplayID id = displays[i];
			SDL_Rect	  bounds;

			Monitor monitor;
			monitor.DPI	 = SDL_GetDisplayContentScale(id);
			monitor.Name = SDL_GetDisplayName(id);

			SDL_GetDisplayBounds(id, &bounds);
			monitor.Position = {bounds.x, bounds.y};
			monitor.Size	 = {bounds.w, bounds.h};

			SDL_GetDisplayUsableBounds(id, &bounds);
			monitor.WorkPosition = {bounds.x, bounds.y};
			monitor.WorkSize	 = {bounds.w, bounds.h};

			monitors.push_back(monitor);
		}

		return monitors;
	}

	const Keyboard &Application::GetGlobalKeyboardState() const
	{
		return m_GlobalKeyboardState;
	}

	std::vector<InputNew::Keyboard> Application::GetKeyboards()
	{
		std::vector<InputNew::Keyboard> keyboards;

		int				count;
		SDL_KeyboardID *sdlKeyboards = SDL_GetKeyboards(&count);

		for (int i = 0; i < count; i++)
		{
			uint32_t	id	 = sdlKeyboards[i];
			std::string name = SDL_GetKeyboardInstanceName(id);
			keyboards.emplace_back(sdlKeyboards[i], name);
		}

		return keyboards;
	}

	std::vector<InputNew::Mouse> Application::GetMice()
	{
		std::vector<InputNew::Mouse> mice;

		int			 count;
		SDL_MouseID *sdlMice = SDL_GetMice(&count);

		for (int i = 0; i < count; i++)
		{
			uint32_t	id	 = sdlMice[i];
			std::string name = SDL_GetKeyboardInstanceName(id);
			mice.emplace_back(sdlMice[i], name);
		}

		return mice;
	}

	std::vector<InputNew::Gamepad> Application::GetGamepads()
	{
		std::vector<InputNew::Gamepad> gamepads;

		int				count;
		SDL_JoystickID *sdlGamepads = SDL_GetGamepads(&count);

		for (int i = 0; i < count; i++)
		{
			if (SDL_IsGamepad(sdlGamepads[i]))
			{
				uint32_t	id	 = sdlGamepads[i];
				std::string name = SDL_GetKeyboardInstanceName(id);
				gamepads.emplace_back(sdlGamepads[i], name);
			}
		}

		return gamepads;
	}

	void Application::DispatchEvent(const InputEvent &event, Window *window)
	{
		if (!OnEvent(event, window))
		{
			window->OnEvent(event);
		}
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
					auto nexusKeyCode									   = SDLToNexusKeycode(event.key.keysym.sym);
					auto nexusScanCode									   = SDLToNexusScancode(event.key.keysym.scancode);

					if (window->m_Input.m_Keyboard.m_CurrentKeys[nexusKeyCode])
					{
						window->OnKeyHeld.Invoke(nexusKeyCode);
					}
					else
					{
						window->OnKeyPressed.Invoke(nexusKeyCode);
					}

					window->m_Input.m_Keyboard.m_CurrentKeys[nexusKeyCode] = true;
					m_GlobalKeyboardState.m_CurrentKeys[nexusKeyCode]	   = true;

					KeyPressedEvent keyPressedEvent {.KeyCode	 = nexusKeyCode,
													 .ScanCode	 = nexusScanCode,
													 .Repeat	 = event.key.repeat,
													 .Unicode	 = event.key.keysym.sym,
													 .KeyboardID = event.kdevice.which};

					DispatchEvent(keyPressedEvent, window);
					break;
				}
				case SDL_EVENT_KEY_UP:
				{
					auto nexusKeyCode									   = SDLToNexusKeycode(event.key.keysym.sym);
					window->m_Input.m_Keyboard.m_CurrentKeys[nexusKeyCode] = false;
					m_GlobalKeyboardState.m_CurrentKeys[nexusKeyCode]	   = false;
					DispatchEvent(KeyReleasedEvent {.Key = nexusKeyCode}, window);
					window->OnKeyReleased.Invoke(nexusKeyCode);
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				{
					DispatchEvent({MouseButtonPressedEvent {.MouseButton = event.button.button}}, window);

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

					DispatchEvent(MouseButtonReleasedEvent {.MouseButton = event.button.button}, window);

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

					MouseMovedEvent e {.Position = {xPos, yPos}, .Movement = {movementX, movementY}};
					DispatchEvent(e, window);

					window->OnMouseMoved.Invoke(Nexus::Point2D<float>(event.motion.xrel, event.motion.yrel));

					break;
				}
				case SDL_EVENT_MOUSE_WHEEL:
				{
					auto &scroll = window->m_Input.m_Mouse.m_CurrentState.MouseWheel;
					scroll.X += event.wheel.x;
					scroll.Y += event.wheel.y;

					DispatchEvent(MouseScrolledEvent {.ScrollX = event.wheel.x, .ScrollY = event.wheel.y}, window);
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
					std::string file = event.drop.data;
					window->OnFileDrop.Invoke(file);
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