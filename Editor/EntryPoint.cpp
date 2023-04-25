#include "Editor.h"

int main(int argc, char** argv)
{
    Nexus::GraphicsAPI api = Nexus::GraphicsAPI::OpenGL;
    std::vector<std::string> arguments;
    for (int i = 0; i < argc; i++)
    {
        std::string argument = std::string(argv[i]);
        arguments.push_back(argument);
    }
    
    if (arguments.size() > 1)
        if (arguments[1] == std::string("DX"))    
            api = Nexus::GraphicsAPI::DirectX11;

    Nexus::Init();

    Editor* app = new Editor(api);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
} 