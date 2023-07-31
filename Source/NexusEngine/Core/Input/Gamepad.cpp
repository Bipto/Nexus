#include "Gamepad.hpp"

#include "Core/Logging/Log.hpp"

#include <sstream>

namespace Nexus
{
    Gamepad::Gamepad(uint32_t index)
    {
        m_GameController = SDL_GameControllerOpen(index);
        if (!m_GameController)
        {
            std::stringstream ss;
            ss << "Unable to open game controller: ";
            ss << SDL_GetError();
            NX_LOG(ss.str());
        }
        else
        {
            NX_LOG("Controller connected successfully");

            // initialize currently pressed keys and previously pressed key maps
            // this is required because a map is empty when constructed
            {
                std::vector<GamepadButton> enumValues =
                    {
                        DpadUp,
                        DpadDown,
                        DpadLeft,
                        DpadRight,
                        A,
                        B,
                        X,
                        Y,
                        Back,
                        Guide,
                        Start,
                        LeftStick,
                        RightStick,
                        LeftShoulder,
                        RightShoulder,
                        Misc,
                        Paddle1,
                        Paddle2,
                        Paddle3,
                        Paddle4,
                        Touchpad,
                        Max,
                    };

                for (auto val : enumValues)
                {
                    m_CurrentButtons[val] = false;
                }
                m_PreviousButtons = m_CurrentButtons;
            }
        }
    }

    Gamepad::~Gamepad()
    {
        SDL_GameControllerClose(m_GameController);
        NX_LOG("Controller removed");
    }

    void Gamepad::Update()
    {
        m_PreviousButtons = m_CurrentButtons;

        m_LeftStick.X = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
        m_LeftStick.Y = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
        m_RightStick.X = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
        m_RightStick.Y = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);
        m_LeftTrigger = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        m_RightTrigger = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

        for (auto &button : m_CurrentButtons)
        {
            auto sdlButton = GetSDLGamepadButtonFromNexusKeyCode(button.first);
            button.second = (bool)SDL_GameControllerGetButton(m_GameController, sdlButton);
        }
    }

    const Point<float> Gamepad::GetLeftStick() const
    {
        auto xAxis = m_LeftStick.X;
        auto yAxis = m_LeftStick.Y;

        if (xAxis < m_Deadzone && xAxis > -m_Deadzone)
        {
            xAxis = 0;
        }
        else
        {
            xAxis = xAxis / 32767.0f;
        }

        if (yAxis < m_Deadzone && yAxis > -m_Deadzone)
        {
            yAxis = 0;
        }
        else
        {
            yAxis = yAxis / 32767.0f;
        }

        return {
            (float)xAxis,
            (float)yAxis};
    }

    const Point<float> Gamepad::GetRightStick() const
    {
        auto xAxis = m_RightStick.X;
        auto yAxis = m_RightStick.Y;

        if (xAxis < m_Deadzone && xAxis > -m_Deadzone)
        {
            xAxis = 0;
        }
        else
        {
            xAxis = xAxis / 32767.0f;
        }

        if (yAxis < m_Deadzone && yAxis > -m_Deadzone)
        {
            yAxis = 0;
        }
        else
        {
            yAxis = yAxis / 32767.0f;
        }

        return {
            (float)xAxis,
            (float)yAxis};
    }

    const float Gamepad::GetLeftTrigger() const
    {
        return m_LeftTrigger / 32767.0f;
    }

    const float Gamepad::GetRightTrigger() const
    {
        return m_RightTrigger / 32767.0f;
    }

    const bool Gamepad::IsButtonHeld(GamepadButton button) const
    {
        return m_CurrentButtons.at(button);
    }

    const bool Gamepad::WasButtonPressed(GamepadButton button) const
    {
        auto current = m_CurrentButtons.at(button);
        auto previous = m_PreviousButtons.at(button);
        return current && !previous;
    }

    const bool Gamepad::WasButtonReleased(GamepadButton button) const
    {
        auto current = m_CurrentButtons.at(button);
        auto previous = m_PreviousButtons.at(button);
        return !current && previous;
    }

    const int Gamepad::GetDeadzone() const
    {
        return m_Deadzone;
    }

    void Gamepad::SetDeadzone(const int deadzone)
    {
        m_Deadzone = deadzone;
    }

    bool Gamepad::HasTouchpad()
    {
        return SDL_GameControllerGetNumTouchpads(m_GameController) > 0;
    }

    bool Gamepad::SupportsRumble()
    {
        return SDL_GameControllerHasRumble(m_GameController);
    }

    bool Gamepad::SupportsRumbleTriggers()
    {
        return SDL_GameControllerHasRumbleTriggers(m_GameController);
    }

    bool Gamepad::HasLED()
    {
        return SDL_GameControllerHasLED(m_GameController);
    }

    void Gamepad::SetLED(uint8_t red, uint8_t green, uint8_t blue)
    {
        SDL_GameControllerSetLED(m_GameController, red, green, blue);
    }

    void Gamepad::Rumble(uint16_t lowFrequency, uint16_t highFrequency, uint32_t milliseconds)
    {
        SDL_GameControllerRumble(m_GameController, lowFrequency, highFrequency, milliseconds);
    }

    void Gamepad::RumbleTriggers(uint16_t left, uint16_t right, uint32_t milliseconds)
    {
        SDL_GameControllerRumbleTriggers(m_GameController, left, right, milliseconds);
    }

    const uint32_t Gamepad::GetControllerIndex()
    {
        return m_Index;
    }

    SDL_GameControllerButton Nexus::GetSDLGamepadButtonFromNexusKeyCode(GamepadButton button)
    {
        switch (button)
        {
        case GamepadButton::DpadUp:
            return SDL_CONTROLLER_BUTTON_DPAD_UP;
        case GamepadButton::DpadDown:
            return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        case GamepadButton::DpadLeft:
            return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        case GamepadButton::DpadRight:
            return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        case GamepadButton::A:
            return SDL_CONTROLLER_BUTTON_A;
        case GamepadButton::B:
            return SDL_CONTROLLER_BUTTON_B;
        case GamepadButton::X:
            return SDL_CONTROLLER_BUTTON_X;
        case GamepadButton::Y:
            return SDL_CONTROLLER_BUTTON_Y;
        case GamepadButton::Back:
            return SDL_CONTROLLER_BUTTON_BACK;
        case GamepadButton::Guide:
            return SDL_CONTROLLER_BUTTON_GUIDE;
        case GamepadButton::Start:
            return SDL_CONTROLLER_BUTTON_START;
        case GamepadButton::LeftStick:
            return SDL_CONTROLLER_BUTTON_LEFTSTICK;
        case GamepadButton::RightStick:
            return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
        case GamepadButton::LeftShoulder:
            return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
        case GamepadButton::RightShoulder:
            return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
        case GamepadButton::Misc:
            return SDL_CONTROLLER_BUTTON_MISC1;
        case GamepadButton::Paddle1:
            return SDL_CONTROLLER_BUTTON_PADDLE1;
        case GamepadButton::Paddle2:
            return SDL_CONTROLLER_BUTTON_PADDLE2;
        case GamepadButton::Paddle3:
            return SDL_CONTROLLER_BUTTON_PADDLE3;
        case GamepadButton::Paddle4:
            return SDL_CONTROLLER_BUTTON_PADDLE4;
        case GamepadButton::Touchpad:
            return SDL_CONTROLLER_BUTTON_TOUCHPAD;
        case GamepadButton::Max:
            return SDL_CONTROLLER_BUTTON_MAX;
        default:
            return SDL_CONTROLLER_BUTTON_INVALID;
        }
    }
}