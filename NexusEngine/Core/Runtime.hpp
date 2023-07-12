#pragma once

#include "nxpch.hpp"
#include "SDL.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

// #include <Python.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Application.hpp"
#include "Logging/Log.hpp"
#include "Input/InputState.hpp"

#include "Runtime/ECS/ComponentRegistry.hpp"
#include "Core/AssetManager.hpp"

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace Nexus
{
    /// @brief A method that initializes the engine
    /// @param argc A pointer to a string containing the input arguments
    /// @param argv An integer containing the number of arguments
    void Init(int argc = 0, char **argv = nullptr);

    /// @brief A method that shuts down the engine
    void Shutdown();
}

//-----------------------------------------------------------------------------
// APPLICATION RUNTIME
//-----------------------------------------------------------------------------

/// @brief A method that runs the main loop and is used on emscripten platforms
void main_loop();

namespace Nexus
{
    /// @brief A method that runs an application
    /// @param app A pointer to an application to run
    void Run(Nexus::Application *app);

    /// @brief A method that returns the currently active application
    /// @return A pointer to the application
    Application *GetApplication();

    /// @brief A method that returns the currently active asset manager
    /// @return A pointer to the asset manager
    AssetManager *GetAssetManager();

    /// @brief A method that returns a reference to the component registry
    /// @return A reference to a component registry
    ComponentRegistry &GetComponentRegistry();
}
