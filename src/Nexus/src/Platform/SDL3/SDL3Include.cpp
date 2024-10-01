#include "SDL3Include.hpp"

namespace Nexus::SDL3
{
	KeyCode GetNexusKeyCodeFromSDLKeyCode(SDL_Keycode keycode)
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

	ScanCode GetNexusScanCodeFromSDLScanCode(SDL_Scancode scancode)
	{
		switch (scancode)
		{
			case SDL_SCANCODE_A: return ScanCode::A;
			case SDL_SCANCODE_B: return ScanCode::B;
			case SDL_SCANCODE_C: return ScanCode::C;
			case SDL_SCANCODE_D: return ScanCode::D;
			case SDL_SCANCODE_E: return ScanCode::E;
			case SDL_SCANCODE_F: return ScanCode::F;
			case SDL_SCANCODE_G: return ScanCode::G;
			case SDL_SCANCODE_H: return ScanCode::H;
			case SDL_SCANCODE_I: return ScanCode::I;
			case SDL_SCANCODE_J: return ScanCode::J;
			case SDL_SCANCODE_K: return ScanCode::K;
			case SDL_SCANCODE_L: return ScanCode::L;
			case SDL_SCANCODE_M: return ScanCode::M;
			case SDL_SCANCODE_N: return ScanCode::N;
			case SDL_SCANCODE_O: return ScanCode::O;
			case SDL_SCANCODE_P: return ScanCode::P;
			case SDL_SCANCODE_Q: return ScanCode::Q;
			case SDL_SCANCODE_R: return ScanCode::R;
			case SDL_SCANCODE_S: return ScanCode::S;
			case SDL_SCANCODE_T: return ScanCode::T;
			case SDL_SCANCODE_U: return ScanCode::U;
			case SDL_SCANCODE_V: return ScanCode::V;
			case SDL_SCANCODE_W: return ScanCode::W;
			case SDL_SCANCODE_X: return ScanCode::X;
			case SDL_SCANCODE_Y: return ScanCode::Y;
			case SDL_SCANCODE_Z: return ScanCode::Z;
			case SDL_SCANCODE_1: return ScanCode::One;
			case SDL_SCANCODE_2: return ScanCode::Two;
			case SDL_SCANCODE_3: return ScanCode::Three;
			case SDL_SCANCODE_4: return ScanCode::Four;
			case SDL_SCANCODE_5: return ScanCode::Five;
			case SDL_SCANCODE_6: return ScanCode::Six;
			case SDL_SCANCODE_7: return ScanCode::Seven;
			case SDL_SCANCODE_8: return ScanCode::Eight;
			case SDL_SCANCODE_9: return ScanCode::Nine;
			case SDL_SCANCODE_0: return ScanCode::Zero;
			case SDL_SCANCODE_RETURN: return ScanCode::Return;
			case SDL_SCANCODE_ESCAPE: return ScanCode::Escape;
			case SDL_SCANCODE_BACKSPACE: return ScanCode::Backspace;
			case SDL_SCANCODE_TAB: return ScanCode::Tab;
			case SDL_SCANCODE_SPACE: return ScanCode::Space;
			case SDL_SCANCODE_MINUS: return ScanCode::Minus;
			case SDL_SCANCODE_EQUALS: return ScanCode::Equals;
			case SDL_SCANCODE_LEFTBRACKET: return ScanCode::LeftBracket;
			case SDL_SCANCODE_RIGHTBRACKET: return ScanCode::RightBracket;
			case SDL_SCANCODE_BACKSLASH: return ScanCode::Backslash;
			case SDL_SCANCODE_NONUSHASH: return ScanCode::NonusHash;

			case SDL_SCANCODE_SEMICOLON: return ScanCode::Semicolon;
			case SDL_SCANCODE_APOSTROPHE: return ScanCode::Apostrophe;
			case SDL_SCANCODE_GRAVE: return ScanCode::Grave;
			case SDL_SCANCODE_COMMA: return ScanCode::Comma;
			case SDL_SCANCODE_PERIOD: return ScanCode::Period;
			case SDL_SCANCODE_SLASH: return ScanCode::Slash;
			case SDL_SCANCODE_CAPSLOCK: return ScanCode::CapsLock;
			case SDL_SCANCODE_F1: return ScanCode::F1;
			case SDL_SCANCODE_F2: return ScanCode::F2;
			case SDL_SCANCODE_F3: return ScanCode::F3;
			case SDL_SCANCODE_F4: return ScanCode::F4;
			case SDL_SCANCODE_F5: return ScanCode::F5;
			case SDL_SCANCODE_F6: return ScanCode::F6;
			case SDL_SCANCODE_F7: return ScanCode::F7;
			case SDL_SCANCODE_F8: return ScanCode::F8;
			case SDL_SCANCODE_F9: return ScanCode::F9;
			case SDL_SCANCODE_F10: return ScanCode::F10;
			case SDL_SCANCODE_F11: return ScanCode::F11;
			case SDL_SCANCODE_F12: return ScanCode::F12;
			case SDL_SCANCODE_PRINTSCREEN: return ScanCode::PrintScreen;
			case SDL_SCANCODE_SCROLLLOCK: return ScanCode::ScrollLock;
			case SDL_SCANCODE_PAUSE: return ScanCode::Pause;
			case SDL_SCANCODE_INSERT: return ScanCode::Insert;
			case SDL_SCANCODE_HOME: return ScanCode::Home;
			case SDL_SCANCODE_PAGEUP: return ScanCode::PageUp;
			case SDL_SCANCODE_DELETE: return ScanCode::Delete;
			case SDL_SCANCODE_END: return ScanCode::End;
			case SDL_SCANCODE_PAGEDOWN: return ScanCode::PageDown;
			case SDL_SCANCODE_RIGHT: return ScanCode::Right;
			case SDL_SCANCODE_LEFT: return ScanCode::Left;
			case SDL_SCANCODE_DOWN: return ScanCode::Down;
			case SDL_SCANCODE_UP: return ScanCode::Up;
			case SDL_SCANCODE_NUMLOCKCLEAR: return ScanCode::NumLockClear;

			case SDL_SCANCODE_KP_DIVIDE: return ScanCode::KeyPad_Divide;
			case SDL_SCANCODE_KP_MULTIPLY: return ScanCode::KeyPad_Multiply;
			case SDL_SCANCODE_KP_MINUS: return ScanCode::Minus;
			case SDL_SCANCODE_KP_PLUS: return ScanCode::KeyPad_Plus;
			case SDL_SCANCODE_KP_ENTER: return ScanCode::KeyPad_Enter;
			case SDL_SCANCODE_KP_1: return ScanCode::KeyPad_One;
			case SDL_SCANCODE_KP_2: return ScanCode::KeyPad_Two;
			case SDL_SCANCODE_KP_3: return ScanCode::KeyPad_Three;
			case SDL_SCANCODE_KP_4: return ScanCode::KeyPad_Four;
			case SDL_SCANCODE_KP_5: return ScanCode::KeyPad_Five;
			case SDL_SCANCODE_KP_6: return ScanCode::KeyPad_Six;
			case SDL_SCANCODE_KP_7: return ScanCode::KeyPad_Seven;
			case SDL_SCANCODE_KP_8: return ScanCode::KeyPad_Eight;
			case SDL_SCANCODE_KP_9: return ScanCode::KeyPad_Nine;
			case SDL_SCANCODE_KP_0: return ScanCode::KeyPad_Zero;
			case SDL_SCANCODE_KP_PERIOD: return ScanCode::KeyPad_Period;
			case SDL_SCANCODE_NONUSBACKSLASH: return ScanCode::NonusBackslash;
			case SDL_SCANCODE_APPLICATION: return ScanCode::Application;
			case SDL_SCANCODE_KP_EQUALS: return ScanCode::KeyPad_Equals;
			case SDL_SCANCODE_F13: return ScanCode::F13;
			case SDL_SCANCODE_F14: return ScanCode::F14;
			case SDL_SCANCODE_F15: return ScanCode::F15;
			case SDL_SCANCODE_F16: return ScanCode::F16;
			case SDL_SCANCODE_F17: return ScanCode::F17;
			case SDL_SCANCODE_F18: return ScanCode::F18;
			case SDL_SCANCODE_F19: return ScanCode::F19;
			case SDL_SCANCODE_F20: return ScanCode::F20;
			case SDL_SCANCODE_F21: return ScanCode::F21;
			case SDL_SCANCODE_F22: return ScanCode::F22;
			case SDL_SCANCODE_F23: return ScanCode::F23;
			case SDL_SCANCODE_F24: return ScanCode::F24;
			case SDL_SCANCODE_EXECUTE: return ScanCode::Execute;
			case SDL_SCANCODE_HELP: return ScanCode::Help;
			case SDL_SCANCODE_MENU: return ScanCode::Menu;
			case SDL_SCANCODE_SELECT: return ScanCode::Select;
			case SDL_SCANCODE_STOP: return ScanCode::Stop;
			case SDL_SCANCODE_AGAIN: return ScanCode::Again;
			case SDL_SCANCODE_UNDO: return ScanCode::Undo;
			case SDL_SCANCODE_CUT: return ScanCode::Cut;
			case SDL_SCANCODE_COPY: return ScanCode::Copy;
			case SDL_SCANCODE_PASTE: return ScanCode::Paste;
			case SDL_SCANCODE_FIND: return ScanCode::Find;
			case SDL_SCANCODE_MUTE: return ScanCode::Mute;
			case SDL_SCANCODE_VOLUMEUP: return ScanCode::VolumeUp;
			case SDL_SCANCODE_VOLUMEDOWN: return ScanCode::VolumeDown;
			case SDL_SCANCODE_KP_COMMA: return ScanCode::KeyPad_Comma;
			case SDL_SCANCODE_KP_EQUALSAS400: return ScanCode::KeyPad_EqualsAS400;
			case SDL_SCANCODE_INTERNATIONAL1: return ScanCode::International1;
			case SDL_SCANCODE_INTERNATIONAL2: return ScanCode::International2;
			case SDL_SCANCODE_INTERNATIONAL3: return ScanCode::International3;
			case SDL_SCANCODE_INTERNATIONAL4: return ScanCode::International4;
			case SDL_SCANCODE_INTERNATIONAL5: return ScanCode::International5;
			case SDL_SCANCODE_INTERNATIONAL6: return ScanCode::International6;
			case SDL_SCANCODE_INTERNATIONAL7: return ScanCode::International7;
			case SDL_SCANCODE_INTERNATIONAL8: return ScanCode::International8;
			case SDL_SCANCODE_INTERNATIONAL9: return ScanCode::International9;
			case SDL_SCANCODE_LANG1: return ScanCode::Lang1;
			case SDL_SCANCODE_LANG2: return ScanCode::Lang2;
			case SDL_SCANCODE_LANG3: return ScanCode::Lang3;
			case SDL_SCANCODE_LANG4: return ScanCode::Lang4;
			case SDL_SCANCODE_LANG5: return ScanCode::Lang5;
			case SDL_SCANCODE_LANG6: return ScanCode::Lang6;
			case SDL_SCANCODE_LANG7: return ScanCode::Lang7;
			case SDL_SCANCODE_LANG8: return ScanCode::Lang8;
			case SDL_SCANCODE_LANG9: return ScanCode::Lang9;
			case SDL_SCANCODE_ALTERASE: return ScanCode::AltErase;
			case SDL_SCANCODE_SYSREQ: return ScanCode::SysReq;
			case SDL_SCANCODE_CANCEL: return ScanCode::Cancel;
			case SDL_SCANCODE_CLEAR: return ScanCode::Clear;
			case SDL_SCANCODE_PRIOR: return ScanCode::Prior;
			case SDL_SCANCODE_RETURN2: return ScanCode::Return2;
			case SDL_SCANCODE_SEPARATOR: return ScanCode::Separator;
			case SDL_SCANCODE_OUT: return ScanCode::Out;
			case SDL_SCANCODE_OPER: return ScanCode::Oper;
			case SDL_SCANCODE_CLEARAGAIN: return ScanCode::ClearAgain;
			case SDL_SCANCODE_CRSEL: return ScanCode::Crsel;
			case SDL_SCANCODE_EXSEL: return ScanCode::Exsel;
			case SDL_SCANCODE_KP_00: return ScanCode::KeyPad_00;
			case SDL_SCANCODE_KP_000: return ScanCode::KeyPad_000;
			case SDL_SCANCODE_THOUSANDSSEPARATOR: return ScanCode::ThousandsSeparator;
			case SDL_SCANCODE_DECIMALSEPARATOR: return ScanCode::DecimalSeparator;
			case SDL_SCANCODE_CURRENCYUNIT: return ScanCode::CurrencyUnit;
			case SDL_SCANCODE_CURRENCYSUBUNIT: return ScanCode::CurrencySubUnit;
			case SDL_SCANCODE_KP_LEFTPAREN: return ScanCode::KeyPad_LeftParenthesis;
			case SDL_SCANCODE_KP_RIGHTPAREN: return ScanCode::KeyPad_RightParenthesis;
			case SDL_SCANCODE_KP_LEFTBRACE: return ScanCode::LeftBracket;
			case SDL_SCANCODE_KP_RIGHTBRACE: return ScanCode::RightBracket;
			case SDL_SCANCODE_KP_TAB: return ScanCode::Tab;
			case SDL_SCANCODE_KP_BACKSPACE: return ScanCode::Backspace;
			case SDL_SCANCODE_KP_A: return ScanCode::KeyPad_A;
			case SDL_SCANCODE_KP_B: return ScanCode::KeyPad_B;
			case SDL_SCANCODE_KP_C: return ScanCode::KeyPad_C;
			case SDL_SCANCODE_KP_D: return ScanCode::KeyPad_D;
			case SDL_SCANCODE_KP_E: return ScanCode::KeyPad_E;
			case SDL_SCANCODE_KP_F: return ScanCode::KeyPad_F;
			case SDL_SCANCODE_KP_XOR: return ScanCode::KeyPad_XOR;
			case SDL_SCANCODE_KP_POWER: return ScanCode::KeyPad_Power;
			case SDL_SCANCODE_KP_PERCENT: return ScanCode::KeyPad_Percent;
			case SDL_SCANCODE_KP_LESS: return ScanCode::KeyPad_Less;
			case SDL_SCANCODE_KP_GREATER: return ScanCode::KeyPad_Greater;
			case SDL_SCANCODE_KP_AMPERSAND: return ScanCode::KeyPad_Ampersand;
			case SDL_SCANCODE_KP_DBLAMPERSAND: return ScanCode::KeyPad_DBLAmpersand;
			case SDL_SCANCODE_KP_VERTICALBAR: return ScanCode::KeyPad_VerticalBar;
			case SDL_SCANCODE_KP_DBLVERTICALBAR: return ScanCode::KeyPad_DBLVerticalBar;
			case SDL_SCANCODE_KP_COLON: return ScanCode::KeyPad_Colon;
			case SDL_SCANCODE_KP_HASH: return ScanCode::KeyPad_Hash;
			case SDL_SCANCODE_KP_SPACE: return ScanCode::Space;
			case SDL_SCANCODE_KP_AT: return ScanCode::KeyPad_At;
			case SDL_SCANCODE_KP_EXCLAM: return ScanCode::KeyPad_Exclam;
			case SDL_SCANCODE_KP_MEMSTORE: return ScanCode::KeyPad_MemStore;
			case SDL_SCANCODE_KP_MEMRECALL: return ScanCode::KeyPad_MemRecall;
			case SDL_SCANCODE_KP_MEMCLEAR: return ScanCode::KeyPad_MemClear;
			case SDL_SCANCODE_KP_MEMADD: return ScanCode::KeyPad_MemAdd;
			case SDL_SCANCODE_KP_MEMSUBTRACT: return ScanCode::KeyPad_MemSubtract;
			case SDL_SCANCODE_KP_MEMMULTIPLY: return ScanCode::KeyPad_MemMultiply;
			case SDL_SCANCODE_KP_MEMDIVIDE: return ScanCode::KeyPad_MemDivide;
			case SDL_SCANCODE_KP_PLUSMINUS: return ScanCode::KeyPad_PlusMinus;
			case SDL_SCANCODE_KP_CLEAR: return ScanCode::KeyPad_Clear;
			case SDL_SCANCODE_KP_CLEARENTRY: return ScanCode::KeyPad_ClearEntry;
			case SDL_SCANCODE_KP_BINARY: return ScanCode::KeyPad_Binary;
			case SDL_SCANCODE_KP_OCTAL: return ScanCode::KeyPad_Octal;
			case SDL_SCANCODE_KP_DECIMAL: return ScanCode::KeyPad_Decimal;
			case SDL_SCANCODE_KP_HEXADECIMAL: return ScanCode::KeyPad_Hexadecimal;
			case SDL_SCANCODE_LCTRL: return ScanCode::LeftControl;
			case SDL_SCANCODE_LSHIFT: return ScanCode::LeftShift;
			case SDL_SCANCODE_LALT: return ScanCode::LeftAlt;
			case SDL_SCANCODE_LGUI: return ScanCode::LeftGUI;
			case SDL_SCANCODE_RCTRL: return ScanCode::RightControl;
			case SDL_SCANCODE_RSHIFT: return ScanCode::RightShift;
			case SDL_SCANCODE_RALT: return ScanCode::RightAlt;
			case SDL_SCANCODE_RGUI: return ScanCode::RightGUI;
			case SDL_SCANCODE_MODE: return ScanCode::Mode;

			case SDL_SCANCODE_AUDIONEXT: return ScanCode::AudioNext;
			case SDL_SCANCODE_AUDIOPREV: return ScanCode::AudioPrevious;
			case SDL_SCANCODE_AUDIOSTOP: return ScanCode::AudioStop;
			case SDL_SCANCODE_AUDIOPLAY: return ScanCode::AudioPlay;
			case SDL_SCANCODE_AUDIOMUTE: return ScanCode::AudioMute;
			case SDL_SCANCODE_MEDIASELECT: return ScanCode::MediaSelect;
			case SDL_SCANCODE_WWW: return ScanCode::WWW;
			case SDL_SCANCODE_MAIL: return ScanCode::Mail;
			case SDL_SCANCODE_CALCULATOR: return ScanCode::Calculator;
			case SDL_SCANCODE_COMPUTER: return ScanCode::Computer;
			case SDL_SCANCODE_AC_SEARCH: return ScanCode::AC_Search;
			case SDL_SCANCODE_AC_HOME: return ScanCode::AC_Home;
			case SDL_SCANCODE_AC_BACK: return ScanCode::AC_Back;
			case SDL_SCANCODE_AC_FORWARD: return ScanCode::AC_Forward;
			case SDL_SCANCODE_AC_STOP: return ScanCode::AC_Stop;
			case SDL_SCANCODE_AC_REFRESH: return ScanCode::AC_Refresh;
			case SDL_SCANCODE_AC_BOOKMARKS: return ScanCode::AC_Bookmarks;

			case SDL_SCANCODE_BRIGHTNESSDOWN: return ScanCode::BrightnessDown;
			case SDL_SCANCODE_BRIGHTNESSUP: return ScanCode::BrightnessUp;
			case SDL_SCANCODE_DISPLAYSWITCH: return ScanCode::DisplaySwitch;

			case SDL_SCANCODE_KBDILLUMTOGGLE: return ScanCode::DillumToggle;
			case SDL_SCANCODE_KBDILLUMDOWN: return ScanCode::DillumDown;
			case SDL_SCANCODE_KBDILLUMUP: return ScanCode::DillumUp;
			case SDL_SCANCODE_EJECT: return ScanCode::Eject;
			case SDL_SCANCODE_SLEEP: return ScanCode::Sleep;
			case SDL_SCANCODE_APP1: return ScanCode::App1;
			case SDL_SCANCODE_APP2: return ScanCode::App2;

			case SDL_SCANCODE_AUDIOREWIND: return ScanCode::AudioRewind;
			case SDL_SCANCODE_AUDIOFASTFORWARD: return ScanCode::AudioFastForward;

			case SDL_SCANCODE_SOFTLEFT: return ScanCode::SoftLeft;
			case SDL_SCANCODE_SOFTRIGHT: return ScanCode::SoftRight;

			case SDL_SCANCODE_CALL: return ScanCode::Call;
			case SDL_SCANCODE_ENDCALL: return ScanCode::EndCall;

			default: throw std::runtime_error("Failed to find a valid scancode");
		}
	}

