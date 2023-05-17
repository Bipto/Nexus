#include "Runtime.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <inttypes.h>

Nexus::Application* appPtr = nullptr;
Nexus::AssetManager* assetManager = nullptr;

//-----------------------------------------------------------------------------
// APPLICATION RUNTIME
//-----------------------------------------------------------------------------

void main_loop()
{
    appPtr->MainLoop();
}

namespace Nexus
{
    void Run(Nexus::Application* app)
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

    Application* GetApplication()
    {
        return appPtr;
    }

    AssetManager *GetAssetManager()
    {
        return assetManager;
    }

    ComponentRegistry registry;
    ComponentRegistry& GetComponentRegistry()
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
        registry.Bind(new TransformComponent());
        registry.Bind(new SpriteRendererComponent());
    }

    void Init(int argc, char** argv)
    {
        //Py_Initialize();
        Assimp::Importer importer{};
        BindComponents();
    }

    void Shutdown()
    {
        SDL_Quit();
        //Py_Finalize();
    }
}