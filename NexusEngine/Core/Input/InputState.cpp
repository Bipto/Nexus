#include "InputState.h"

namespace Nexus
{
    void InputState::CachePreviousInput()
    {
        m_Keyboard.CacheInput();
        m_Mouse.CacheInput();

        for (auto gamepad : m_Gamepads)
        {
            gamepad->Update();
        }
    }
    void InputState::ConnectController(uint32_t index)
    {
        Gamepad *gamepad = new Gamepad(index);
        m_Gamepads.push_back(gamepad);
    }

    void InputState::RemoveController(uint32_t index)
    {
        for (int i = 0; i < m_Gamepads.size(); i++)
        {
            auto gamepad = m_Gamepads[i];
            if (gamepad->GetControllerIndex() == index)
            {
                auto &gamepad = m_Gamepads[i];
                delete gamepad;
                m_Gamepads.erase(m_Gamepads.begin() + i);
            }
        }
    }
    Gamepad *InputState::GetGamepadByIndex(uint32_t index)
    {
        for (int i = 0; i < m_Gamepads.size(); i++)
        {
            auto gamepad = m_Gamepads[i];
            if (gamepad->GetControllerIndex() == index)
                return gamepad;
        }
        return nullptr;
    }
}