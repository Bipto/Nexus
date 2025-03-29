#pragma once

#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus
{
	/// @brief An enum representing the state of a mouse button
	enum class MouseButtonState
	{
		/// @brief A value representing that the button is currently released
		Released = 0,

		/// @brief A value representing that the button is currently pressed
		Pressed,
	};

	enum class MouseButton
	{
		Left,
		Middle,
		Right,
		X1,
		X2
	};

	class Mouse
	{
	  public:
		Mouse() = delete;
		Mouse(uint32_t id, const std::string &name) : m_Id(id), m_Name(name)
		{
		}

		uint32_t GetId() const
		{
			return m_Id;
		}

		const std::string &GetName() const
		{
			return m_Name;
		}

	  private:
		uint32_t	m_Id;
		std::string m_Name;
	};

	/// @brief A struct containing the current state of the mouse
	struct MouseState
	{
		/// @brief A enum value representing the current state of the left mouse
		/// button
		MouseButtonState LeftButton;

		/// @brief A enum value representing the current state of the right mouse
		/// button
		MouseButtonState RightButton;

		/// @brief An enum value representing the current state of the middle mouse
		/// button
		MouseButtonState MiddleButton;

		/// @brief An enum value representing the current state of the X1 button
		MouseButtonState X1Button;

		/// @brief An enum value representing the current state of the X2 button
		MouseButtonState X2Button;

		/// @brief A Nexus::Point containing two integers representing the position of
		/// the mouse cursor
		Point2D<float> MousePosition;

		/// @brief A Nexus::Point containing two floats representing the movement of the mouse
		Point2D<float> MouseMovement;

		/// @brief A Nexus::Point containing two floats representing the current
		/// position of the mouse wheel
		Point2D<float> MouseWheel;
	};
}	 // namespace Nexus