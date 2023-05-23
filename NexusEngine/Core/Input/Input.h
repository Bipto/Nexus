#pragma once

#include "InputState.h"

namespace Nexus
{
    class Input
    {
    public:
        //-----------------------------------------------------------------------------
        //                                    MOUSE
        //-----------------------------------------------------------------------------
        static bool IsLeftMousePressed();
        static bool IsLeftMouseReleased();
        static bool IsLeftMouseHeld();

        static bool IsRightMousePressed();
        static bool IsRightMouseReleased();
        static bool IsRightMouseHeld();

        static Point<int> GetMousePosition();
        static Point<int> GetMouseMovement();

        static float GetMouseScrollX();
        static float GetMouseScrollMovementX();

        static float GetMouseScrollY();
        static float GetMouseScrollMovementY();

        //-----------------------------------------------------------------------------
        //                                  KEYBOARD
        //-----------------------------------------------------------------------------
        static bool IsKeyPressed(KeyCode code);
        static bool IsKeyReleased(KeyCode code);
        static bool IsKeyHeld(KeyCode code);

        //-----------------------------------------------------------------------------
        //                                   Gamepad
        //-----------------------------------------------------------------------------

        static bool IsGamepadConnected();
        static int GetGamepadCount();

        static bool IsGamepadKeyHeld(uint32_t index, GamepadButton button);
        static bool WasGamepadKeyPressed(uint32_t index, GamepadButton button);
        static bool WasGamepadKeyReleased(uint32_t index, GamepadButton button);

        static Point<float> GetGamepadAxisLeft(uint32_t index);
        static Point<float> GetGamepadAxisRight(uint32_t index);
        static float GetGamepadLeftTrigger(uint32_t index);
        static float GetGamepadRightTrigger(uint32_t index);

        static bool GamepadSupportsRumble(uint32_t index);
        static bool GamepadSupportsRumbleTriggers(uint32_t index);
        static bool GamepadSupportsLED(uint32_t index);
        static void GamepadRumble(uint32_t index, uint16_t lowFrequency, uint16_t highFrequency, uint32_t milliseconds);
        static void GamepadRumbleTriggers(uint32_t index, uint16_t left, uint16_t right, uint32_t milliseconds);
        static void GamepadSetLED(uint32_t index, uint8_t red, uint8_t green, uint8_t blue);
    };
};