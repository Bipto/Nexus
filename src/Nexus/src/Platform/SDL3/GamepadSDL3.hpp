#pragma once

#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "SDL3Include.hpp"

namespace Nexus
{
	class GamepadSDL3 : public IGamepad
	{
	  public:
		/// @brief An empty controller cannot be created
		GamepadSDL3() = delete;

		/// @brief A constructor that takes in an index to a connected controller
		/// @param index An unsigned 32 bit integer representing the index of the
		/// connected controller
		GamepadSDL3(uint32_t index);

		/// @brief A destructor that is used to clean up SDL resources when a
		/// controller is deleted
		virtual ~GamepadSDL3();

		/// @brief An method that queries the currently pressed buttons and current
		/// axis states
		void Update();

		/// @brief A method that returns the current position of the left stick
		/// @return A Nexus::Point containing two floating point values containing the
		/// position of the left stick
		const Point2D<float> GetLeftStick() const final;

		/// @brief A method that returns the current position of the right stick
		/// @return A Nexus::Point containing two floating point values containing the
		/// position of the right stick
		const Point2D<float> GetRightStick() const final;

		/// @brief A method that returns the current position of the left trigger
		/// @return A floating point value representing the position of the left
		/// trigger
		const float GetLeftTrigger() const final;

		/// @brief A method that returns the current position of the right trigger
		/// @return A floating point value representing the position of the  right
		/// trigger
		const float GetRightTrigger() const final;

		/// @brief A method that returns whether a button on the gamepad is held or
		/// not
		/// @param button An enum value representing the button to check
		/// @return A boolean representing whether the button is held
		const bool IsButtonHeld(GamepadButton button) const final;

		/// @brief A method that returns whether a button on the gamepad is pressed or
		/// not
		/// @param button An enum value representing the button to check
		/// @return A boolean representing whether the button is pressed
		const bool WasButtonPressed(GamepadButton button) const final;

		/// @brief A method that returns whether a button on the gamepad is released
		/// or not
		/// @param button An enum value representing the button to check
		/// @return A boolean representing whether the button is released
		const bool WasButtonReleased(GamepadButton button) const final;

		/// @brief A method that returns the currently set deadzone of the gamepad
		/// @return An integer representing the current deadzone being used on the
		/// gamepad
		const int GetDeadzone() const final;

		/// @brief A method that sets the deadzone of the gamepad
		/// @param deadzone An integer value representing the new deadzone to use
		void SetDeadzone(const int deadzone) final;

		/// @brief A method that checks whether a gamepad has a touchpad or not
		/// @return A boolean value representing whether a gamepad contains a touchpad
		bool HasTouchpad() final;

		/// @brief A method that sets the colour of an LED in a controller
		/// @param red The red channel of the colour value
		/// @param green The green channel of the colour value
		/// @param blue The blue channel of the colour value
		void SetLED(uint8_t red, uint8_t green, uint8_t blue) final;

		/// @brief A method that rumbles a controller
		/// @param lowFrequency An unsigned 16 bit integer representing the low
		/// frequency to use
		/// @param highFrequency An unsigned 16 bit integer representing the high
		/// frequency to use
		/// @param milliseconds An unsigned 32 bit integer representing how long the
		/// gamepad should rumble for
		void Rumble(uint16_t lowFrequency, uint16_t highFrequency, uint32_t milliseconds) final;

		/// @brief A method that rumbles a controller's triggers
		/// @param left An unsigned 16 bit integer representing the value to use to
		/// rumble the left trigger
		/// @param right An unsigned 16 bit integer representing the value to use to
		/// rumble the right trigger
		/// @param milliseconds An unsigned 32 bit integer representing how long the
		/// triggers should rumble for
		void RumbleTriggers(uint16_t left, uint16_t right, uint32_t milliseconds) final;

		/// @brief A method that returns the index of the connected controller
		/// @return An unsigned 32 bit integer representing the index of the
		/// controller
		const uint32_t GetControllerIndex() final;

	  private:
		/// @brief A Nexus::Point containing the current state of the left stick axis
		Point2D<float> m_LeftStick;

		/// @brief A Nexus::Point containing the current state of the right stick axis
		Point2D<float> m_RightStick;

		/// @brief A floating point value containing the current state of the left
		/// trigger
		float m_LeftTrigger = 0.0f;

		/// @brief A floating point value containing the current state of the right
		/// trigger
		float m_RightTrigger = 0.0f;

		/// @brief A map containing the current state of the buttons on the controller
		std::map<GamepadButton, bool> m_CurrentButtons;

		/// @brief A map containing the state of the buttons on the controller in the
		/// previous frame
		std::map<GamepadButton, bool> m_PreviousButtons;

		/// @brief A pointer to an underlying SDL_GameController object
		SDL_Gamepad *m_GameController = nullptr;

		/// @brief An integer containing the current deadzone of the controller
		int m_Deadzone = 8000;

		/// @brief An unsigned 32 bit integer representing the index of the
		/// SDL_GameController
		uint32_t m_Index = 0;
	};
}	 // namespace Nexus