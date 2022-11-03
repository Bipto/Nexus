#include <iostream>

#include "Core/Core.h"
#include "Core/Application.h"


int main(int, char**)
{
    NexusEngine::Init();

    NexusEngine::Application* app = new NexusEngine::Application();
    app->Run();
    delete app;

    NexusEngine::Shutdown();
    return 0;
}