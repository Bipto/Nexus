#pragma once

#include "Nexus-Core/IWindow.hpp"

#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief A static class representing the current input state of an application
	class Input
	{
	  public:
		NX_API static bool			 IsMouseButtonDown(uint32_t id, MouseButton button);
		NX_API static bool			 IsMouseButtonUp(uint32_t id, MouseButton button);
		NX_API static bool			 IsKeyDown(uint32_t id, ScanCode scancode);
		NX_API static bool			 IsKeyUp(uint32_t id, ScanCode scancode);
		NX_API static Point2D<float> GetMousePosition(uint32_t id);
		NX_API static Point2D<float> GetScroll(uint32_t id);

		NX_API static bool			 IsMouseButtonDown(MouseButton button);
		NX_API static bool			 IsMouseButtonUp(MouseButton button);
		NX_API static bool			 IsKeyDown(ScanCode scancode);
		NX_API static bool			 IsKeyUp(ScanCode scancode);
		NX_API static Point2D<float> GetMousePosition();
		NX_API static Point2D<float> GetScroll();

		NX_API static Point2D<float> GetCursorPosition();
		NX_API static Point2D<float> GetGlobalCursorPosition();

	  public:
		NX_API static IWindow *GetContext();
		NX_API static void	   SetContext(IWindow *context);

	  private:
		static inline IWindow *s_InputContext = nullptr;
	};
};	  // namespace Nexus