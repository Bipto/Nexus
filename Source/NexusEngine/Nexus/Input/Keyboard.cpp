#include "Keyboard.hpp"

namespace Nexus
{
    void Keyboard::CacheInput()
    {
        m_PreviousKeys = m_CurrentKeys;
    }

    const bool Keyboard::IsKeyHeld(KeyCode code) const
    {
        CacheKeyIfNecessary(code, false);
        return m_CurrentKeys.at(code);
    }

    const bool Keyboard::WasKeyPressed(KeyCode code) const
    {
        CacheKeyIfNecessary(code, false);
        return m_CurrentKeys.at(code) && !m_PreviousKeys.at(code);
    }

    const bool Keyboard::WasKeyReleased(KeyCode code) const
    {
        CacheKeyIfNecessary(code, false);
        return !m_CurrentKeys.at(code) && m_PreviousKeys.at(code);
    }

    const std::map<KeyCode, bool> &Keyboard::GetKeys() const
    {
        return m_CurrentKeys;
    }

    void Keyboard::CacheKeyIfNecessary(KeyCode code, bool value) const
    {
        m_CurrentKeys.try_emplace(code, value);
        m_PreviousKeys.try_emplace(code, value);
    }
}