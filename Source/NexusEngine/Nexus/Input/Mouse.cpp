#include "Mouse.hpp"

namespace Nexus
{
    Point<int> Mouse::s_GlobalMousePosition = {0, 0};
    MouseState Mouse::s_GlobalMouseState;

    void Mouse::CacheInput()
    {
        m_PreviousState = m_CurrentState;
    }

    bool Mouse::WasLeftMouseClicked() const
    {
        return m_CurrentState.LeftButton == MouseButtonState::Pressed && m_PreviousState.LeftButton == MouseButtonState::Released;
    }

    bool Mouse::WasLeftMouseReleased() const
    {
        return m_CurrentState.LeftButton == MouseButtonState::Released && m_PreviousState.LeftButton == MouseButtonState::Pressed;
    }

    bool Mouse::IsLeftMouseHeld() const
    {
        return m_CurrentState.LeftButton == MouseButtonState::Pressed;
    }

    bool Mouse::WasRightMouseClicked() const
    {
        return m_CurrentState.RightButton == MouseButtonState::Pressed && m_PreviousState.RightButton == MouseButtonState::Released;
    }

    bool Mouse::WasRightMouseReleased() const
    {
        return m_CurrentState.RightButton == MouseButtonState::Released && m_PreviousState.RightButton == MouseButtonState::Pressed;
    }

    bool Mouse::IsRightMouseHeld() const
    {
        return m_CurrentState.RightButton == MouseButtonState::Pressed;
    }

    bool Mouse::WasMiddleMouseClicked() const
    {
        return m_CurrentState.MiddleButton == MouseButtonState::Pressed && m_PreviousState.MiddleButton == MouseButtonState::Released;
    }

    bool Mouse::WasMiddleMouseReleased() const
    {
        return m_CurrentState.MiddleButton == MouseButtonState::Released && m_PreviousState.MiddleButton == MouseButtonState::Pressed;
    }

    bool Mouse::IsMiddleMouseHeld() const
    {
        return m_CurrentState.MiddleButton == MouseButtonState::Pressed;
    }

    Point<int> Mouse::GetMousePosition() const
    {
        return m_CurrentState.MousePosition;
    }

    Point<int> Mouse::GetMouseMovement() const
    {
        return {
            m_CurrentState.MousePosition.X - m_PreviousState.MousePosition.X,
            m_CurrentState.MousePosition.Y - m_PreviousState.MousePosition.Y};
    }

    Point<int> Mouse::GetScroll() const
    {
        return m_CurrentState.MouseWheel;
    }

    Point<int> Mouse::GetScrollMovement() const
    {
        return {
            m_CurrentState.MouseWheel.X - m_PreviousState.MouseWheel.X,
            m_CurrentState.MouseWheel.Y - m_PreviousState.MouseWheel.Y};
    }

    Point<int> Mouse::GetGlobalMousePosition()
    {
        return s_GlobalMousePosition;
    }

    bool Mouse::IsGlobalLeftMouseHeld()
    {
        return s_GlobalMouseState.LeftButton == MouseButtonState::Pressed;
    }

    bool Mouse::IsGlobalRightMouseHeld()
    {
        return s_GlobalMouseState.RightButton == MouseButtonState::Pressed;
    }

    bool Mouse::IsGlobalMiddleMouseHeld()
    {
        return s_GlobalMouseState.MiddleButton == MouseButtonState::Pressed;
    }
}