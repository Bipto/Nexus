#include "Editor.hpp"

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;

    if (arguments.size() > 1)
    {
        if (arguments[1] == "DX")
        {
            spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::D3D11;
        }
    }

    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;
    spec.ImGuiActive = true;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

    return new Editor(spec);
}