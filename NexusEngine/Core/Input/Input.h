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
        static Point<float> GetGamepadAxisLeft(uint32_t index);
        static Point<float> GetGamepadAxisRight(uint32_t index);
    };
};