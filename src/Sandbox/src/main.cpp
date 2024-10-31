#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/CatmullRom.hpp"
#include "Nexus-Core/Graphics/RoundedRectangle.hpp"
#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/UI/Button.hpp"
#include "Nexus-Core/UI/Canvas.hpp"
#include "Nexus-Core/UI/Label.hpp"
#include "Nexus-Core/UI/Panel.hpp"
#include "Nexus-Core/UI/PictureBox.hpp"
#include "Nexus-Core/Utils/Utils.hpp"
#include "Nexus.hpp"

#include "Nexus-Core/Platform.hpp"

class Sandbox : public Nexus::Application
{
  public:
	explicit Sandbox(const Nexus::ApplicationSpecification &spec) : Nexus::Application(spec)
	{
	}

	virtual void Load() override
	{
	}

	virtual void Update(Nexus::TimeSpan time) override
	{
	}

	virtual void Render(Nexus::TimeSpan time) override
	{
		m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();

		m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->SwapBuffers();
	}

	virtual void OnResize(Nexus::Point2D<uint32_t> size) override
	{
	}

	void CreatePipeline()
	{
	}

	virtual void Unload() override
	{
	}
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
	Nexus::ApplicationSpecification spec;
	spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::Vulkan;
	spec.AudioAPI	 = Nexus::Audio::AudioAPI::OpenAL;

	spec.WindowProperties.Width		= 1280;
	spec.WindowProperties.Height	= 720;
	spec.WindowProperties.Resizable = true;
	spec.WindowProperties.Title		= "Sandbox";

	spec.SwapchainSpecification.Samples = Nexus::Graphics::SampleCount::SampleCount8;

	return new Sandbox(spec);
}