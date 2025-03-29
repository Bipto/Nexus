#pragma once

#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief An enum representing all of the buttons on a gamepad
	enum GamepadButton
	{
		/// @brief The up button on the Dpad
		DpadUp,

		/// @brief The down button on the Dpad
		DpadDown,

		/// @brief The left button on the Dpad
		DpadLeft,

		/// @brief The right button on the Dpad
		DpadRight,

		/// @brief The A button on the right control section
		A,

		/// @brief The B button on the right control section
		B,

		/// @brief The X button on the right control section
		X,

		/// @brief The Y button on the right control section
		Y,

		/// @brief The back button on the controller
		Back,

		/// @brief The guide button on the controller
		Guide,

		/// @brief The start button on the controller
		Start,

		/// @brief The left stick button
		LeftStick,

		/// @brief The right stick button
		RightStick,

		/// @brief The left shoulder button
		LeftShoulder,

		/// @brief The right shoulder button
		RightShoulder,

		/// @brief A misc button
		Misc,

		/// @brief The first of four optional paddle buttons
		LeftPaddle1,

		/// @brief The second of four optional paddle buttons
		LeftPaddle2,

		/// @brief The third of four optional paddle buttons
		RightPaddle1,

		/// @brief The last of the four optional paddle buttons
		RightPaddle2,

		/// @brief A touchpad button that some controllers may have
		Touchpad
	};

	class Gamepad
	{
	  public:
		Gamepad() = delete;
		Gamepad(uint32_t id, const std::string &name) : m_Id(id), m_Name(name)
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
}	 // namespace Nexus