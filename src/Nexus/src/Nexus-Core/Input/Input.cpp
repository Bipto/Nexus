#include "Nexus-Core/Input/Input.hpp"

#include "Nexus-Core/Platform.hpp"
#include "Nexus-Core/Runtime.hpp"

namespace Nexus
{
	bool Input::IsMouseButtonDown(uint32_t id, MouseButton button)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsMouseButtonPressed(id, button);
	}

	bool Input::IsMouseButtonUp(uint32_t id, MouseButton button)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return !s_InputContext->IsMouseButtonPressed(id, button);
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

		return !s_InputContext->IsKeyDown(id, scancode);
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

		return s_InputContext->GetMouseScroll(id);
	}

	bool Input::IsMouseButtonDown(MouseButton button)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return s_InputContext->IsMouseButtonPressed(button);
	}

	bool Input::IsMouseButtonUp(MouseButton button)
	{
		if (!s_InputContext)
		{
			return false;
		}

		return !s_InputContext->IsMouseButtonPressed(button);
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

		return !s_InputContext->IsKeyDown(scancode);
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

		return s_InputContext->GetMouseScroll();
	}

	Point2D<float> Input::GetCursorPosition()
	{
		if (!s_InputContext)
		{
			return {};
		}

		return s_InputContext->GetMousePosition();
	}

	Point2D<float> Input::GetGlobalCursorPosition()
	{
		MouseState state = Platform::GetMouseState();
		return state.MousePosition;
	}

	IWindow *Input::GetContext()
	{
		return s_InputContext;
	}	 // namespace Nexus

	void Nexus::Input::SetContext(IWindow *context)
	{
		s_InputContext = context;
	}

}	 // namespace Nexus
