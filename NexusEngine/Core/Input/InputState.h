#pragma once

#include "Core/Point.h"
#include "Core/Logging/Log.h"

#include "SDL.h"
#include "SDL_gamecontroller.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

#include <map>
#include <sstream>
#include <vector>

const int JOYSTICK_DEAD_ZONE = 8000;

namespace Nexus
{
    class InputState
    {
    public:
        void CachePreviousInput()
        {
            m_Keyboard.CacheInput();
            m_Mouse.CacheInput();

            for (auto gamepad : m_Gamepads)
            {
                gamepad->Update();
            }
        }

        void ConnectController(uint32_t index)
        {
            Gamepad *gamepad = new Gamepad(index);
            m_Gamepads.push_back(gamepad);
        }

        void RemoveController(uint32_t index)
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

        Gamepad *GetGamepadByIndex(uint32_t index)
        {
            for (int i = 0; i < m_Gamepads.size(); i++)
            {
                auto gamepad = m_Gamepads[i];
                if (gamepad->GetControllerIndex() == index)
                    return gamepad;
            }
            return nullptr;
        }

        const Keyboard &GetKeyboard() { return m_Keyboard; }
        const Mouse &GetMouse() { return m_Mouse; }
        const std::vector<Gamepad *> &GetGamepads() { return m_Gamepads; }

    private:
        Keyboard m_Keyboard;
        Mouse m_Mouse;
        std::vector<Gamepad *> m_Gamepads;

        friend class Window;
    };
}