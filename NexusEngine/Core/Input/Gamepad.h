#pragma once

#include "SDL.h"
#include "SDL_gamecontroller.h"

#include "Core/Point.h"

namespace Nexus
{
    enum GamepadButton
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

    SDL_GameControllerButton GetSDLGamepadButtonFromNexusKeyCode(GamepadButton button);

    class Gamepad
    {
    public:
        Gamepad(uint32_t index);
        ~Gamepad();

        void Update();

        const Point<float> GetLeftStick() const;
        const Point<float> GetRightStick() const;

        const float GetLeftTrigger() const;
        const float GetRightTrigger() const;

        const bool IsButtonHeld(GamepadButton button) const;
        const bool WasButtonPressed(GamepadButton button) const;
        const bool WasButtonReleased(GamepadButton button) const;

        const int
        GetDeadzone() const;
        void SetDeadzone(const int deadzone);

        bool HasTouchpad();
        bool SupportsRumble();
        bool SupportsRumbleTriggers();
        bool SupportsLED();
        void Rumble(uint16_t lowFrequency, uint16_t highFrequency, uint32_t milliseconds);
        void RumbleTriggers(uint16_t left, uint16_t right, uint32_t milliseconds);
        void SetLED(uint8_t red, uint8_t green, uint8_t blue);

        const uint32_t GetControllerIndex();

    private:
        Point<float> m_LeftStick;
        Point<float> m_RightStick;

        float m_LeftTrigger = 0.0f;
        float m_RightTrigger = 0.0f;

        std::map<GamepadButton, bool> m_CurrentButtons;
        std::map<GamepadButton, bool> m_PreviousButtons;

        SDL_GameController *m_GameController = nullptr;
        int m_Deadzone = 8000;
        uint32_t m_Index = 0;
        friend class Window;
    };
}