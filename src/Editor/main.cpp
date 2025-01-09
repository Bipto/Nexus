#include "Nexus.hpp"

#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/Renderer/Renderer3D.hpp"

#include "Nexus-Core/FileSystem/FileDialogs.hpp"
#include "Nexus-Core/Graphics/HdriProcessor.hpp"

class EditorApplication : public Nexus::Application
{
  public:
	EditorApplication(const Nexus::ApplicationSpecification &spec) : Application(spec)
	{
	}

	virtual ~EditorApplication()
	{
	}

	virtual void Load() override
	{
		m_Renderer = std::make_unique<Nexus::Graphics::Renderer3D>(m_GraphicsDevice);

		m_CommandList = m_GraphicsDevice->CreateCommandList();

		m_ImGuiRenderer = std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer>(new Nexus::ImGuiUtils::ImGuiGraphicsRenderer(this));

		auto &io = ImGui::GetIO();
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ApplyDarkTheme();

		Nexus::Graphics::FramebufferSpecification framebufferSpec = {};
		framebufferSpec.Width									  = 1280;
		framebufferSpec.Height									  = 720;
		framebufferSpec.DepthAttachmentSpecification			  = {Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt};
		framebufferSpec.ColorAttachmentSpecification.Attachments  = {{Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat()}};

		m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(framebufferSpec);

		m_FramebufferTextureID = m_ImGuiRenderer->BindTexture(m_Framebuffer->GetColorTexture(0));
	}

	virtual void Update(Nexus::TimeSpan time) override
	{
	}

	void RenderMainMenuBar()
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File", true))
		{
			if (ImGui::MenuItem("New")) {}

			if (ImGui::MenuItem("Open Cubemap"))
			{
				std::vector<const char *> filters  = {"*.hdr"};
				const char				 *filepath = Nexus::FileDialogs::OpenFile(filters);

				if (filepath)
				{
					Nexus::Graphics::HdriProcessor processor(filepath, m_GraphicsDevice);
					m_Cubemap = processor.Generate(2048);
				}
			}

			if (ImGui::MenuItem("Quit"))
			{
				this->Close();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void ApplyDarkTheme()
	{
		auto &colors			  = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = {0.05f, 0.055f, 0.06f, 1.0f};

		colors[ImGuiCol_Header]		   = {0.15f, 0.15f, 0.16f, 1.0f};
		colors[ImGuiCol_HeaderHovered] = {0.3f, 0.3f, 0.31f, 1.0f};
		colors[ImGuiCol_HeaderActive]  = {0.3f, 0.3f, 0.31f, 1.0f};

		colors[ImGuiCol_Button]		   = {0.15f, 0.15f, 0.15f, 1.0f};
		colors[ImGuiCol_ButtonHovered] = {0.25f, 0.25f, 0.25f, 1.0f};
		colors[ImGuiCol_ButtonActive]  = {0.1f, 0.1f, 0.1f, 1.0f};

		colors[ImGuiCol_FrameBg]		= {0.3f, 0.3f, 0.31f, 1.0f};
		colors[ImGuiCol_FrameBgHovered] = {0.4f, 0.4f, 0.41f, 1.0f};
		colors[ImGuiCol_FrameBgActive]	= {0.3f, 0.3f, 0.31f, 1.0f};

		colors[ImGuiCol_Tab]				= {0.15f, 0.15f, 0.15f, 1.0f};
		colors[ImGuiCol_TabHovered]			= {0.3f, 0.3f, 0.31f, 1.0f};
		colors[ImGuiCol_TabActive]			= {0.15f, 0.15f, 0.16f, 1.0f};
		colors[ImGuiCol_TabUnfocused]		= {0.15f, 0.15f, 0.16f, 1.0f};
		colors[ImGuiCol_TabUnfocusedActive] = {0.15f, 0.15f, 0.16f, 1.0f};

		colors[ImGuiCol_MenuBarBg]		  = {0.08f, 0.085f, 0.09f, 1.0f};
		colors[ImGuiCol_TitleBg]		  = {0.05f, 0.055f, 0.05f, 1.0f};
		colors[ImGuiCol_TitleBgActive]	  = {0.08f, 0.085f, 0.09f, 1.0f};
		colors[ImGuiCol_TitleBgCollapsed] = {0.05f, 0.055f, 0.06f, 1.0f};

		colors[ImGuiCol_CheckMark]		  = {0.55f, 0.55f, 0.55f, 1.0f};
		colors[ImGuiCol_Text]			  = {0.85f, 0.85f, 0.85f, 1.0f};
		colors[ImGuiCol_SliderGrab]		  = {0.55f, 0.55f, 0.55f, 1.0f};
		colors[ImGuiCol_SliderGrabActive] = {0.55f, 0.55f, 0.55f, 1.0f};
	}

	void RenderDockspace()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar;

		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});

		flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
				 ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_MenuBar;

		ImGui::Begin("Dockspace", nullptr, flags);

		ImGui::PopStyleVar(3);
		ImGui::DockSpace(ImGui::GetID("Dockspace"));

		RenderMainMenuBar();

		if (ImGui::Begin("Viewport"))
		{
			ImVec2 size = ImGui::GetWindowSize();
			ImGui::Image(m_FramebufferTextureID, size);
		}
		ImGui::End();

		ImGui::ShowDemoWindow();

		ImGui::End();
	}

	virtual void Render(Nexus::TimeSpan time) override
	{
		Nexus::GetApplication()->GetPrimarySwapchain()->Prepare();

		m_CommandList->Begin();
		m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget(Nexus::GetApplication()->GetPrimarySwapchain()));
		m_CommandList->ClearColorTarget(0, {1.0f, 0.0f, 0.0f, 1.0f});
		m_CommandList->End();
		m_GraphicsDevice->SubmitCommandList(m_CommandList);

		Nexus::Graphics::Scene		  scene {.Environment = nullptr, .EnvironmentSampler = nullptr, .EnvironmentColour = {1.0f, 0.0f, 0.0f, 1.0f}};
		Nexus::Graphics::RenderTarget target(m_Framebuffer);

		m_Renderer->Begin(scene, target, m_Cubemap, time);
		m_Renderer->End();

		m_ImGuiRenderer->BeforeLayout(time);
		RenderDockspace();
		m_ImGuiRenderer->AfterLayout();

		Nexus::GetApplication()->GetPrimarySwapchain()->SwapBuffers();
	}

	virtual void Unload() override
	{
	}

  private:
	std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer = nullptr;
	Nexus::Ref<Nexus::Graphics::CommandList>				  m_CommandList	  = nullptr;
	Nexus::Ref<Nexus::Graphics::Framebuffer>				  m_Framebuffer	  = nullptr;

	ImTextureID m_FramebufferTextureID = {};

	std::unique_ptr<Nexus::Graphics::Renderer3D> m_Renderer;
	Nexus::Ref<Nexus::Graphics::Cubemap>		 m_Cubemap = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
	Nexus::ApplicationSpecification spec;
	spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
	spec.AudioAPI	 = Nexus::Audio::AudioAPI::OpenAL;

	spec.WindowProperties.Width			   = 1280;
	spec.WindowProperties.Height		   = 720;
	spec.WindowProperties.Title			   = "Editor";
	spec.WindowProperties.Resizable		   = true;
	spec.WindowProperties.RendersPerSecond = 60;
	spec.WindowProperties.UpdatesPerSecond = 60;

	spec.SwapchainSpecification.Samples	   = Nexus::Graphics::SampleCount::SampleCount8;
	spec.SwapchainSpecification.VSyncState = Nexus::Graphics::VSyncState::Disabled;

	return new EditorApplication(spec);
}