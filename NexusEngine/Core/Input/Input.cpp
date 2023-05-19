#include "Input.h"

#include "Core/Runtime.h"

namespace Nexus
{
    bool Input::IsLeftMousePressed()
    {
        return (Nexus::GetApplication()->GetCoreInputState()->GetMouseState().LeftButton == MouseButtonState::Pressed) && 
            (Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().LeftButton == MouseButtonState::Released);
    }

    bool Input::IsLeftMouseReleased()
    {
        return (Nexus::GetApplication()->GetCoreInputState()->GetMouseState().LeftButton == MouseButtonState::Released) && 
            (Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().LeftButton == MouseButtonState::Pressed);
    }

    bool Input::IsLeftMouseHeld()
    {
        return (Nexus::GetApplication()->GetCoreInputState()->GetMouseState().LeftButton == MouseButtonState::Pressed) && 
            (Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().LeftButton == MouseButtonState::Pressed);
    }

    bool Input::IsRightMousePressed()
    {
        return (Nexus::GetApplication()->GetCoreInputState()->GetMouseState().RightButton == MouseButtonState::Pressed) && 
            (Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().RightButton == MouseButtonState::Released);
    }

    bool Input::IsRightMouseReleased()
    {
        return (Nexus::GetApplication()->GetCoreInputState()->GetMouseState().RightButton == MouseButtonState::Released) && 
            (Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().RightButton == MouseButtonState::Pressed);
    }

    bool Input::IsRightMouseHeld()
    {
        return (Nexus::GetApplication()->GetCoreInputState()->GetMouseState().RightButton == MouseButtonState::Pressed) && 
            (Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().RightButton == MouseButtonState::Pressed);
    }

    Point Input::GetMousePosition()
    {
        return Nexus::GetApplication()->GetCoreInputState()->GetMouseState().MousePosition;
    }

    Point Input::GetMouseMovement()
    {
        auto currentPosition = Nexus::GetApplication()->GetCoreInputState()->GetMouseState().MousePosition;
        auto previousPosition = Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().MousePosition;
        return { currentPosition.X - previousPosition.X, currentPosition.Y - previousPosition.Y };
    }

    float Input::GetMouseScrollX()
    {
        return Nexus::GetApplication()->GetCoreInputState()->GetMouseState().MouseWheelX;
    }

    float Input::GetMouseScrollMovementX()
    {
        return Nexus::GetApplication()->GetCoreInputState()->GetMouseState().MouseWheelX - 
            Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().MouseWheelX;
    }

    float Input::GetMouseScrollY()
    {
        return Nexus::GetApplication()->GetCoreInputState()->GetMouseState().MouseWheelY;
    }

    float Input::GetMouseScrollMovementY()
    {
        return Nexus::GetApplication()->GetCoreInputState()->GetMouseState().MouseWheelY - 
            Nexus::GetApplication()->GetCoreInputState()->GetPreviousMouseState().MouseWheelY;
    }

    bool Input::IsGamepadConnected()
    {
        return Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates().size() > 0;
    }

    int Input::GetGamepadCount()
    {
        return Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates().size();
    }

    bool Input::IsGamepadKeyHeld(uint32_t index, GamepadButton button)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->IsGamepadButtonPressed(button);
    }

    int Input::GetGamepadLeftXAxis(uint32_t index)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->GetLeftAxisX();
    }

    int Input::GetGamepadLeftYAxis(uint32_t index)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->GetLeftAxisY();
    }

    int Input::GetGamepadRightXAxis(uint32_t index)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->GetRightAxisX();
    }

    int Input::GetGamepadRightYAxis(uint32_t index)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->GetRightAxisY();    }

    int Input::GetGamepadLeftXAxisNormalized(uint32_t index)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->GetLeftAxisXNormalized();
    }

    int Input::GetGamepadLeftYAxisNormalized(uint32_t index)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->GetLeftAxisYNormalized();
    }

    int Input::GetGamepadRightXAxisNormalized(uint32_t index)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->GetRightAxisXNormalized();
    }

    int Input::GetGamepadRightYAxisNormalized(uint32_t index)
    {
        auto gamepadState = Nexus::GetApplication()->GetCoreInputState()->GetGamepadStates()[index];
        return gamepadState->GetRightAxisYNormalized();
    }

    bool Input::IsKeyPressed(KeyCode code)
    {
        auto keys = Nexus::GetApplication()->GetCoreInputState()->GetKeyboardState();
        return keys[code];
    }

    bool Input::IsKeyReleased(KeyCode code)
    {
        auto keys = Nexus::GetApplication()->GetCoreInputState()->GetKeyboardState();
        return !keys[code];
    }

    bool Input::IsKeyHeld(KeyCode code)
    {
        auto keys = Nexus::GetApplication()->GetCoreInputState()->GetKeyboardState();
        auto previousKeys = Nexus::GetApplication()->GetCoreInputState()->GetPreviousKeyboardState();
        return keys[code] && previousKeys[code];
    }
}