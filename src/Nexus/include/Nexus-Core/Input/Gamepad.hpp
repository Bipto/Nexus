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

	/// @brief A class representing a gamepad that can be used for input
	class IGamepad
	{
	  public:
		/// @brief An empty controller cannot be created
		IGamepad() = delete;

		/// @brief A constructor that takes in an index to a connected controller
		/// @param index An unsigned 32 bit integer representing the index of the
		/// connected controller
		IGamepad(uint32_t index)
		{
		}

		/// @brief A destructor that is used to clean up SDL resources when a
		/// controller is deleted
		virtual ~IGamepad()
		{
		}

		/// @brief An method that queries the currently pressed buttons and current
		/// axis states
		virtual void Update() = 0;

		/// @brief A method that returns the current position of the left stick
		/// @return A Nexus::Point containing two floating point values containing the
		/// position of the left stick
		 virtual const Point2D<float> GetLeftStick() const = 0;

		 /// @brief A method that returns the current position of the right stick
		 /// @return A Nexus::Point containing two floating point values containing the
		 /// position of the right stick
		 virtual const Point2D<float> GetRightStick() const = 0;

		 /// @brief A method that returns the current position of the left trigger
		 /// @return A floating point value representing the position of the left
		 /// trigger
		 virtual const float GetLeftTrigger() const = 0;

		 /// @brief A method that returns the current position of the right trigger
		 /// @return A floating point value representing the position of the  right
		 /// trigger
		 virtual const float GetRightTrigger() const = 0;

		 /// @brief A method that returns whether a button on the gamepad is held or
		 /// not
		 /// @param button An enum value representing the button to check
		 /// @return A boolean representing whether the button is held
		 virtual const bool IsButtonHeld(GamepadButton button) const = 0;

		 /// @brief A method that returns whether a button on the gamepad is pressed or
		 /// not
		 /// @param button An enum value representing the button to check
		 /// @return A boolean representing whether the button is pressed
		 virtual const bool WasButtonPressed(GamepadButton button) const = 0;

		 /// @brief A method that returns whether a button on the gamepad is released
		 /// or not
		 /// @param button An enum value representing the button to check
		 /// @return A boolean representing whether the button is released
		 virtual const bool WasButtonReleased(GamepadButton button) const = 0;

		 /// @brief A method that returns the currently set deadzone of the gamepad
		 /// @return An integer representing the current deadzone being used on the
		 /// gamepad
		 virtual const int GetDeadzone() const = 0;

		 /// @brief A method that sets the deadzone of the gamepad
		 /// @param deadzone An integer value representing the new deadzone to use
		 virtual void SetDeadzone(const int deadzone) = 0;

		 /// @brief A method that checks whether a gamepad has a touchpad or not
		 /// @return A boolean value representing whether a gamepad contains a touchpad
		 virtual bool HasTouchpad() = 0;

		 /// @brief A method that sets the colour of an LED in a controller
		 /// @param red The red channel of the colour value
		 /// @param green The green channel of the colour value
		 /// @param blue The blue channel of the colour value
		 virtual void SetLED(uint8_t red, uint8_t green, uint8_t blue) = 0;

		 /// @brief A method that rumbles a controller
		 /// @param lowFrequency An unsigned 16 bit integer representing the low
		 /// frequency to use
		 /// @param highFrequency An unsigned 16 bit integer representing the high
		 /// frequency to use
		 /// @param milliseconds An unsigned 32 bit integer representing how long the
		 /// gamepad should rumble for
		 virtual void Rumble(uint16_t lowFrequency, uint16_t highFrequency, uint32_t milliseconds) = 0;

		 /// @brief A method that rumbles a controller's triggers
		 /// @param left An unsigned 16 bit integer representing the value to use to
		 /// rumble the left trigger
		 /// @param right An unsigned 16 bit integer representing the value to use to
		 /// rumble the right trigger
		 /// @param milliseconds An unsigned 32 bit integer representing how long the
		 /// triggers should rumble for
		 virtual void RumbleTriggers(uint16_t left, uint16_t right, uint32_t milliseconds) = 0;

		 /// @brief A method that returns the index of the connected controller
		 /// @return An unsigned 32 bit integer representing the index of the
		 /// controller
		 virtual const uint32_t GetControllerIndex() = 0;
	};
}	 // namespace Nexus