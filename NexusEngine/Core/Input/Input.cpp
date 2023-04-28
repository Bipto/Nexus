#include "Input.h"

#include "Core/Runtime.h"

namespace Nexus
{
    MouseButtonState Input::GetLeftMouseButtonState()
    {
        return Nexus::GetApplication()->GetCoreInput()->GetLeftMouseState();
    }

    MouseButtonState Input::GetRightMouseButtonState()
    {
        return Nexus::GetApplication()->GetCoreInput()->GetRightMouseState();
    }

    Point Input::GetMousePosition()
    {
        return Nexus::GetApplication()->GetCoreInput()->GetMousePosition();
    }

    float Input::GetMouseScrollX()
    {
        return Nexus::GetApplication()->GetCoreInput()->GetMouseScrollX();
    }

    float Input::GetMouseScrollY()
    {
        return Nexus::GetApplication()->GetCoreInput()->GetMouseScrollY();
    }

    bool Input::IsKeyPressed(KeyCode code)
    {
        return Nexus::GetApplication()->GetCoreInput()->IsKeyPressed(code);
    }

    bool Input::IsKeyReleased(KeyCode code)
    {
        return !Nexus::GetApplication()->GetCoreInput()->IsKeyPressed(code);
    }
}