	Keyboard::Modifiers GetNexusModifiersFromSDLModifiers(Uint16 modifiers)
	{
		Keyboard::Modifiers mods = 0;

		if (modifiers & SDL_KMOD_LSHIFT)
		{
			mods |= Keyboard::Modifier::LeftShift;
		}

		if (modifiers & SDL_KMOD_RSHIFT)
		{
			mods |= Keyboard::Modifier::RightShift;
		}

		if (modifiers & SDL_KMOD_LCTRL)
		{
			mods |= Keyboard::Modifier::LeftControl;
		}

		if (modifiers & SDL_KMOD_RCTRL)
		{
			mods |= Keyboard::Modifier::RightControl;
		}

		// new
		if (modifiers & SDL_KMOD_LALT)
		{
			mods |= Keyboard::Modifier::LeftAlt;
		}

		if (modifiers & SDL_KMOD_RALT)
		{
			mods |= Keyboard::Modifier::RightAlt;
		}

		if (modifiers & SDL_KMOD_LGUI)
		{
			mods |= Keyboard::Modifier::LeftGUI;
		}

		if (modifiers & SDL_KMOD_RGUI)
		{
			mods |= Keyboard::Modifier::RightGUI;
		}

		if (modifiers & SDL_KMOD_NUM)
		{
			mods |= Keyboard::Modifier::Num;
		}

		if (modifiers & SDL_KMOD_CAPS)
		{
			mods |= Keyboard::Modifier::Caps;
		}

		if (modifiers & SDL_KMOD_MODE)
		{
			mods |= Keyboard::Modifier::Mode;
		}

		if (modifiers & SDL_KMOD_SCROLL)
		{
			mods |= Keyboard::Modifier::Scroll;
		}

		if (modifiers & Keyboard::Modifier::LeftControl || modifiers & Keyboard::Modifier::RightControl)
		{
			mods |= Keyboard::Modifier::Control;
		}

		if (modifiers & Keyboard::Modifier::LeftShift || modifiers & Keyboard::Modifier::RightShift)
		{
			mods |= Keyboard::Modifier::Shift;
		}

		if (modifiers & Keyboard::Modifier::LeftAlt || modifiers & Keyboard::Modifier::RightAlt)
		{
			mods |= Keyboard::Modifier::Alt;
		}

		if (modifiers & Keyboard::Modifier::LeftGUI || modifiers & Keyboard::Modifier::RightGUI)
		{
			mods |= Keyboard::Modifier::GUI;
		}

		return mods;
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

		return gamepads;
	}

