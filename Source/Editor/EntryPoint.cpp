#include "Editor.hpp"

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;
    spec.ImGuiActive = true;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

    return new Editor(spec);
}