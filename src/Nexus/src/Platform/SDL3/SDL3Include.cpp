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

			case SDLK_GRAVE: return KeyCode::BackQuote;
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
			case SDLK_Q: return KeyCode::Q;
			case SDLK_W: return KeyCode::W;
			case SDLK_E: return KeyCode::E;
			case SDLK_R: return KeyCode::R;
			case SDLK_T: return KeyCode::T;
			case SDLK_Y: return KeyCode::Y;
			case SDLK_U: return KeyCode::U;
			case SDLK_I: return KeyCode::I;
			case SDLK_O: return KeyCode::O;
			case SDLK_P: return KeyCode::P;
			case SDLK_LEFTBRACKET: return KeyCode::LeftBracket;
			case SDLK_RIGHTBRACKET: return KeyCode::RightBracket;
			case SDLK_RETURN: return KeyCode::Return;

			case SDLK_CAPSLOCK: return KeyCode::CapsLock;
			case SDLK_A: return KeyCode::A;
			case SDLK_S: return KeyCode::S;
			case SDLK_D: return KeyCode::D;
			case SDLK_F: return KeyCode::F;
			case SDLK_G: return KeyCode::G;
			case SDLK_H: return KeyCode::H;
			case SDLK_J: return KeyCode::J;
			case SDLK_K: return KeyCode::K;
			case SDLK_L: return KeyCode::L;
			case SDLK_SEMICOLON: return KeyCode::Semicolon;
			case SDLK_APOSTROPHE: return KeyCode::Quote;
			case SDLK_HASH: return KeyCode::Hash;

			case SDLK_LSHIFT: return KeyCode::LeftShift;
			case SDLK_BACKSLASH: return KeyCode::BackSlash;
			case SDLK_Z: return KeyCode::Z;
			case SDLK_X: return KeyCode::X;
			case SDLK_C: return KeyCode::C;
			case SDLK_V: return KeyCode::V;
			case SDLK_B: return KeyCode::B;
			case SDLK_N: return KeyCode::N;
			case SDLK_M: return KeyCode::M;
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

			case SDL_SCANCODE_KP_DIVIDE: return ScanCode::KeyPadDivide;
			case SDL_SCANCODE_KP_MULTIPLY: return ScanCode::KeyPadMultiply;
			case SDL_SCANCODE_KP_MINUS: return ScanCode::KeyPadMinus;
			case SDL_SCANCODE_KP_PLUS: return ScanCode::KeyPadPlus;
			case SDL_SCANCODE_KP_ENTER: return ScanCode::KeyPadEnter;
			case SDL_SCANCODE_KP_1: return ScanCode::KeyPadOne;
			case SDL_SCANCODE_KP_2: return ScanCode::KeyPadTwo;
			case SDL_SCANCODE_KP_3: return ScanCode::KeyPadThree;
			case SDL_SCANCODE_KP_4: return ScanCode::KeyPadFour;
			case SDL_SCANCODE_KP_5: return ScanCode::KeyPadFive;
			case SDL_SCANCODE_KP_6: return ScanCode::KeyPadSix;
			case SDL_SCANCODE_KP_7: return ScanCode::KeyPadSeven;
			case SDL_SCANCODE_KP_8: return ScanCode::KeyPadEight;
			case SDL_SCANCODE_KP_9: return ScanCode::KeyPadNine;
			case SDL_SCANCODE_KP_0: return ScanCode::KeyPadZero;
			case SDL_SCANCODE_KP_PERIOD: return ScanCode::KeyPadPeriod;

			case SDL_SCANCODE_NONUSBACKSLASH: return ScanCode::NonusBackSlash;
			case SDL_SCANCODE_APPLICATION: return ScanCode::Application;
			case SDL_SCANCODE_POWER: return ScanCode::Power;

			case SDL_SCANCODE_KP_EQUALS: return ScanCode::KeyPadEquals;
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

			case SDL_SCANCODE_KP_COMMA: return ScanCode::KeyPadComma;
			case SDL_SCANCODE_KP_EQUALSAS400: return ScanCode::KeyPadEqualsAS400;

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

			case SDL_SCANCODE_KP_00: return ScanCode::KeyPad00;
			case SDL_SCANCODE_KP_000: return ScanCode::KeyPad000;
			case SDL_SCANCODE_THOUSANDSSEPARATOR: return ScanCode::ThousandsSeparator;
			case SDL_SCANCODE_DECIMALSEPARATOR: return ScanCode::DecimalSeparator;
			case SDL_SCANCODE_CURRENCYUNIT: return ScanCode::CurrencyUnit;
			case SDL_SCANCODE_CURRENCYSUBUNIT: return ScanCode::CurrencySubUnit;
			case SDL_SCANCODE_KP_LEFTPAREN: return ScanCode::KeyPadLeftParenthesis;
			case SDL_SCANCODE_KP_RIGHTPAREN: return ScanCode::KeyPadRightParenthesis;
			case SDL_SCANCODE_KP_LEFTBRACE: return ScanCode::KeyPadLeftBrace;
			case SDL_SCANCODE_KP_RIGHTBRACE: return ScanCode::KeyPadRightBrace;
			case SDL_SCANCODE_KP_TAB: return ScanCode::KeyPadTab;
			case SDL_SCANCODE_KP_BACKSPACE: return ScanCode::KeyPadBackspace;
			case SDL_SCANCODE_KP_A: return ScanCode::KeyPadA;
			case SDL_SCANCODE_KP_B: return ScanCode::KeyPadB;
			case SDL_SCANCODE_KP_C: return ScanCode::KeyPadC;
			case SDL_SCANCODE_KP_D: return ScanCode::KeyPadD;
			case SDL_SCANCODE_KP_E: return ScanCode::KeyPadE;
			case SDL_SCANCODE_KP_F: return ScanCode::KeyPadF;
			case SDL_SCANCODE_KP_XOR: return ScanCode::KeyPadXor;
			case SDL_SCANCODE_KP_POWER: return ScanCode::KeyPadPower;
			case SDL_SCANCODE_KP_PERCENT: return ScanCode::KeyPadPercent;
			case SDL_SCANCODE_KP_LESS: return ScanCode::KeyPadLess;
			case SDL_SCANCODE_KP_GREATER: return ScanCode::KeyPadGreater;
			case SDL_SCANCODE_KP_AMPERSAND: return ScanCode::KeyPadAmpersand;
			case SDL_SCANCODE_KP_DBLAMPERSAND: return ScanCode::KeyPadDBLAmpersand;
			case SDL_SCANCODE_KP_VERTICALBAR: return ScanCode::KeyPadVerticalBar;
			case SDL_SCANCODE_KP_DBLVERTICALBAR: return ScanCode::KeyPadDBLVerticalBar;
			case SDL_SCANCODE_KP_COLON: return ScanCode::KeyPadColon;
			case SDL_SCANCODE_KP_HASH: return ScanCode::KeyPadHash;
			case SDL_SCANCODE_KP_SPACE: return ScanCode::KeyPadSpace;
			case SDL_SCANCODE_KP_AT: return ScanCode::KeyPadAt;
			case SDL_SCANCODE_KP_EXCLAM: return ScanCode::KeyPadExclam;
			case SDL_SCANCODE_KP_MEMSTORE: return ScanCode::KeyPadMemstore;
			case SDL_SCANCODE_KP_MEMRECALL: return ScanCode::KeyPadMemRecall;
			case SDL_SCANCODE_KP_MEMCLEAR: return ScanCode::KeyPadMemClear;
			case SDL_SCANCODE_KP_MEMADD: return ScanCode::KeyPadMemAdd;
			case SDL_SCANCODE_KP_MEMSUBTRACT: return ScanCode::KeyPadMemSubtract;
			case SDL_SCANCODE_KP_MEMMULTIPLY: return ScanCode::KeyPadMemMultiply;
			case SDL_SCANCODE_KP_MEMDIVIDE: return ScanCode::KeyPadMemDivide;
			case SDL_SCANCODE_KP_PLUSMINUS: return ScanCode::KeyPadPlusMinus;
			case SDL_SCANCODE_KP_CLEAR: return ScanCode::KeyPadClear;
			case SDL_SCANCODE_KP_CLEARENTRY: return ScanCode::KeyPadClearEntry;
			case SDL_SCANCODE_KP_BINARY: return ScanCode::KeyPadBinary;
			case SDL_SCANCODE_KP_OCTAL: return ScanCode::KeyPadOctal;
			case SDL_SCANCODE_KP_DECIMAL: return ScanCode::KeyPadDecimal;
			case SDL_SCANCODE_KP_HEXADECIMAL: return ScanCode::KeyPadHexadecimal;

			case SDL_SCANCODE_LCTRL: return ScanCode::LeftControl;
			case SDL_SCANCODE_LSHIFT: return ScanCode::RightControl;
			case SDL_SCANCODE_LALT: return ScanCode::LeftAlt;
			case SDL_SCANCODE_LGUI: return ScanCode::LeftGUI;
			case SDL_SCANCODE_RCTRL: return ScanCode::RightControl;
			case SDL_SCANCODE_RSHIFT: return ScanCode::RightShift;
			case SDL_SCANCODE_RALT: return ScanCode::RightAlt;
			case SDL_SCANCODE_RGUI: return ScanCode::RightGUI;

			case SDL_SCANCODE_MODE: return ScanCode::Mode;

			case SDL_SCANCODE_SLEEP: return ScanCode::Sleep;
			case SDL_SCANCODE_WAKE: return ScanCode::Wake;

			case SDL_SCANCODE_CHANNEL_INCREMENT: return ScanCode::ChannelIncrement;
			case SDL_SCANCODE_CHANNEL_DECREMENT: return ScanCode::ChannelDecrement;

			case SDL_SCANCODE_MEDIA_PLAY: return ScanCode::MediaPlay;
			case SDL_SCANCODE_MEDIA_PAUSE: return ScanCode::MediaPause;
			case SDL_SCANCODE_MEDIA_RECORD: return ScanCode::MediaRecord;
			case SDL_SCANCODE_MEDIA_FAST_FORWARD: return ScanCode::MediaFastForward;
			case SDL_SCANCODE_MEDIA_REWIND: return ScanCode::MediaRewind;
			case SDL_SCANCODE_MEDIA_NEXT_TRACK: return ScanCode::MediaNextTrack;
			case SDL_SCANCODE_MEDIA_PREVIOUS_TRACK: return ScanCode::MediaPreviousTrack;
			case SDL_SCANCODE_MEDIA_STOP: return ScanCode::MediaStop;
			case SDL_SCANCODE_MEDIA_EJECT: return ScanCode::MediaEject;
			case SDL_SCANCODE_MEDIA_PLAY_PAUSE: return ScanCode::MediaPlayPause;
			case SDL_SCANCODE_MEDIA_SELECT: return ScanCode::MediaSelect;

			case SDL_SCANCODE_AC_NEW: return ScanCode::AcNew;
			case SDL_SCANCODE_AC_OPEN: return ScanCode::AcOpen;
			case SDL_SCANCODE_AC_CLOSE: return ScanCode::AcClose;
			case SDL_SCANCODE_AC_EXIT: return ScanCode::AcExit;
			case SDL_SCANCODE_AC_SAVE: return ScanCode::AcSave;
			case SDL_SCANCODE_AC_PRINT: return ScanCode::AcPrint;
			case SDL_SCANCODE_AC_PROPERTIES: return ScanCode::AcProperties;

			case SDL_SCANCODE_AC_SEARCH: return ScanCode::AcSearch;
			case SDL_SCANCODE_AC_HOME: return ScanCode::AcHome;
			case SDL_SCANCODE_AC_BACK: return ScanCode::AcBack;
			case SDL_SCANCODE_AC_FORWARD: return ScanCode::AcForward;
			case SDL_SCANCODE_AC_STOP: return ScanCode::AcStop;
			case SDL_SCANCODE_AC_REFRESH: return ScanCode::AcRefresh;
			case SDL_SCANCODE_AC_BOOKMARKS: return ScanCode::AcBookmarks;

			case SDL_SCANCODE_SOFTLEFT: return ScanCode::SoftLeft;
			case SDL_SCANCODE_SOFTRIGHT: return ScanCode::SoftRight;

			case SDL_SCANCODE_CALL: return ScanCode::Call;
			case SDL_SCANCODE_ENDCALL: return ScanCode::EndCall;

			default: throw std::runtime_error("Failed to find a valid scancode");
		}
	}

	uint16_t GetNexusModifiersFromSDLModifiers(Uint16 modifiers)
	{
		uint16_t mods = 0;

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

	std::optional<MouseButton> GetMouseButton(Uint8 mouseButton)
	{
		if (mouseButton == SDL_BUTTON_LEFT)
		{
			return MouseButton::Left;
		}
		else if (mouseButton == SDL_BUTTON_MIDDLE)
		{
			return MouseButton::Middle;
		}
		else if (mouseButton == SDL_BUTTON_RIGHT)
		{
			return MouseButton::Right;
		}
		else if (mouseButton == SDL_BUTTON_X1)
		{
			return MouseButton::X1;
		}
		else if (mouseButton == SDL_BUTTON_X2)
		{
			return MouseButton::X2;
		}

		return {};
	}
}	 // namespace Nexus::SDL3