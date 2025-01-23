#include "Nexus.hpp"

#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/Renderer/Renderer3D.hpp"

#include "Nexus-Core/FileSystem/FileDialogs.hpp"
#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/HdriProcessor.hpp"

#include "EditorProperties.hpp"
#include "InspectorPanel.hpp"
#include "ProjectViewPanel.hpp"
#include "SceneViewPanel.hpp"

#include "Nexus-Core/ECS/ComponentRegistry.hpp"
#include "Nexus-Core/ECS/Registry.hpp"

#include "Nexus-Core/Utils/ScriptProjectGenerator.hpp"

enum class SceneState
{
	Playing,
	Paused,
	Stopped
};

class EditorApplication : public Nexus::Application
{
  public:
	EditorApplication(const Nexus::ApplicationSpecification &spec) : Application(spec)
	{
	}

	virtual ~EditorApplication()
	{
		SaveLayoutSettings();
	}

	virtual void Load() override
	{
		m_Renderer = std::make_unique<Nexus::Graphics::Renderer3D>(m_GraphicsDevice);

		m_ImGuiRenderer = std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer>(new Nexus::ImGuiUtils::ImGuiGraphicsRenderer(this));

		auto &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("fonts/roboto/roboto-black.ttf", 24);
		m_ImGuiRenderer->RebuildFontAtlas();

		CreateFramebuffer(ImVec2(1280, 720));
		ApplyDarkTheme();

		SceneViewPanel *sceneViewPanel = new SceneViewPanel();
		InspectorPanel *inspectorPanel = new InspectorPanel();

		sceneViewPanel->OnEntitySelected.Bind(
			[this](std::optional<Nexus::GUID> id)
			{
				for (Panel *panel : m_Panels) { panel->OnEntitySelected(id); }
			});

		m_Panels.push_back(new ProjectViewPanel());
		m_Panels.push_back(sceneViewPanel);
		m_Panels.push_back(inspectorPanel);

		std::string title = "Editor: (" + m_GraphicsDevice->GetAPIName() + std::string(")");
		Nexus::GetApplication()->GetPrimaryWindow()->SetTitle(title);

		m_EditorPropertiesPanel = new EditorPropertiesPanel(&m_Panels);
		m_Panels.push_back(m_EditorPropertiesPanel);
		LoadLayoutSettings();
	}

	virtual void Update(Nexus::TimeSpan time) override
	{
	}

	void CreateFramebuffer(ImVec2 size)
	{
		Nexus::Graphics::FramebufferSpecification framebufferSpec = {};
		framebufferSpec.Width									  = (uint32_t)size.x;
		framebufferSpec.Height									  = (uint32_t)size.y;
		framebufferSpec.DepthAttachmentSpecification			  = {Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt};
		framebufferSpec.ColorAttachmentSpecification.Attachments  = {{Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm}};

		m_Framebuffer		   = m_GraphicsDevice->CreateFramebuffer(framebufferSpec);
		m_FramebufferTextureID = m_ImGuiRenderer->BindTexture(m_Framebuffer->GetColorTexture(0));
	}

	void RenderNewProjectWindow()
	{
		if (!m_NewProjectWindowOpen)
		{
			return;
		}

		if (ImGui::Begin("New Project", &m_NewProjectWindowOpen))
		{
			static std::string s_ProjectName;
			static std::string s_ProjectDirectory;

			ImGui::InputText("Name", &s_ProjectName);
			ImGui::Text("Directory");
			ImGui::SameLine();
			ImGui::Text(s_ProjectDirectory.c_str());
			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				const char *directory = Nexus::FileDialogs::OpenFolder("Select a directory", "");
				if (directory)
				{
					s_ProjectDirectory = directory;
				}
			}

			if (ImGui::Button("Create"))
			{
				m_Project = Nexus::CreateRef<Nexus::Project>(s_ProjectName, s_ProjectDirectory, true);
				m_Project->Serialize();

				Nexus::Utils::ScriptProjectGenerator generator = {};
				generator.Generate("project_template", m_Project->GetName(), s_ProjectDirectory);

				LoadProject(m_Project);
			}
		}

