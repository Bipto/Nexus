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
            static MouseButtonState GetLeftMouseButtonState();
            static MouseButtonState GetRightMouseButtonState();
            static Point GetMousePosition();
            static float GetMouseScrollX();
            static float GetMouseScrollY();

            //-----------------------------------------------------------------------------
            //                                  KEYBOARD
            //-----------------------------------------------------------------------------
            static bool IsKeyPressed(KeyCode code);
            static bool IsKeyReleased(KeyCode code);
    };
};