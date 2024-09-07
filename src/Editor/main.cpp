#include "NexusEngine.hpp"

#include "Editor.hpp"

Nexus::Application *
Nexus::CreateApplication(const CommandLineArguments &arguments) {
  Nexus::ApplicationSpecification spec;
  spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
  spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;

  spec.WindowProperties.Width = 1280;
  spec.WindowProperties.Height = 720;
  spec.WindowProperties.Resizable = true;
  spec.WindowProperties.Title = "Editor";

  spec.SwapchainSpecification.Samples =
      Nexus::Graphics::SampleCount::SampleCount8;

  return new EditorApplication(spec);
}