#pragma once

#include <optional>
#include <tuple>

#include <SDL3/SDL.h>

#include "Platform/Input/Events.hpp"
#include "Platform/Input/Gamepad.hpp"
#include "Platform/Input/Keyboard.hpp"
#include "Platform/Input/Mouse.hpp"
#include "Platform/Monitor.hpp"

namespace Nexus::SDL3
{
    KeyCode GetNexusKeyCodeFromSDLKeyCode(SDL_Keycode keycode);
    ScanCode GetNexusScanCodeFromSDLScanCode(SDL_Scancode scancode);
    uint16_t GetNexusModifiersFromSDLModifiers(Uint16 modifiers);

    FileDropType GetFileDropType(SDL_EventType type);
    std::tuple<MouseType, uint32_t> GetMouseInfo(SDL_MouseID mouseId);
    ScrollDirection GetScrollDirection(SDL_MouseWheelDirection scrollDirection);

    std::optional<MouseButton> GetMouseButton(Uint8 mouseButton);
} // namespace Nexus::SDL3