#pragma once

#include "Core/Point.h"

namespace Nexus
{
    enum MouseButtonState
    {
        Released,
        Pressed,
    };

    struct MouseState
    {
        MouseButtonState LeftButton;
        MouseButtonState RightButton;
        MouseButtonState MiddleButton;
        Point<int> MousePosition;
        float MouseWheelX;
        float MouseWheelY;
        Point<int> MouseWheel;
    };

    class Mouse
    {
    public:
        void CacheInput();

        bool WasLeftMouseClicked();
        bool WasLeftMouseReleased();
        bool IsLeftMouseHeld();

        bool WasRightMouseClicked();
        bool WasRightMouseReleased();
        bool IsRightMouseHeld();

        bool WasMiddleMouseClicked();
        bool WasMiddleMouseReleased();
        bool IsMiddleMouseHeld();

        Point<int> GetMousePosition();
        Point<int> GetMouseMovement();

        Point<int> GetScroll();
        Point<int> GetScrollMovement();

    private:
        MouseState m_CurrentState;
        MouseState m_PreviousState;

        friend class Window;
    };
}