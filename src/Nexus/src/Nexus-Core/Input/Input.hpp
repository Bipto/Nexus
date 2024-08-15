#pragma once

#include "InputState.hpp"

namespace Nexus
{
/// @brief A static class representing the current input state of an application
class Input
{
  public:
    //-----------------------------------------------------------------------------
    //                                    MOUSE
    //-----------------------------------------------------------------------------

    /// @brief A static method representing whether the left mouse button was pressed
    /// @return A boolean value representing whether the left mouse button was pressed
    static bool IsLeftMousePressed();

    /// @brief A static method representing whether the left mouse button was released
    /// @return A boolean value representing whether the left mouse button was released
    static bool IsLeftMouseReleased();

    /// @brief A static method representing whether the left mouse button was held
    /// @return A boolean value representing whether the left mouse button was held
    static bool IsLeftMouseHeld();

    /// @brief A static method representing whether the right mouse button was pressed
    /// @return A boolean value representing whether the right mouse button was pressed
    static bool IsRightMousePressed();

    /// @brief A static method representing whether the right mouse button was released
    /// @return A boolean value representing whether the right mouse button was released
    static bool IsRightMouseReleased();

    /// @brief A static method representing whether the right mouse button was held
    /// @return A boolean value representing whether the right mouse button was held
    static bool IsRightMouseHeld();

    /// @brief A static method representing whether the middle mouse button was pressed
    /// @return A boolean value representing whether the middle mouse button was pressed
    static bool IsMiddleMousePressed();

    /// @brief A static method representing whether the middle mouse button was released
    /// @return A boolean value representing whether the middle mouse button was released
    static bool IsMiddleMouseReleased();

    /// @brief A static method representing whether the middle mouse button was held
    /// @return A boolean value representing whether the middle mouse button was held
    static bool IsMiddleMouseHeld();

    /// @brief A static method representing the current position of the mouse pointer
    /// @return A Nexus::Point containing two integer representing the position of the mouse pointer
    static Point2D<int> GetMousePosition();

    /// @brief A static method representing the movement of the mouse pointer between frames
    /// @return A Nexus::Point containing two integer representing the movement of the mouse pointer
    static Point2D<int> GetMouseMovement();

    /// @brief A static method representing the current X position of the mouse scroll wheel
    /// @return A floating point value representing the mouse scroll X since the start of the application
    static float GetMouseScrollX();

    /// @brief A static method representing the mouse scroll X movement between frames
    /// @return A floating point value representing the change in mouse scroll X between frames
    static float GetMouseScrollMovementX();

    /// @brief A static method representing the current Y position of the mouse scroll wheel
    /// @return A floating point value representing the mouse scroll Y since the start of the application
    static float GetMouseScrollY();

    /// @brief A static method representing the mouse scroll Y movement between frames
    /// @return A floating point value representing the change in mouse scroll Y between frames
    static float GetMouseScrollMovementY();

    //-----------------------------------------------------------------------------
    //                                  KEYBOARD
    //-----------------------------------------------------------------------------

    /// @brief A static method representing whether a key was pressed on the keyboard
    /// @param code An enum value representing the key to check the state of
    /// @return A boolean value representing whether the key is pressed
    static bool IsKeyPressed(KeyCode code);

    /// @brief A static method representing whether a key was released on the keyboard
    /// @param code An enum value representing the key to check the state of
    /// @return A boolean value representing whether the key is released
    static bool IsKeyReleased(KeyCode code);

    /// @brief A static method representing whether a key was held on the keyboard
    /// @param code An enum value representing the key to check the state of
    /// @return A boolean value representing whether the key is held
    static bool IsKeyHeld(KeyCode code);

    //-----------------------------------------------------------------------------
    //                                   Gamepad
    //-----------------------------------------------------------------------------

    /// @brief A static method representing whether any gamepads are connected to the device
    /// @return A boolean value representing whether any gamepads are available
    static bool IsGamepadConnected();

    /// @brief A static method representing the number of gamepads connected to the device
    /// @return An integer value representing the number of gamepads connected
    static int GetGamepadCount();

    /// @brief A static method representing whether a key was held on a gamepad
    /// @param index The index of the controller to check
    /// @param button An enum value representing the button to check
    /// @return A boolean value representing whether the key was held
    static bool IsGamepadKeyHeld(uint32_t index, GamepadButton button);

    /// @brief A static method representing whether a key was pressed on a gamepad
    /// @param index The index of the controller to check
    /// @param button An enum value representing the button to check
    /// @return A boolean value representing whether the key was pressed
    static bool WasGamepadKeyPressed(uint32_t index, GamepadButton button);

    /// @brief A static method representing whether a key was released on a gamepad
    /// @param index The index of the controller to check
    /// @param button An enum value representing the button to check
    /// @return A boolean value representing whether the key was released
    static bool WasGamepadKeyReleased(uint32_t index, GamepadButton button);

    /// @brief A static method representing the current state of the gamepad left axis
    /// @param index The index of the controller to check
    /// @return A Nexus::Point containing two floating point values representing the position of the left axis
    static Point2D<float> GetGamepadAxisLeft(uint32_t index);

    /// @brief A static method representing the current state of the gamepad right axis
    /// @param index The index of the controller to check
    /// @return A Nexus::Point containing two floating point values representing the position of the right axis
    static Point2D<float> GetGamepadAxisRight(uint32_t index);

    /// @brief A static method representing the current state of the gamepad left trigger
    /// @param index The index of the controller to check
    /// @return A floating point value representing the current position of the left trigger
    static float GetGamepadLeftTrigger(uint32_t index);

    /// @brief A static method representing the current state of the gamepad right trigger
    /// @param index The index of the controller to check
    /// @return A floating point value representing the current position of the right trigger
    static float GetGamepadRightTrigger(uint32_t index);

    /// @brief A static method to rumble a controller
    /// @param index The index of the controller to rumble
    /// @param lowFrequency The low frequency to use for the rumble
    /// @param highFrequency The high frequency to use for the rumble
    /// @param milliseconds The number of milliseconds to rumble for
    static void GamepadRumble(uint32_t index, uint16_t lowFrequency, uint16_t highFrequency, uint32_t milliseconds);

    /// @brief A static method to rumble a controller's triggers
    /// @param index The index of the controller to rumble triggers of`
    /// @param left The rumble to use for the left trigger
    /// @param right The rumble to use for the right trigger
    /// @param milliseconds The number of milliseconds to rumble for
    static void GamepadRumbleTriggers(uint32_t index, uint16_t left, uint16_t right, uint32_t milliseconds);

    /// @brief A static method to set the colour of a controller's LED
    /// @param index The index of the controller to set the LED of
    /// @param red The red channel of the colour
    /// @param green The green channel of the colour
    /// @param blue The blue channel of the colour
    static void GamepadSetLED(uint32_t index, uint8_t red, uint8_t green, uint8_t blue);

    static void SetInputContext(const InputState *input);

    static const InputState *GetCurrentInputContext();

    Gamepad *GetGamepadByIndex(uint32_t index);

  private:
    static std::vector<Gamepad *> s_Gamepads;

    static void AddController(uint32_t index);
    static void RemoveController(uint32_t index);
    void CacheInput();

    static const InputState *s_InputContext;

    friend class Application;
};
}; // namespace Nexus