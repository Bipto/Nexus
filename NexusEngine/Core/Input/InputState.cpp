#include "InputState.h"

SDL_GameControllerButton Nexus::GetSDLGamepadButtonFromNexusKeyCode(GamepadButton button)
{
    switch (button)
        {
            case GamepadButton::DpadUp:         return SDL_CONTROLLER_BUTTON_DPAD_UP;
            case GamepadButton::DpadDown:       return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
            case GamepadButton::DpadLeft:       return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
            case GamepadButton::DpadRight:      return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
            case GamepadButton::A:              return SDL_CONTROLLER_BUTTON_A;
            case GamepadButton::B:              return SDL_CONTROLLER_BUTTON_B;
            case GamepadButton::X:              return SDL_CONTROLLER_BUTTON_X;
            case GamepadButton::Y:              return SDL_CONTROLLER_BUTTON_Y;
            case GamepadButton::Back:           return SDL_CONTROLLER_BUTTON_BACK;
            case GamepadButton::Guide:          return SDL_CONTROLLER_BUTTON_GUIDE;
            case GamepadButton::Start:          return SDL_CONTROLLER_BUTTON_START;
            case GamepadButton::LeftStick:      return SDL_CONTROLLER_BUTTON_LEFTSTICK;
            case GamepadButton::RightStick:     return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
            case GamepadButton::LeftShoulder:   return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
            case GamepadButton::RightShoulder:  return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
            case GamepadButton::Misc:           return SDL_CONTROLLER_BUTTON_MISC1;
            case GamepadButton::Paddle1:        return SDL_CONTROLLER_BUTTON_PADDLE1;
            case GamepadButton::Paddle2:        return SDL_CONTROLLER_BUTTON_PADDLE2;
            case GamepadButton::Paddle3:        return SDL_CONTROLLER_BUTTON_PADDLE3;
            case GamepadButton::Paddle4:        return SDL_CONTROLLER_BUTTON_PADDLE4;
            case GamepadButton::Touchpad:       return SDL_CONTROLLER_BUTTON_TOUCHPAD;
            case GamepadButton::Max:            return SDL_CONTROLLER_BUTTON_MAX;
            default:                            return SDL_CONTROLLER_BUTTON_INVALID;
        }
}