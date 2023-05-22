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
        Max
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

        const bool IsGamepadButtonHeld(GamepadButton button) const;

        const int GetDeadzone() const;
        void SetDeadzone(const int deadzone);

        const uint32_t GetControllerIndex();

    private:
        Point<float> m_LeftStick;
        Point<float> m_RightStick;

        float m_LeftTrigger = 0.0f;
        float m_RightTrigger = 0.0f;

        SDL_GameController *m_GameController = nullptr;
        int m_Deadzone = 8000;
        uint32_t m_Index = 0;
        friend class Window;
    };
}