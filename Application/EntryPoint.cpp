#include <iostream>

#include "Core/Core.h"
#include "Core/Application.h"
#include "Core/Graphics/GraphicsDevice.h"

class Editor : public NexusEngine::Application
{
    public:
        Editor() : Application(NexusEngine::GraphicsAPI::OpenGL){}

        void Load() override
        {
            std::cout << "Loading" << std::endl;
        }

        void Update()
        {

        }

        void Unload()
        {
            std::cout << "Unloading" << std::endl;
        }
};

int main(int, char**)
{
    NexusEngine::Init();

    Editor* app = new Editor();
    NexusEngine::Run(app);
    delete app;

    NexusEngine::Shutdown();
    return 0;
}