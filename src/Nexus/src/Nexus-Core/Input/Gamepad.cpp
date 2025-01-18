#include "Nexus-Core/Input/Gamepad.hpp"

#include "Nexus-Core/Logging/Log.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	Gamepad::Gamepad(uint32_t index)
	{
		m_GameController = SDL_OpenGamepad(index);
		if (!m_GameController)
		{
			std::stringstream ss;
			ss << "Unable to open game controller: ";
			ss << SDL_GetError();
			NX_LOG(ss.str());
		}
		else
		{
			NX_LOG("Controller connected successfully");

			// initialize currently pressed keys and previously pressed key maps
			// this is required because a map is empty when constructed
			{
				std::vector<GamepadButton> enumValues = {
					DpadUp,	  DpadDown,	   DpadLeft,	DpadRight,	  A,
					B,		  X,		   Y,			Back,		  Guide,
					Start,	  LeftStick,   RightStick,	LeftShoulder, RightShoulder,
					Misc,	  LeftPaddle1, LeftPaddle2, RightPaddle1, RightPaddle2,
					Touchpad,
				};

				for (auto val : enumValues) { m_CurrentButtons[val] = false; }
				m_PreviousButtons = m_CurrentButtons;
			}
		}
	}

	Gamepad::~Gamepad()
	{
		SDL_CloseGamepad(m_GameController);
		NX_LOG("Controller removed");
	}

	void Gamepad::Update()
	{
		m_PreviousButtons = m_CurrentButtons;

		m_LeftStick.X  = SDL_GetGamepadAxis(m_GameController, SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFTX);
		m_LeftStick.Y  = SDL_GetGamepadAxis(m_GameController, SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFTY);
		m_RightStick.X = SDL_GetGamepadAxis(m_GameController, SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHTX);
		m_RightStick.Y = SDL_GetGamepadAxis(m_GameController, SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHTY);
		m_LeftTrigger  = SDL_GetGamepadAxis(m_GameController, SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
		m_RightTrigger = SDL_GetGamepadAxis(m_GameController, SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

		for (auto &button : m_CurrentButtons)
		{
			auto sdlButton = GetSDLGamepadButtonFromNexusKeyCode(button.first);
			button.second  = (bool)SDL_GetGamepadButton(m_GameController, sdlButton);
		}
	}

	const Point2D<float> Gamepad::GetLeftStick() const
	{
		auto xAxis = m_LeftStick.X;
		auto yAxis = m_LeftStick.Y;

		if (xAxis < m_Deadzone && xAxis > -m_Deadzone)
		{
			xAxis = 0;
		}
		else
		{
			xAxis = xAxis / 32767.0f;
		}

		if (yAxis < m_Deadzone && yAxis > -m_Deadzone)
		{
			yAxis = 0;
		}
		else
		{
			yAxis = yAxis / 32767.0f;
		}

		return {(float)xAxis, (float)yAxis};
	}

	const Point2D<float> Gamepad::GetRightStick() const
	{
		auto xAxis = m_RightStick.X;
		auto yAxis = m_RightStick.Y;

		if (xAxis < m_Deadzone && xAxis > -m_Deadzone)
		{
			xAxis = 0;
		}
		else
		{
			xAxis = xAxis / 32767.0f;
		}

		if (yAxis < m_Deadzone && yAxis > -m_Deadzone)
		{
			yAxis = 0;
		}
		else
		{
			yAxis = yAxis / 32767.0f;
		}

		return {(float)xAxis, (float)yAxis};
	}

	const float Gamepad::GetLeftTrigger() const
	{
		return m_LeftTrigger / 32767.0f;
	}

	const float Gamepad::GetRightTrigger() const
	{
		return m_RightTrigger / 32767.0f;
	}

	const bool Gamepad::IsButtonHeld(GamepadButton button) const
	{
		return m_CurrentButtons.at(button);
	}

	const bool Gamepad::WasButtonPressed(GamepadButton button) const
	{
		auto current  = m_CurrentButtons.at(button);
		auto previous = m_PreviousButtons.at(button);
		return current && !previous;
	}

	const bool Gamepad::WasButtonReleased(GamepadButton button) const
	{
		auto current  = m_CurrentButtons.at(button);
		auto previous = m_PreviousButtons.at(button);
		return !current && previous;
	}

	const int Gamepad::GetDeadzone() const
	{
		return m_Deadzone;
	}

	void Gamepad::SetDeadzone(const int deadzone)
	{
		m_Deadzone = deadzone;
	}

	bool Gamepad::HasTouchpad()
	{
		return SDL_GetNumGamepadTouchpads(m_GameController) > 0;
	}

	void Gamepad::SetLED(uint8_t red, uint8_t green, uint8_t blue)
	{
		SDL_SetGamepadLED(m_GameController, red, green, blue);
	}

	void Gamepad::Rumble(uint16_t lowFrequency, uint16_t highFrequency, uint32_t milliseconds)
	{
		SDL_RumbleGamepad(m_GameController, lowFrequency, highFrequency, milliseconds);
	}

	void Gamepad::RumbleTriggers(uint16_t left, uint16_t right, uint32_t milliseconds)
	{
		SDL_RumbleGamepadTriggers(m_GameController, left, right, milliseconds);
	}

	const uint32_t Gamepad::GetControllerIndex()
	{
		return m_Index;
	}

	SDL_GamepadButton GetSDLGamepadButtonFromNexusKeyCode(GamepadButton button)
	{
		switch (button)
		{
			case GamepadButton::DpadUp: return SDL_GAMEPAD_BUTTON_DPAD_UP;
			case GamepadButton::DpadDown: return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
			case GamepadButton::DpadLeft: return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
			case GamepadButton::DpadRight: return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
			case GamepadButton::A: return SDL_GAMEPAD_BUTTON_SOUTH;
			case GamepadButton::B: return SDL_GAMEPAD_BUTTON_EAST;
			case GamepadButton::X: return SDL_GAMEPAD_BUTTON_WEST;
			case GamepadButton::Y: return SDL_GAMEPAD_BUTTON_NORTH;
			case GamepadButton::Back: return SDL_GAMEPAD_BUTTON_BACK;
			case GamepadButton::Guide: return SDL_GAMEPAD_BUTTON_GUIDE;
			case GamepadButton::Start: return SDL_GAMEPAD_BUTTON_START;
			case GamepadButton::LeftStick: return SDL_GAMEPAD_BUTTON_LEFT_STICK;
			case GamepadButton::RightStick: return SDL_GAMEPAD_BUTTON_RIGHT_STICK;
			case GamepadButton::LeftShoulder: return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
			case GamepadButton::RightShoulder: return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
			case GamepadButton::Misc: return SDL_GAMEPAD_BUTTON_MISC1;
			case GamepadButton::LeftPaddle1: return SDL_GAMEPAD_BUTTON_LEFT_PADDLE1;
			case GamepadButton::LeftPaddle2: return SDL_GAMEPAD_BUTTON_LEFT_PADDLE2;
			case GamepadButton::RightPaddle1: return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1;
			case GamepadButton::RightPaddle2: return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2;
			case GamepadButton::Touchpad: return SDL_GAMEPAD_BUTTON_TOUCHPAD;
			default: return SDL_GAMEPAD_BUTTON_INVALID;
		}
	}
}	 // namespace Nexus