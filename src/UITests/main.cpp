#include "Nexus-Core/EntryPoint.hpp"
#include "Nexus-Core/UI/UIManager.hpp"

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
	Nexus::ApplicationDescription spec;
	spec.GraphicsCreateInfo.API = Nexus::Graphics::GraphicsAPI::OpenGL;
	spec.AudioAPI				= Nexus::Audio::AudioAPI::OpenAL;

	spec.WindowProperties.Width			   = 1280;
	spec.WindowProperties.Height		   = 720;
	spec.WindowProperties.Title			   = "UITests";
	spec.WindowProperties.Flags			   = WindowFlags_Resizable | WindowFlags_HighPixelDensity;
	spec.WindowProperties.RendersPerSecond = {};
	spec.WindowProperties.UpdatesPerSecond = {};

	spec.SwapchainDescription.Samples		   = 1;
	spec.SwapchainDescription.ImagePresentMode = Nexus::Graphics::PresentMode::Fifo;
	spec.EventDriven						   = false;

	return new Nexus::UI::UIManager(spec);
}