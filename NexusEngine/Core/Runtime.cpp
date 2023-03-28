#include "Runtime.h"

Nexus::Application* appPtr = nullptr;
Nexus::Logger* logger = new Nexus::Logger();

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

    void Init()
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