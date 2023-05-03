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
}