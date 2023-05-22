#include "Keyboard.h"

namespace Nexus
{
    void Keyboard::CacheInput()
    {
        m_PreviousKeys = m_CurrentKeys;
    }

    bool Keyboard::IsKeyHeld(KeyCode code)
    {
        auto current = m_CurrentKeys[code];
        return current;
    }

    bool Keyboard::WasKeyPressed(KeyCode code)
    {
        auto current = m_CurrentKeys[code];
        auto previous = m_PreviousKeys[code];
        return current && !previous;
    }

    bool Keyboard::WasKeyReleased(KeyCode code)
    {
        auto current = m_CurrentKeys[code];
        auto previous = m_PreviousKeys[code];
        return !current && previous;
    }
}