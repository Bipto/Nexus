#include "Nexus-Core/Input/InputContext.hpp"

#include "Nexus-Core/Platform.hpp"

#include "Nexus-Core/Window.hpp"

namespace Nexus::InputNew
{
	InputContext::InputContext(Nexus::IWindow *window)
		: m_Window(window),
		  m_OnKeyPressed(&window->OnKeyPressed, [this](const KeyPressedEventArgs &args) { OnKeyPressed(args); }),
		  m_OnKeyReleased(&window->OnKeyReleased, [this](const KeyReleasedEventArgs &args) { OnKeyReleased(args); }),
		  m_OnMouseButtonPressed(&window->OnMousePressed, [this](const MouseButtonPressedEventArgs &args) { OnMouseButtonPressed(args); }),
		  m_OnMouseButtonReleased(&window->OnMouseReleased, [this](const MouseButtonReleasedEventArgs &args) { OnMouseButtonReleased(args); }),
		  m_OnMouseMoved(&window->OnMouseMoved, [this](const MouseMovedEventArgs &args) { OnMouseMoved(args); }),
		  m_OnMouseScrolled(&window->OnScroll, [this](const MouseScrolledEventArgs &args) { OnScroll(args); })
	{
	}

	InputContext::~InputContext()
	{
	}

	bool InputContext::IsMouseButtonDown(uint32_t id, MouseButton button)
	{
		return m_MouseStates[id].Buttons[button];
	}

	bool InputContext::IsMouseButtonUp(uint32_t id, MouseButton button)
	{
		return !m_MouseStates[id].Buttons[button];
	}

	bool InputContext::IsKeyDown(uint32_t id, ScanCode scancode)
	{
		return m_KeyboardStates[id].Keys[scancode];
	}

	bool InputContext::IsKeyUp(uint32_t id, ScanCode scancode)
	{
		return !m_KeyboardStates[id].Keys[scancode];
	}

	Point2D<float> InputContext::GetMousePosition(uint32_t id)
	{
		return m_MouseStates[id].Position;
	}

	Point2D<float> InputContext::GetScroll(uint32_t id)
	{
		return m_MouseStates[id].Scroll;
	}

	bool InputContext::IsMouseButtonDown(MouseButton button)
	{
		std::optional<uint32_t> activeMouse = Platform::GetActiveMouseId();

		if (activeMouse.has_value())
		{
			return m_MouseStates[activeMouse.value()].Buttons[button];
		}

		return false;
	}

	bool InputContext::IsMouseButtonUp(MouseButton button)
	{
		std::optional<uint32_t> activeMouse = Platform::GetActiveMouseId();

		if (activeMouse.has_value())
		{
			return !m_MouseStates[activeMouse.value()].Buttons[button];
		}

		return false;
	}

	bool InputContext::IsKeyDown(ScanCode scancode)
	{
		std::optional<uint32_t> activeKeyboard = Platform::GetActiveKeyboardId();

		if (activeKeyboard.has_value())
		{
			return m_KeyboardStates[activeKeyboard.value()].Keys[scancode];
		}

		return false;
	}

	bool InputContext::IsKeyUp(ScanCode scancode)
	{
		std::optional<uint32_t> activeKeyboard = Platform::GetActiveKeyboardId();

		if (activeKeyboard.has_value())
		{
			return !m_KeyboardStates[activeKeyboard.value()].Keys[scancode];
		}

		return false;
	}

	Point2D<float> InputContext::GetMousePosition()
	{
		std::optional<uint32_t> activeMouse = Platform::GetActiveMouseId();

		if (activeMouse.has_value())
		{
			return m_MouseStates[activeMouse.value()].Position;
		}

		return {0.0f, 0.0f};
	}

	Point2D<float> InputContext::GetScroll()
	{
		std::optional<uint32_t> activeMouse = Platform::GetActiveMouseId();

		if (activeMouse.has_value())
		{
			return m_MouseStates[activeMouse.value()].Scroll;
		}

		return {0.0f, 0.0f};
	}

	Point2D<float> InputContext::GetCursorPosition()
	{
		return m_CursorPosition;
	}

	Point2D<float> Nexus::InputNew::InputContext::GetGlobalCursorPosition()
	{
		return Platform::GetGlobalMouseInfo().Position;
	}

	void InputContext::Reset()
	{
		for (auto &[id, mouseState] : m_MouseStates)
		{
			mouseState.Movement		  = {};
			mouseState.ScrollMovement = {};
		}
	}

	void InputContext::OnKeyPressed(const KeyPressedEventArgs &args)
	{
		m_KeyboardStates[args.KeyboardID].Keys[args.ScanCode] = true;
	}

	void InputContext::OnKeyReleased(const KeyReleasedEventArgs &args)
	{
		m_KeyboardStates[args.KeyboardID].Keys[args.ScanCode] = false;
	}

	void InputContext::OnMouseButtonPressed(const MouseButtonPressedEventArgs &args)
	{
		m_MouseStates[args.MouseID].Buttons[args.Button] = true;
	}

	void InputContext::OnMouseButtonReleased(const MouseButtonReleasedEventArgs &args)
	{
		m_MouseStates[args.MouseID].Buttons[args.Button] = false;
	}

	void InputContext::OnMouseMoved(const MouseMovedEventArgs &args)
	{
		m_MouseStates[args.MouseID].Position = args.Position;
		m_MouseStates[args.MouseID].Movement = args.Movement;
		m_CursorPosition					 = args.Position;
	}

	void InputContext::OnScroll(const MouseScrolledEventArgs &args)
	{
		m_MouseStates[args.MouseID].Scroll.X += args.Scroll.X;
		m_MouseStates[args.MouseID].Scroll.Y += args.Scroll.Y;
		m_MouseStates[args.MouseID].ScrollMovement = args.Scroll;
	}
}	 // namespace Nexus::InputNew
