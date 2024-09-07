#include "Mouse.hpp"

namespace Nexus {
Point2D<int> Mouse::s_GlobalMousePosition = {0, 0};
MouseState Mouse::s_GlobalMouseState;

void Mouse::CacheInput() { m_PreviousState = m_CurrentState; }

const bool Mouse::WasLeftMouseClicked() const {
  return m_CurrentState.LeftButton == MouseButton::Pressed &&
         m_PreviousState.LeftButton == MouseButton::Released;
}

const bool Mouse::WasLeftMouseReleased() const {
  return m_CurrentState.LeftButton == MouseButton::Released &&
         m_PreviousState.LeftButton == MouseButton::Pressed;
}

const bool Mouse::IsLeftMouseHeld() const {
  return m_CurrentState.LeftButton == MouseButton::Pressed;
}

const bool Mouse::WasRightMouseClicked() const {
  return m_CurrentState.RightButton == MouseButton::Pressed &&
         m_PreviousState.RightButton == MouseButton::Released;
}

const bool Mouse::WasRightMouseReleased() const {
  return m_CurrentState.RightButton == MouseButton::Released &&
         m_PreviousState.RightButton == MouseButton::Pressed;
}

const bool Mouse::IsRightMouseHeld() const {
  return m_CurrentState.RightButton == MouseButton::Pressed;
}

const bool Mouse::WasMiddleMouseClicked() const {
  return m_CurrentState.MiddleButton == MouseButton::Pressed &&
         m_PreviousState.MiddleButton == MouseButton::Released;
}

const bool Mouse::WasMiddleMouseReleased() const {
  return m_CurrentState.MiddleButton == MouseButton::Released &&
         m_PreviousState.MiddleButton == MouseButton::Pressed;
}

const bool Mouse::IsMiddleMouseHeld() const {
  return m_CurrentState.MiddleButton == MouseButton::Pressed;
}

const Point2D<int> Mouse::GetMousePosition() const {
  return m_CurrentState.MousePosition;
}

const Point2D<int> Mouse::GetMouseMovement() const {
  return {m_CurrentState.MousePosition.X - m_PreviousState.MousePosition.X,
          m_CurrentState.MousePosition.Y - m_PreviousState.MousePosition.Y};
}

const Point2D<int> Mouse::GetScroll() const {
  return m_CurrentState.MouseWheel;
}

const Point2D<int> Mouse::GetScrollMovement() const {
  return {m_CurrentState.MouseWheel.X - m_PreviousState.MouseWheel.X,
          m_CurrentState.MouseWheel.Y - m_PreviousState.MouseWheel.Y};
}

Point2D<int> Mouse::GetGlobalMousePosition() { return s_GlobalMousePosition; }

bool Mouse::IsGlobalLeftMouseHeld() {
  return s_GlobalMouseState.LeftButton == MouseButton::Pressed;
}

bool Mouse::IsGlobalRightMouseHeld() {
  return s_GlobalMouseState.RightButton == MouseButton::Pressed;
}

bool Mouse::IsGlobalMiddleMouseHeld() {
  return s_GlobalMouseState.MiddleButton == MouseButton::Pressed;
}
} // namespace Nexus