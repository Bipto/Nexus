#include "Input.h"

#include "Core/Runtime.h"

namespace Nexus
{
    bool Input::IsLeftMousePressed()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.WasLeftMouseClicked();
    }

    bool Input::IsLeftMouseReleased()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.WasLeftMouseReleased();
    }

    bool Input::IsLeftMouseHeld()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.IsLeftMouseHeld();
    }

    bool Input::IsRightMousePressed()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.WasRightMouseClicked();
    }

    bool Input::IsRightMouseReleased()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.WasRightMouseReleased();
    }

    bool Input::IsRightMouseHeld()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.IsRightMouseHeld();
    }

    Point<int> Input::GetMousePosition()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.GetMousePosition();
    }

    Point<int> Input::GetMouseMovement()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.GetMouseMovement();
    }

    float Input::GetMouseScrollX()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.GetScroll().X;
    }

    float Input::GetMouseScrollMovementX()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.GetScrollMovement().X;
    }

    float Input::GetMouseScrollY()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.GetScroll().Y;
    }

    float Input::GetMouseScrollMovementY()
    {
        auto mouse = Nexus::GetApplication()->GetCoreInputState()->GetMouse();
        return mouse.GetScrollMovement().Y;
    }

    bool Input::IsGamepadConnected()
    {
        auto gamepads = Nexus::GetApplication()->GetCoreInputState()->GetGamepads();
        return gamepads.size() > 0;
    }

    int Input::GetGamepadCount()
    {
        auto gamepads = Nexus::GetApplication()->GetCoreInputState()->GetGamepads();
        return gamepads.size();
    }

    bool Input::IsGamepadKeyHeld(uint32_t index, GamepadButton button)
    {
        if (GetGamepadCount() == 0)
            return false;
        auto gamepads = Nexus::GetApplication()->GetCoreInputState()->GetGamepads();
        return gamepads[index]->IsGamepadButtonHeld(button);
    }

    Point<float> Input::GetGamepadAxisLeft(uint32_t index)
    {
        if (GetGamepadCount() == 0)
            return {
                0, 0};

        auto gamepads = Nexus::GetApplication()->GetCoreInputState()->GetGamepads();
        return gamepads[index]->GetLeftStick();
    }

    Point<float> Input::GetGamepadAxisRight(uint32_t index)
    {
        if (GetGamepadCount() == 0)
            return {
                0, 0};

        auto gamepads = Nexus::GetApplication()->GetCoreInputState()->GetGamepads();
        return gamepads[index]->GetRightStick();
    }

    bool Input::IsKeyPressed(KeyCode code)
    {
        auto keyboard = Nexus::GetApplication()->GetCoreInputState()->GetKeyboard();
        return keyboard.WasKeyPressed(code);
    }

    bool Input::IsKeyReleased(KeyCode code)
    {
        auto keyboard = Nexus::GetApplication()->GetCoreInputState()->GetKeyboard();
        return keyboard.WasKeyReleased(code);
    }

    bool Input::IsKeyHeld(KeyCode code)
    {
        auto keyboard = Nexus::GetApplication()->GetCoreInputState()->GetKeyboard();
        return keyboard.IsKeyHeld(code);
    }
}