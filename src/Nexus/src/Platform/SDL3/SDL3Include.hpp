#pragma once

#include "SDL.h"

#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"

namespace Nexus::SDL3
{
	KeyCode				GetNexusKeyCodeFromSDLKeyCode(SDL_Keycode keycode);
	ScanCode			GetNexusScanCodeFromSDLScanCode(SDL_Scancode scancode);
	Keyboard::Modifiers GetNexusModifiersFromSDLModifiers(Uint16 modifiers);
}	 // namespace Nexus::SDL3