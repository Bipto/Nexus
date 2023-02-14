#pragma once

#include "nxpch.h"
#include "SDL.h"

/* extern "C" 
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
} */

#include <Python.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Application.h"
#include "Logging/Log.h"
#include "Input/Input.h"

#include "assimp/Importer.hpp"

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace Nexus
{
    static void Init()
    {
        Py_Initialize();
        Assimp::Importer importer{};
    }

    static void Shutdown()
    {
        SDL_Quit();
        Py_Finalize();
    }
}

//-----------------------------------------------------------------------------
// APPLICATION RUNTIME
//-----------------------------------------------------------------------------

Nexus::Application* appPtr = nullptr;
Nexus::Logger* logger = new Nexus::Logger();
Nexus::Input* input = new Nexus::Input();

void main_loop()
{
    appPtr->MainLoop();
}

namespace Nexus
{
    void Run(Nexus::Application* app)
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

    Application* GetApplication()
    {
        return appPtr;
    }

    Logger* GetCoreLogger()
    {
        return logger;
    }

    Input* GetCoreInput()
    {
        return input;
    }
}
