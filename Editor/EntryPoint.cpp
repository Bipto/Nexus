#include "Editor.h"

int main(int argc, char** argv)
{
    Nexus::Init();

    Editor* app = new Editor(Nexus::GraphicsAPI::OpenGL);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
} 