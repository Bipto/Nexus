#pragma once

#include "SDL.h"

#include "Nexus-Core/Point.hpp"

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
        Touchpad,

        /// @brief The max button that some controllers may have
        Max,
    };

    /// @brief A method that returns an SDL_GameControllerButton from a nexus gamepad button
    /// @param button The button to convert to an SDL game controller button
    /// @return An enum value representing an underlying SDL_GameControllerButton
    SDL_GamepadButton GetSDLGamepadButtonFromNexusKeyCode(GamepadButton button);

    /// @brief A class representing a gamepad that can be used for input
    class Gamepad
    {
    public:
        /// @brief An empty controller cannot be created
        Gamepad() = delete;

        /// @brief A constructor that takes in an index to a connected controller
        /// @param index An unsigned 32 bit integer representing the index of the connected controller
        Gamepad(uint32_t index);

        /// @brief A destructor that is used to clean up SDL resources when a controller is deleted
        ~Gamepad();

        /// @brief An method that queries the currently pressed buttons and current axis states
        void Update();

        /// @brief A method that returns the current position of the left stick
        /// @return A Nexus::Point containing two floating point values containing the position of the left stick
        const Point2D<float> GetLeftStick() const;

        /// @brief A method that returns the current position of the right stick
        /// @return A Nexus::Point containing two floating point values containing the position of the right stick
        const Point2D<float> GetRightStick() const;

        /// @brief A method that returns the current position of the left trigger
        /// @return A floating point value representing the position of the left trigger
        const float GetLeftTrigger() const;

        /// @brief A method that returns the current position of the right trigger
        /// @return A floating point value representing the position of the  right trigger
        const float GetRightTrigger() const;

        /// @brief A method that returns whether a button on the gamepad is held or not
        /// @param button An enum value representing the button to check
        /// @return A boolean representing whether the button is held
        const bool IsButtonHeld(GamepadButton button) const;

        /// @brief A method that returns whether a button on the gamepad is pressed or not
        /// @param button An enum value representing the button to check
        /// @return A boolean representing whether the button is pressed
        const bool WasButtonPressed(GamepadButton button) const;

        /// @brief A method that returns whether a button on the gamepad is released or not
        /// @param button An enum value representing the button to check
        /// @return A boolean representing whether the button is released
        const bool WasButtonReleased(GamepadButton button) const;

        /// @brief A method that returns the currently set deadzone of the gamepad
        /// @return An integer representing the current deadzone being used on the gamepad
        const int GetDeadzone() const;

        /// @brief A method that sets the deadzone of the gamepad
        /// @param deadzone An integer value representing the new deadzone to use
        void SetDeadzone(const int deadzone);

        /// @brief A method that checks whether a gamepad has a touchpad or not
        /// @return A boolean value representing whether a gamepad contains a touchpad
        bool HasTouchpad();

        /// @brief A method that sets the colour of an LED in a controller
        /// @param red The red channel of the colour value
        /// @param green The green channel of the colour value
        /// @param blue The blue channel of the colour value
        void SetLED(uint8_t red, uint8_t green, uint8_t blue);

        /// @brief A method that rumbles a controller
        /// @param lowFrequency An unsigned 16 bit integer representing the low frequency to use
        /// @param highFrequency An unsigned 16 bit integer representing the high frequency to use
        /// @param milliseconds An unsigned 32 bit integer representing how long the gamepad should rumble for
        void Rumble(uint16_t lowFrequency, uint16_t highFrequency, uint32_t milliseconds);

        /// @brief A method that rumbles a controller's triggers
        /// @param left An unsigned 16 bit integer representing the value to use to rumble the left trigger
        /// @param right An unsigned 16 bit integer representing the value to use to rumble the right trigger
        /// @param milliseconds An unsigned 32 bit integer representing how long the triggers should rumble for
        void RumbleTriggers(uint16_t left, uint16_t right, uint32_t milliseconds);

        /// @brief A method that returns the index of the connected controller
        /// @return An unsigned 32 bit integer representing the index of the controller
        const uint32_t GetControllerIndex();

    private:
        /// @brief A Nexus::Point containing the current state of the left stick axis
        Point2D<float> m_LeftStick;

        /// @brief A Nexus::Point containing the current state of the right stick axis
        Point2D<float> m_RightStick;

        /// @brief A floating point value containing the current state of the left trigger
        float m_LeftTrigger = 0.0f;

        /// @brief A floating point value containing the current state of the right trigger
        float m_RightTrigger = 0.0f;

        /// @brief A map containing the current state of the buttons on the controller
        std::map<GamepadButton, bool> m_CurrentButtons;

        /// @brief A map containing the state of the buttons on the controller in the previous frame
        std::map<GamepadButton, bool> m_PreviousButtons;

        /// @brief A pointer to an underlying SDL_GameController object
        SDL_Gamepad *m_GameController = nullptr;

        /// @brief An integer containing the current deadzone of the controller
        int m_Deadzone = 8000;

        /// @brief An unsigned 32 bit integer representing the index of the SDL_GameController
        uint32_t m_Index = 0;

        /// @brief A friend class to allow a window to access the private members of this class
        friend class Window;
    };
}