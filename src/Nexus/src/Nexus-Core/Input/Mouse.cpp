#include "Mouse.hpp"

namespace Nexus
{
	void Mouse::CacheInput()
	{
		m_PreviousState = m_CurrentState;
	}

	const bool Mouse::WasLeftMouseClicked() const
	{
		return m_CurrentState.LeftButton == MouseButtonState::Pressed && m_PreviousState.LeftButton == MouseButtonState::Released;
	}

	const bool Mouse::WasLeftMouseReleased() const
	{
		return m_CurrentState.LeftButton == MouseButtonState::Released && m_PreviousState.LeftButton == MouseButtonState::Pressed;
	}

	const bool Mouse::IsLeftMouseHeld() const
	{
		return m_CurrentState.LeftButton == MouseButtonState::Pressed;
	}

	const bool Mouse::WasRightMouseClicked() const
	{
		return m_CurrentState.RightButton == MouseButtonState::Pressed && m_PreviousState.RightButton == MouseButtonState::Released;
	}

	const bool Mouse::WasRightMouseReleased() const
	{
		return m_CurrentState.RightButton == MouseButtonState::Released && m_PreviousState.RightButton == MouseButtonState::Pressed;
	}

	const bool Mouse::IsRightMouseHeld() const
	{
		return m_CurrentState.RightButton == MouseButtonState::Pressed;
	}

	const bool Mouse::WasMiddleMouseClicked() const
	{
		return m_CurrentState.MiddleButton == MouseButtonState::Pressed && m_PreviousState.MiddleButton == MouseButtonState::Released;
	}

	const bool Mouse::WasMiddleMouseReleased() const
	{
		return m_CurrentState.MiddleButton == MouseButtonState::Released && m_PreviousState.MiddleButton == MouseButtonState::Pressed;
	}

	const bool Mouse::IsMiddleMouseHeld() const
	{
		return m_CurrentState.MiddleButton == MouseButtonState::Pressed;
	}

	const Point2D<float> Mouse::GetMousePosition() const
	{
		return m_CurrentState.MousePosition;
	}

	const Point2D<float> Mouse::GetMouseMovement() const
	{
		return {m_CurrentState.MousePosition.X - m_PreviousState.MousePosition.X, m_CurrentState.MousePosition.Y - m_PreviousState.MousePosition.Y};
	}

	const Point2D<float> Mouse::GetScroll() const
	{
		return m_CurrentState.MouseWheel;
	}

	const Point2D<float> Mouse::GetScrollMovement() const
	{
		return {m_CurrentState.MouseWheel.X - m_PreviousState.MouseWheel.X, m_CurrentState.MouseWheel.Y - m_PreviousState.MouseWheel.Y};
	}

	Point2D<float> Mouse::GetGlobalMousePosition()
	{
		return s_GlobalMousePosition;
	}

	Point2D<float> Mouse::GetGlobalMouseMovement()
	{
		return s_GlobalMouseMovement;
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
}	 // namespace Nexus