#include "Runtime.h"

#if defined(NX_PLATFORM_SDL_NET)
#include "SDL_net.h"
#endif

#include <iostream>
#include <chrono>
#include <vector>
#include <inttypes.h>

Nexus::Application *appPtr = nullptr;
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
        appPtr = app;
        assetManager = new AssetManager(app->GetGraphicsDevice());
        appPtr->Load();

#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(main_loop, 0, 1);
#else
        while (!appPtr->ShouldClose())
            appPtr->MainLoop();
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

    ComponentRegistry registry;
    ComponentRegistry &GetComponentRegistry()
    {
        return registry;
    }
}

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace Nexus
{
    void BindComponents()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
        {
            NX_LOG("Could not initialize SDL");
        }

#if defined(NX_PLATFORM_SDL_NET)
        if (SDLNet_Init() != 0)
        {
            NX_LOG("Could not initialize SDL_net");
        }
#endif

        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_PLAYER_LED, "1");

        registry.Bind(new TransformComponent());
        registry.Bind(new SpriteRendererComponent());
    }

    void Init(int argc, char **argv)
    {
        // Py_Initialize();
        BindComponents();
    }

    void Shutdown()
    {
#if defined(NX_PLATFORM_SDL_NET)
        SDLNet_Quit();
#endif

        SDL_Quit();
        // Py_Finalize();
    }
}