#pragma once

#include "nxpch.h"
#include "SDL.h"

extern "C" 
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

//#include <Python.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Application.h"
#include "Logging/Log.h"
#include "Input/Input.h"

#include "Runtime/ECS/ComponentRegistry.h"

#include "assimp/Importer.hpp"

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace Nexus
{
    void Init();
    void Shutdown();
}

//-----------------------------------------------------------------------------
// APPLICATION RUNTIME
//-----------------------------------------------------------------------------

void main_loop();

namespace Nexus
{
    void Run(Nexus::Application* app);
    Application* GetApplication();
    ComponentRegistry& GetComponentRegistry();
}
