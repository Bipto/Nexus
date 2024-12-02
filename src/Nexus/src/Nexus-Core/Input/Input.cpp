#include "Input.hpp"

#include "Nexus-Core/Runtime.hpp"

namespace Nexus
{
	bool Input::IsMouseButtonDown(uint32_t id, MouseButton button)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsMouseButtonDown(id, button);
	}

	bool Input::IsMouseButtonUp(uint32_t id, MouseButton button)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsMouseButtonUp(id, button);
	}

	bool Input::IsKeyDown(uint32_t id, ScanCode scancode)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsKeyDown(id, scancode);
	}

	bool Input::IsKeyUp(uint32_t id, ScanCode scancode)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsKeyUp(id, scancode);
	}

	Point2D<float> Input::GetMousePosition(uint32_t id)
	{
		if (!s_InputContext)
		{
			return {};
		}

		return s_InputContext->GetMousePosition(id);
	}

	Point2D<float> Input::GetScroll(uint32_t id)
	{
		if (!s_InputContext)
		{
			return {};
		}

		return s_InputContext->GetScroll(id);
	}

	bool Input::IsMouseButtonDown(MouseButton button)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsMouseButtonDown(button);
	}

	bool Input::IsMouseButtonUp(MouseButton button)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsMouseButtonUp(button);
	}

	bool Input::IsKeyDown(ScanCode scancode)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsKeyDown(scancode);
	}

	bool Input::IsKeyUp(ScanCode scancode)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsKeyUp(scancode);
	}

	Point2D<float> Input::GetMousePosition()
	{
		if (!s_InputContext)
		{
			return {};
		}

		return s_InputContext->GetMousePosition();
	}

	Point2D<float> Input::GetScroll()
	{
		if (!s_InputContext)
		{
			return {};
		}

		return s_InputContext->GetScroll();
	}

	Point2D<float> Input::GetCursorPosition()
	{
		if (!s_InputContext)
		{
			return {};
		}

		return s_InputContext->GetCursorPosition();
	}

	Point2D<float> Input::GetGlobalCursorPosition()
	{
		if (!s_InputContext)
		{
			return {};
		}

		return s_InputContext->GetGlobalCursorPosition();
	}

	InputNew::InputContext *Input::GetContext()
	{
		return s_InputContext;
	}

	void Input::SetContext(InputNew::InputContext *context)
	{
		s_InputContext = context;
	}
}	 // namespace Nexus