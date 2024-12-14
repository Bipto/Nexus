#pragma once

#include "InputContext.hpp"

namespace Nexus
{
	/// @brief A static class representing the current input state of an application
	class Input
	{
	  public:
		static bool			  IsMouseButtonDown(uint32_t id, MouseButton button);
		static bool			  IsMouseButtonUp(uint32_t id, MouseButton button);
		static bool			  IsKeyDown(uint32_t id, ScanCode scancode);
		static bool			  IsKeyUp(uint32_t id, ScanCode scancode);
		static Point2D<float> GetMousePosition(uint32_t id);
		static Point2D<float> GetScroll(uint32_t id);

		static bool			  IsMouseButtonDown(MouseButton button);
		static bool			  IsMouseButtonUp(MouseButton button);
		static bool			  IsKeyDown(ScanCode scancode);
		static bool			  IsKeyUp(ScanCode scancode);
		static Point2D<float> GetMousePosition();
		static Point2D<float> GetScroll();

		static Point2D<float> GetCursorPosition();
		static Point2D<float> GetGlobalCursorPosition();

	  public:
		static InputNew::InputContext *GetContext();
		static void					   SetContext(InputNew::InputContext *context);

	  private:
		static inline InputNew::InputContext *s_InputContext = nullptr;
	};
};	  // namespace Nexus