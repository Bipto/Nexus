#include "Editor.hpp"

int main(int argc, char **argv)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
    spec.ImGuiActive = true;
    spec.UpdatesPerSecond = 60;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;
    std::vector<std::string> arguments;
    for (int i = 0; i < argc; i++)
    {
        std::string argument = std::string(argv[i]);
        arguments.push_back(argument);
    }

    if (arguments.size() > 1)
        if (arguments[1] == std::string("DX"))
            spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::DirectX11;

    Nexus::Init();

    Editor *app = new Editor(spec);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
}