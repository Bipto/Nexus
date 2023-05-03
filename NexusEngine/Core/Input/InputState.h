#pragma once

#include "Core/Point.h"
#include <map>

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

    struct MouseState
    {
        MouseButtonState LeftButton;
        MouseButtonState RightButton;
        Point MousePosition;
        float MouseWheelX;
        float MouseWheelY;
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

        private:
            std::map<KeyCode, bool> m_KeyboardState;
            std::map<KeyCode, bool> m_PreviousKeyState;
            MouseState m_MouseState;
            MouseState m_PreviousMouseState;
    };
}