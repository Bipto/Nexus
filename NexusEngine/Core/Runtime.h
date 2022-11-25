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

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace NexusEngine
{
    static void Init()
    {
        gladLoadGL();

        // Decide GL+GLSL versions
        #ifdef __EMSCRIPTEN__
            // GL ES 2.0 + GLSL 100
            const char* glsl_version = "#version 100";
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #elif defined(__APPLE__)
            // GL 3.2 Core + GLSL 150
            const char* glsl_version = "#version 150";
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #else
            // GL 3.0 + GLSL 130
            const char* glsl_version = "#version 130";
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #endif

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cout << "Could not initialize SDL\n";
        }

        Py_Initialize();
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

NexusEngine::Application* appPtr = nullptr;
NexusEngine::Logger* logger = new NexusEngine::Logger();

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

    Application* GetApplication()
    {
        return appPtr;
    }

    Logger* GetCoreLogger()
    {
        return logger;
    }
}
