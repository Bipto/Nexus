#pragma once

#include "Nexus-Core/nxpch.hpp"

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include "Application.hpp"
#include "Input/InputState.hpp"
#include "Logging/Log.hpp"
#include "Nexus-Core/AssetManager.hpp"

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace Nexus
{
	/// @brief A method that initializes the engine
	/// @param argc A pointer to a string containing the input arguments
	/// @param argv An integer containing the number of arguments
	NX_API void Init(int argc = 0, char **argv = nullptr);

	/// @brief A method that shuts down the engine
	NX_API void Shutdown();
}	 // namespace Nexus

//-----------------------------------------------------------------------------
// APPLICATION RUNTIME
//-----------------------------------------------------------------------------

/// @brief A method that runs the main loop and is used on emscripten platforms
void main_loop();

namespace Nexus
{
	/// @brief A method that runs an application
	/// @param app A pointer to an application to run
	NX_API void Run(Nexus::Application *app);

	/// @brief A method that returns the currently active application
	/// @return A pointer to the application
	NX_API Application *GetApplication();

	/// @brief A method that returns the currently active asset manager
	/// @return A pointer to the asset manager
	NX_API AssetManager *GetAssetManager();

	NX_API void SetApplication(Nexus::Application *app);
}	 // namespace Nexus
