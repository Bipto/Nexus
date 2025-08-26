#include "Nexus-Core/Runtime.hpp"

#include "Nexus-Core/Platform.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/ECS/Components.hpp"

Nexus::Application *appPtr = nullptr;

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
		appPtr->Load();

#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(main_loop, 0, 1);
#else
		while (appPtr->IsRunning()) { appPtr->MainLoop(); }
#endif

		app->Unload();
	}

	Application *GetApplication()
	{
		return appPtr;
	}

	void SetApplication(Nexus::Application *app)
	{
		appPtr = app;
	}
}	 // namespace Nexus

//-----------------------------------------------------------------------------
// CORE ENGINE FUNCTIONS
//-----------------------------------------------------------------------------

namespace Nexus
{
	void Init(int argc, char **argv)
	{
		ZoneScopedN("Initialisation");
		Platform::Initialise();
	}

	void Shutdown()
	{
		ZoneScopedN("Shutdown");
		Platform::Shutdown();
	}
}	 // namespace Nexus