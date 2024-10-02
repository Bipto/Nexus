#include "Nexus-Core/Platform.hpp"

#include "SDL3Include.hpp"

namespace Nexus::Platform
{
	std::vector<InputNew::Keyboard> GetKeyboards()
	{
		std::vector<InputNew::Keyboard> keyboards;

		int				count;
		SDL_KeyboardID *sdlKeyboards = SDL_GetKeyboards(&count);

		for (int i = 0; i < count; i++)
		{
			uint32_t id = sdlKeyboards[i];

			if (const char *instanceName = SDL_GetKeyboardInstanceName(id))
			{
				std::string name = instanceName;
				keyboards.emplace_back(sdlKeyboards[i], name);
			}
			else
			{
				NX_ERROR(SDL_GetError());
			}
		}

		SDL_free(sdlKeyboards);

		return keyboards;
	}

	std::vector<InputNew::Mouse> GetMice()
	{
		std::vector<InputNew::Mouse> mice;

		int			 count;
		SDL_MouseID *sdlMice = SDL_GetMice(&count);

		for (int i = 0; i < count; i++)
		{
			uint32_t id = sdlMice[i];

			if (const char *instanceName = SDL_GetMouseInstanceName(id))
			{
				std::string name = instanceName;
				mice.emplace_back(sdlMice[i], name);
			}
			else
			{
				NX_ERROR(SDL_GetError());
			}
		}

		SDL_free(sdlMice);

		return mice;
	}

	std::vector<InputNew::Gamepad> GetGamepads()
	{
		std::vector<InputNew::Gamepad> gamepads;

		int				count;
		SDL_JoystickID *sdlGamepads = SDL_GetGamepads(&count);

		for (int i = 0; i < count; i++)
		{
			if (SDL_IsGamepad(sdlGamepads[i]))
			{
				uint32_t id = sdlGamepads[i];

				if (const char *instanceName = SDL_GetGamepadInstanceName(id))
				{
					std::string name = instanceName;
					gamepads.emplace_back(sdlGamepads[i], name);
				}
			}
		}

		SDL_free(sdlGamepads);

		return gamepads;
	}

	std::vector<Monitor> GetMonitors()
	{
		std::vector<Monitor> monitors;

		int			   displayCount;
		SDL_DisplayID *displays = SDL_GetDisplays(&displayCount);

		for (int i = 0; i < displayCount; i++)
		{
			SDL_DisplayID id = displays[i];

			// Monitor monitor;
			float		dpi	 = SDL_GetDisplayContentScale(id);
			std::string name = SDL_GetDisplayName(id);

			SDL_Rect displayBounds;
			SDL_GetDisplayBounds(id, &displayBounds);

			SDL_Rect usableBounds;
			SDL_GetDisplayUsableBounds(id, &usableBounds);

			const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(id);

			Monitor monitor {.Position	   = {displayBounds.x, displayBounds.y},
							 .Size		   = {displayBounds.w, displayBounds.h},
							 .WorkPosition = {usableBounds.x, usableBounds.y},
							 .WorkSize	   = {usableBounds.w, usableBounds.h},
							 .DPI		   = dpi,
							 .RefreshRate  = displayMode->refresh_rate,
							 .Name		   = name};

			monitors.push_back(monitor);
		}

		SDL_free(displays);

		return monitors;
	}

	void Initialise()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD) != 0)
		{
			NX_LOG("Could not initialize SDL");
		}

		SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
		SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
		SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_PLAYER_LED, "1");
	}

	void Shutdown()
	{
		SDL_Quit();
	}
}	 // namespace Nexus::Platform