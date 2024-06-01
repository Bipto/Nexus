#include "Keyboard.hpp"

namespace Nexus
{
    void Keyboard::CacheInput()
    {
        m_PreviousKeys = m_CurrentKeys;
    }

    const bool Keyboard::IsKeyHeld(KeyCode code) const
    {
        const auto &it = m_CurrentKeys.find(code);

        // we do not currently have a value stored for the key, so assume it is not held
        if (it == m_CurrentKeys.end())
        {
            return false;
        }

        return it->second;
    }

    const bool Keyboard::WasKeyPressed(KeyCode code) const
    {
        const auto &current = m_CurrentKeys.find(code);

        // we do not currently have a value stored for the key, so assume it is not held
        if (current == m_CurrentKeys.end())
        {
            return false;
        }

        const auto &previous = m_PreviousKeys.find(code);

        if (previous == m_CurrentKeys.end())
        {
            return false;
        }

        return current->second && !previous->second;
    }

    const bool Keyboard::WasKeyReleased(KeyCode code) const
    {
        const auto &current = m_CurrentKeys.find(code);

        // we do not currently have a value stored for the key, so assume it is not held
        if (current == m_CurrentKeys.end())
        {
            return false;
        }

        const auto &previous = m_PreviousKeys.find(code);

        if (previous == m_CurrentKeys.end())
        {
            return false;
        }

        return !current->second && previous->second;
    }

    const std::map<KeyCode, bool> &Keyboard::GetKeys() const
    {
        return m_CurrentKeys;
    }
}