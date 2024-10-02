#include "Runtime.hpp"

#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Platform.hpp"
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
		Platform::Initialise();
		InputNew::InputContext::Initialise();
	}

	void Shutdown()
	{
		Platform::Shutdown();
	}
}	 // namespace Nexus