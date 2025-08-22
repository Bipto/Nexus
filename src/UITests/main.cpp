#include "Nexus-Core/EntryPoint.hpp"
#include "Nexus-Core/UI/UIManager.hpp"

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
	Nexus::ApplicationSpecification spec;
	spec.GraphicsCreateInfo.API = Nexus::Graphics::GraphicsAPI::OpenGL;
	spec.AudioAPI	 = Nexus::Audio::AudioAPI::OpenAL;

	spec.WindowProperties.Width			   = 1280;
	spec.WindowProperties.Height		   = 720;
	spec.WindowProperties.Title			   = "UITests";
	spec.WindowProperties.Resizable		   = true;
	spec.WindowProperties.RendersPerSecond = {};
	spec.WindowProperties.UpdatesPerSecond = {};
	spec.WindowProperties.Shown			   = true;

	spec.SwapchainSpecification.Samples	   = 1;
	spec.SwapchainSpecification.VSyncState = Nexus::Graphics::VSyncState::Disabled;
	spec.EventDriven					   = false;

	return new Nexus::UI::UIManager(spec);
}