#pragma once

#include "Core/nxpch.h"

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
        Pressed,
        Released,
    };

    struct MouseState
    {
        MouseButtonState LeftButton;
        MouseButtonState RightButton;

        int MouseX;
        int MouseY;
        float MouseWheelX;
        float MouseWheelY;
    };

    class Input
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
                m_MouseState.MouseX = pos;
            }

            void SetMousePosY(int pos)
            {
                m_MouseState.MouseY = pos;
            }

            void SetMouseScrollX(float scroll)
            {
                m_MouseState.MouseWheelX = scroll;
            }

            void SetMouseScrollY(float scroll)
            {
                m_MouseState.MouseWheelY = scroll;
            }

            //-----------------------------------------------------------------------------
            // GET
            //-----------------------------------------------------------------------------

            inline bool IsKeyPressed(KeyCode key) 
            {
                return m_KeyboardState[key];
            }

            MouseButtonState GetLeftMouseState()
            {
                return m_MouseState.LeftButton;
            }

            MouseButtonState GetRightMouseState()
            {
                return m_MouseState.RightButton;
            }

            int GetMousePosX()
            {
                return m_MouseState.MouseX;
            }

            int GetMousePosY()
            {
                return m_MouseState.MouseY;
            }

            float GetMouseScrollX()
            {
                return m_MouseState.MouseWheelX;
            }

            float GetMouseScrollY()
            {
                return m_MouseState.MouseWheelY;
            }

        private:
            std::map<KeyCode, bool> m_KeyboardState;
            MouseState m_MouseState;
    };
}