	std::vector<Nexus::Monitor> GetMonitors()
	{
		std::vector<Monitor> monitors;

		int					 displayCount;
		const SDL_DisplayID *displays = SDL_GetDisplays(&displayCount);

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

		return monitors;
	}

	FileDropType GetFileDropType(SDL_EventType type)
	{
		switch (type)
		{
			case SDL_EVENT_DROP_BEGIN: return FileDropType::Begin;
			case SDL_EVENT_DROP_COMPLETE: return FileDropType::Complete;
			case SDL_EVENT_DROP_FILE: return FileDropType::File;
			case SDL_EVENT_DROP_TEXT: return FileDropType::Text;
			case SDL_EVENT_DROP_POSITION: return FileDropType::Position;

			default: throw std::runtime_error("Failed to find valid event type");
		}
	}

	std::tuple<MouseType, uint32_t> GetMouseInfo(SDL_MouseID mouseId)
	{
		uint32_t  id		= 0;
		MouseType mouseType = MouseType::Mouse;

		if (mouseId == SDL_TOUCH_MOUSEID)
		{
			mouseType = MouseType::Touch;
		}
		else
		{
			id = mouseId;
		}

		return {mouseType, id};
	}

	ScrollDirection GetScrollDirection(SDL_MouseWheelDirection scrollDirection)
	{
		ScrollDirection direction = ScrollDirection::Normal;

		if (scrollDirection == SDL_MOUSEWHEEL_FLIPPED)
		{
			direction = ScrollDirection::Flipped;
		}

		return direction;
	}
}	 // namespace Nexus::SDL3