		ImGui::End();
	}

	void LoadProject(Nexus::Ref<Nexus::Project> project)
	{
		m_Project = project;

		for (auto panel : m_Panels) { panel->LoadProject(m_Project); }
	}

	void RenderMainMenuBar()
	{
		ImGui::BeginMenuBar();

		if (ImGui::BeginMenu("File", true))
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("New Project"))
				{
					m_NewProjectWindowOpen = true;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Open"))
				{
					std::vector<const char *> filters  = {"*.proj"};
					const char				 *filepath = Nexus::FileDialogs::OpenFile(filters);
					if (filepath)
					{
						m_Project = Nexus::Project::Deserialize(filepath);
						if (m_Project->GetNumberOfScenes() > 0)
						{
							m_Project->LoadScene(0);
						}
						LoadProject(m_Project);
					}
				}

				bool selected = false;
				if (ImGui::MenuItem("Save Project", "", &selected, m_Project != nullptr))
				{
					if (m_Project)
					{
						m_Project->Serialize();
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Open Model"))
			{
				std::vector<const char *> filters  = {"*.*"};
				const char				 *filepath = Nexus::FileDialogs::OpenFile(filters);

				if (filepath)
				{
					Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
					m_Model = factory.CreateFrom3DModelFile(filepath);
				}
			}

			if (ImGui::MenuItem("Quit"))
			{
				this->Close();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Editor", true))
		{
			bool editorPropertiesOpen = m_EditorPropertiesPanel->IsOpen();
			if (ImGui::Checkbox(m_EditorPropertiesPanel->GetName().c_str(), &editorPropertiesOpen))
			{
				if (editorPropertiesOpen)
				{
					m_EditorPropertiesPanel->Open();
				}
				else
				{
					m_EditorPropertiesPanel->Close();
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
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

	void ResizeFramebuffer(ImVec2 size)
	{
		m_Framebuffer->Resize((uint32_t)size.x, (uint32_t)size.y);
		m_FramebufferTextureID = m_ImGuiRenderer->BindTexture(m_Framebuffer->GetColorTexture(0));
		m_PreviousViewportSize = size;
	}

	void SaveLayoutSettings()
	{
		YAML::Node node;

		for (Panel *panel : m_Panels) { node[panel->GetName()] = (int)panel->IsOpen(); }

		YAML::Emitter out;
		out << node;
		Nexus::FileSystem::WriteFile("editor.yaml", out.c_str());
	}

	void LoadLayoutSettings()
	{
		std::string editorYamlFilePath = "editor.yaml";

		if (std::filesystem::exists(editorYamlFilePath))
		{
			std::string text = Nexus::FileSystem::ReadFileToString("editor.yaml");
			YAML::Node	root = YAML::Load(text);

			for (Panel *panel : m_Panels)
			{
				const std::string &panelName = panel->GetName();
				if (root[panelName])
				{
					bool open = (bool)root[panelName].as<int>();
					if (open)
					{
						panel->Open();
					}
					else
					{
						panel->Close();
					}
				}
			}
		}
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));

		if (ImGui::Begin("Viewport"))
		{
			if (ImGui::Button("Play"))
			{
				m_SceneState = SceneState::Playing;
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
			{
				m_SceneState = SceneState::Paused;
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				m_SceneState = SceneState::Stopped;
			}

			ImVec2 size = ImGui::GetContentRegionAvail();
			ImVec2 uv0	= {0, 0};
			ImVec2 uv1	= {1, 1};

			if (m_GraphicsDevice->GetGraphicsAPI() == Nexus::Graphics::GraphicsAPI::OpenGL)
			{
				uv0 = {0, 1};
				uv1 = {1, 0};
			}
			ImGui::Image(m_FramebufferTextureID, size, uv0, uv1);

			if (size.x != m_PreviousViewportSize.x || size.y != m_PreviousViewportSize.y)
			{
				if (size.x > 0 && size.y > 0)
				{
					ResizeFramebuffer(size);
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		RenderNewProjectWindow();
		for (auto panel : m_Panels)
		{
			if (panel->IsOpen())
			{
				panel->Update();
				panel->Render();
			}
		}

		ImGui::End();
	}

	virtual void Render(Nexus::TimeSpan time) override
	{
		Nexus::GetApplication()->GetPrimarySwapchain()->Prepare();

		Nexus::Graphics::RenderTarget target(m_Framebuffer);

		if (m_Project && m_Project->IsSceneLoaded())
		{
			Nexus::Scene *scene = m_Project->GetLoadedScene();
			m_Renderer->Begin(scene, target, time);
			m_Renderer->End();
		}

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
	Nexus::Ref<Nexus::Graphics::Framebuffer>				  m_Framebuffer	  = nullptr;

	ImTextureID m_FramebufferTextureID = {};

	std::unique_ptr<Nexus::Graphics::Renderer3D> m_Renderer;
	Nexus::Ref<Nexus::Graphics::Cubemap>		 m_Cubemap = nullptr;
	Nexus::Ref<Nexus::Graphics::Model>			 m_Model   = nullptr;

	ImVec2 m_PreviousViewportSize = {0, 0};
	bool   m_NewProjectWindowOpen = false;

	Nexus::Ref<Nexus::Project> m_Project			   = nullptr;
	std::vector<Panel *>	   m_Panels				   = {};
	EditorPropertiesPanel	  *m_EditorPropertiesPanel = nullptr;

	SceneState m_SceneState = SceneState::Stopped;
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