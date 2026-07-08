#pragma once

#include <cstdint>
#include <optional>
#include <utility>

#include "Platform/Input/Gamepad.hpp"
#include "Platform/Input/Keyboard.hpp"
#include "Platform/Input/Mouse.hpp"
#include "Platform/Platform-Core.hpp"

namespace Nexus
{
    class IWindow;

    /// @brief A static class representing the current input state of an application
    class Input
    {
      public:
        NX_PLATFORM_API static bool IsMouseButtonDown(uint32_t id, MouseButton button);
        NX_PLATFORM_API static bool IsMouseButtonUp(uint32_t id, MouseButton button);
        NX_PLATFORM_API static bool IsKeyDown(uint32_t id, ScanCode scancode);
        NX_PLATFORM_API static bool IsKeyUp(uint32_t id, ScanCode scancode);
        NX_PLATFORM_API static std::pair<float, float> GetMousePosition(uint32_t id);
        NX_PLATFORM_API static std::pair<float, float> GetScroll(uint32_t id);

        NX_PLATFORM_API static bool IsMouseButtonDown(MouseButton button);
        NX_PLATFORM_API static bool IsMouseButtonUp(MouseButton button);
        NX_PLATFORM_API static bool IsKeyDown(ScanCode scancode);
        NX_PLATFORM_API static bool IsKeyUp(ScanCode scancode);
        NX_PLATFORM_API static std::optional<std::pair<float, float>> GetMousePosition();
        NX_PLATFORM_API static std::optional<std::pair<float, float>> GetScroll();

        NX_PLATFORM_API static std::optional<std::pair<float, float>> GetCursorPosition();
        NX_PLATFORM_API static std::pair<float, float> GetGlobalCursorPosition();

      public:
        NX_PLATFORM_API static IWindow *GetContext();
        NX_PLATFORM_API static void SetContext(IWindow *context);

      private:
        static inline IWindow *s_InputContext = nullptr;
    };
}; // namespace Nexus