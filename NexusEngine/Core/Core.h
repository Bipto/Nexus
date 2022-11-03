#pragma once

#include <iostream>
#include "SDL.h"

extern "C" 
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Application.h"

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace NexusEngine
{
    static void Init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cout << "Could not initialize SDL\n";
        }
    }

    static void Shutdown()
    {
        SDL_Quit();
    }
}

//-----------------------------------------------------------------------------
// APPLICATION RUNTIME
//-----------------------------------------------------------------------------

NexusEngine::Application* appPtr = nullptr;

void main_loop()
{
    appPtr->MainLoop();
}

namespace NexusEngine
{
    void Run(NexusEngine::Application* app)
    {
        appPtr = app;
        appPtr->Load();

        #ifdef __EMSCRIPTEN__
            emscripten_set_main_loop(main_loop, 0, 1);
        #else
            while (!appPtr->ShouldClose())
                appPtr->MainLoop();
        #endif

        app->Unload();
    }
}
    