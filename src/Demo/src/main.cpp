#include "DemoApplication.hpp"
#include "Nexus-Core/EntryPoint.hpp"

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
	Nexus::ApplicationDescription desc;

	desc.GraphicsCreateInfo.API	  = Nexus::Graphics::GraphicsAPI::OpenGL;
	desc.GraphicsCreateInfo.Debug = true;
	desc.AudioAPI				  = Nexus::Audio::AudioAPI::OpenAL;

	desc.WindowProperties.Width	 = 1280;
	desc.WindowProperties.Height = 720;
	desc.WindowProperties.Title	 = "Demo";

	desc.WindowProperties.Flags			   = WindowFlags_Resizable | WindowFlags_HighPixelDensity;
	desc.WindowProperties.RendersPerSecond = {};
	desc.WindowProperties.UpdatesPerSecond = {};

	desc.SwapchainDescription.Samples		   = 8;
	desc.SwapchainDescription.ImagePresentMode = Nexus::Graphics::PresentMode::Immediate;

	desc.Organization = "Nexus";
	desc.App		  = "Demo";

	return new DemoApplication(desc);
}