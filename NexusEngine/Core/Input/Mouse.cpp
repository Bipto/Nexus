#include "Mouse.h"

namespace Nexus
{
    void Mouse::CacheInput()
    {
        m_PreviousState = m_CurrentState;
    }

    bool Mouse::WasLeftMouseClicked()
    {
        return m_CurrentState.LeftButton == MouseButtonState::Pressed && m_PreviousState.LeftButton == MouseButtonState::Released;
    }

    bool Mouse::WasLeftMouseReleased()
    {
        return m_CurrentState.LeftButton == MouseButtonState::Released && m_PreviousState.LeftButton == MouseButtonState::Pressed;
    }

    bool Mouse::IsLeftMouseHeld()
    {
        return m_CurrentState.LeftButton == MouseButtonState::Pressed;
    }

    bool Mouse::WasRightMouseClicked()
    {
        return m_CurrentState.RightButton == MouseButtonState::Pressed && m_PreviousState.RightButton == MouseButtonState::Released;
    }

    bool Mouse::WasRightMouseReleased()
    {
        return m_CurrentState.RightButton == MouseButtonState::Released && m_PreviousState.RightButton == MouseButtonState::Pressed;
    }

    bool Mouse::IsRightMouseHeld()
    {
        return m_CurrentState.RightButton == MouseButtonState::Pressed;
    }

    bool Mouse::WasMiddleMouseClicked()
    {
        return m_CurrentState.MiddleButton == MouseButtonState::Pressed && m_PreviousState.MiddleButton == MouseButtonState::Released;
    }

    bool Mouse::WasMiddleMouseReleased()
    {
        return m_CurrentState.MiddleButton == MouseButtonState::Released && m_PreviousState.MiddleButton == MouseButtonState::Pressed;
    }

    bool Mouse::IsMiddleMouseHeld()
    {
        return m_CurrentState.MiddleButton == MouseButtonState::Pressed;
    }

    Point<int> Mouse::GetMousePosition()
    {
        return m_CurrentState.MousePosition;
    }

    Point<int> Mouse::GetMouseMovement()
    {
        return {
            m_CurrentState.MousePosition.X - m_PreviousState.MousePosition.X,
            m_CurrentState.MousePosition.Y - m_PreviousState.MousePosition.Y};
    }

    Point<int> Mouse::GetScroll()
    {
        return m_CurrentState.MouseWheel;
    }

    Point<int> Mouse::GetScrollMovement()
    {
        return {
            m_CurrentState.MouseWheel.X - m_PreviousState.MouseWheel.X,
            m_CurrentState.MouseWheel.Y - m_PreviousState.MouseWheel.Y};
    }
}