#include "Editor.h"

int main(int argc, char** argv)
{
    Nexus::Init();

    Editor* app = new Editor(Nexus::GraphicsAPI::DirectX11);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
} 