#include "Runtime.hpp"

#include "Nexus-Core/nxpch.hpp"

Nexus::Application	*appPtr		  = nullptr;
Nexus::AssetManager *assetManager = nullptr;

//-----------------------------------------------------------------------------
// APPLICATION RUNTIME
//-----------------------------------------------------------------------------

void main_loop()
{
	appPtr->MainLoop();
}

namespace Nexus
{
	void Run(Nexus::Application *app)
	{
		appPtr		 = app;
		assetManager = new AssetManager(app->GetGraphicsDevice());
		appPtr->Load();

#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(main_loop, 0, 1);
#else
		while (!appPtr->ShouldClose()) appPtr->MainLoop();
#endif

		app->Unload();
	}

	Application *GetApplication()
	{
		return appPtr;
	}

	AssetManager *GetAssetManager()
	{
		return assetManager;
	}
}	 // namespace Nexus

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace Nexus
{
	void Init(int argc, char **argv)
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
}	 // namespace Nexus