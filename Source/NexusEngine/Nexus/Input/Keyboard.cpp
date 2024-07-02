#include "Keyboard.hpp"

namespace Nexus
{
    void Keyboard::CacheInput()
    {
        m_PreviousKeys = m_CurrentKeys;
    }

    const bool Keyboard::IsKeyHeld(KeyCode code) const
    {
        return m_CurrentKeys[code];
    }

    const bool Keyboard::WasKeyPressed(KeyCode code) const
    {
        return m_CurrentKeys[code] && !m_PreviousKeys[code];
    }

    const bool Keyboard::WasKeyReleased(KeyCode code) const
    {
        return !m_CurrentKeys[code] && m_PreviousKeys[code];
    }

    const std::map<KeyCode, bool> &Keyboard::GetKeys() const
    {
        return m_CurrentKeys;
    }
}