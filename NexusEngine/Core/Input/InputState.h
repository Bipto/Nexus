#pragma once

#include "Core/Point.h"
#include "Core/Logging/Log.h"

#include "SDL.h"
#include "SDL_gamecontroller.h"

#include <map>
#include <sstream>
#include <vector>

const int JOYSTICK_DEAD_ZONE = 8000;

namespace Nexus
{
    enum class KeyCode
    {
        Unknown = 0,

        Escape,
        
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,

        PrintScreen,
        ScrollLock,
        PauseBreak,

        Tilde,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Zero,
        Underscore,
        Equals,
        Back,

        Tab,
        Q,
        W,
        E,
        R,
        T,
        Y,
        U,
        I,
        O,
        P,
        LeftBracket,
        RightBracket,
        Enter,

        CapsLock,
        A,
        S,
        D,
        F,
        G,
        H,
        J,
        K,
        L,
        SemiColon,
        Apostrophe,
        Hash,

        LeftShift,
        Backslash,
        Z,
        X,
        C,
        V,
        B,
        N,
        M,
        Comma,
        Period,
        Slash,
        RightShift,

        LeftControl,
        LeftWin,
        LeftAlt,
        Space,
        RightAlt,
        RightWin,
        RightControl,

        Insert,
        Home,
        PageUp,
        Delete,
        End,
        PageDown,

        KeyLeft,
        KeyUp,
        KeyDown,
        KeyRight,

        NumLock,
        KeyDivide,
        NumMultiply,
        NumSubstract,
        Num7,
        Num8,
        Num9,
        NumPlus,
        Num4,
        Num5,
        Num6,
        Num1,
        Num2,
        Num3,
        NumEnter,
        Num0,
        NumDelete,
    };

    enum MouseButtonState
    {
        Released,
        Pressed,
    };

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

    struct MouseState
    {
        MouseButtonState LeftButton;
        MouseButtonState RightButton;
        Point MousePosition;
        float MouseWheelX;
        float MouseWheelY;
    };

    SDL_GameControllerButton GetSDLGamepadButtonFromNexusKeyCode(GamepadButton button);

    struct GamepadState
    {
        public:
            GamepadState(uint32_t index)
            {
                if (SDL_IsGameController(index))
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
                        NX_LOG("Connected controller successfully");
                    }
                }
            }
            ~GamepadState()
            {
                SDL_GameControllerClose(m_GameController);
            }

            const int GetLeftAxisXNormalized() const
            {
                auto axis = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);

                if (axis < -JOYSTICK_DEAD_ZONE)
                {
                    return -1;
                }
                else if (axis > JOYSTICK_DEAD_ZONE)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            const int GetLeftAxisYNormalized() const
            {
                auto axis = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
                if (axis < -JOYSTICK_DEAD_ZONE)
                {
                    return -1;
                }
                else if (axis > JOYSTICK_DEAD_ZONE)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            const int GetRightAxisXNormalized() const
            {
                auto axis = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);

                if (axis < -JOYSTICK_DEAD_ZONE)
                {
                    return -1;
                }
                else if (axis > JOYSTICK_DEAD_ZONE)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            const int GetRightAxisYNormalized() const
            {
                auto axis = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);
                if (axis < -JOYSTICK_DEAD_ZONE)
                {
                    return -1;
                }
                else if (axis > JOYSTICK_DEAD_ZONE)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            const int GetLeftAxisX() const
            {
                auto axis = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);

                if (axis < -JOYSTICK_DEAD_ZONE)
                {
                    return -1;
                }
                else if (axis > JOYSTICK_DEAD_ZONE)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            const int GetLeftAxisY() const
            {
                auto axis = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
                if (axis < -JOYSTICK_DEAD_ZONE)
                {
                    return axis;
                }
                else if (axis > JOYSTICK_DEAD_ZONE)
                {
                    return axis;
                }
                else
                {
                    return 0;
                }
            }
            const int GetRightAxisX() const
            {
                auto axis = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);

                if (axis < -JOYSTICK_DEAD_ZONE)
                {
                    return axis;
                }
                else if (axis > JOYSTICK_DEAD_ZONE)
                {
                    return axis;
                }
                else
                {
                    return 0;
                }
            }
            const int GetRightAxisY() const
            {
                auto axis = SDL_GameControllerGetAxis(m_GameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);
                if (axis < -JOYSTICK_DEAD_ZONE)
                {
                    return axis;
                }
                else if (axis > JOYSTICK_DEAD_ZONE)
                {
                    return axis;
                }
                else
                {
                    return 0;
                }
            }

            const bool IsGamepadButtonPressed(GamepadButton button) const
            {
                return (bool)SDL_GameControllerGetButton(m_GameController, GetSDLGamepadButtonFromNexusKeyCode(button));
            }
        private:
            SDL_GameController* m_GameController = nullptr;
    };

    class InputState
    {
        public:

            //-----------------------------------------------------------------------------
            // SET
            //-----------------------------------------------------------------------------

            void SetKeyState(KeyCode code, bool pressed)
            {
                m_KeyboardState[code] = pressed;
            }

            void SetLeftMouseState(MouseButtonState state)
            {
                m_MouseState.LeftButton = state;
            }

            void SetRightMouseState(MouseButtonState state)
            {
                m_MouseState.RightButton = state;
            }

            void SetMousePosX(int pos)
            {
                m_MouseState.MousePosition.X = pos;
            }

            void SetMousePosY(int pos)
            {
                m_MouseState.MousePosition.Y = pos;
            }

            void SetMouseScrollX(float scroll)
            {
                m_MouseState.MouseWheelX = scroll;
            }

            void SetMouseScrollY(float scroll)
            {
                m_MouseState.MouseWheelY = scroll;
            }

            void CachePreviousInput()
            {
                for (auto key : m_KeyboardState)
                {
                    m_PreviousKeyState[key.first] = key.second;
                }

                m_PreviousMouseState.LeftButton = m_MouseState.LeftButton;
                m_PreviousMouseState.RightButton = m_MouseState.RightButton;
                m_PreviousMouseState.MousePosition = m_MouseState.MousePosition;
                m_PreviousMouseState.MouseWheelX = m_MouseState.MouseWheelX;
                m_PreviousMouseState.MouseWheelY = m_MouseState.MouseWheelY;
            }

            //-----------------------------------------------------------------------------
            // GET
            //-----------------------------------------------------------------------------

            const std::map<KeyCode, bool>& GetKeyboardState()
            {
                return m_KeyboardState;
            }

            const std::map<KeyCode, bool>& GetPreviousKeyboardState()
            {
                return m_PreviousKeyState;
            }

            MouseState GetMouseState()
            {
                return m_MouseState;
            }

            MouseState GetPreviousMouseState()
            {
                return m_PreviousMouseState;
            }

            void ConnectController(uint32_t index)
            {
                GamepadState* gamepadState = new GamepadState(index);
                m_GamepadStates.push_back(gamepadState);
            }

            const std::vector<GamepadState*>& GetGamepadStates()
            {
                return m_GamepadStates;
            }

        private:
            std::map<KeyCode, bool> m_KeyboardState;
            std::map<KeyCode, bool> m_PreviousKeyState;
            MouseState m_MouseState;
            MouseState m_PreviousMouseState;
            std::vector<GamepadState*> m_GamepadStates;
    };
}