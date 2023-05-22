#pragma once

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

    class Keyboard
    {
    public:
        void CacheInput();

        bool IsKeyHeld(KeyCode code);
        bool WasKeyPressed(KeyCode code);
        bool WasKeyReleased(KeyCode code);

    private:
        std::map<KeyCode, bool> m_CurrentKeys;
        std::map<KeyCode, bool> m_PreviousKeys;
        friend class Window;
    };
}