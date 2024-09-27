#include "Demos/AudioDemo.hpp"
#include "Demos/BatchingDemo.hpp"
#include "Demos/CameraDemo.hpp"
#include "Demos/ClearScreenDemo.hpp"
#include "Demos/ClippingAndTriangulationDemo.hpp"
#include "Demos/CubemapDemo.hpp"
#include "Demos/Demo3D.hpp"
#include "Demos/FramebufferDemo.hpp"
#include "Demos/HelloTriangle.hpp"
#include "Demos/HelloTriangleIndexed.hpp"
#include "Demos/InstancingDemo.hpp"
#include "Demos/Lighting.hpp"
#include "Demos/MipmapDemo.hpp"
#include "Demos/Models.hpp"
#include "Demos/PythonDemo.hpp"
#include "Demos/Splines.hpp"
#include "Demos/Texturing.hpp"
#include "Demos/TimingDemo.hpp"
#include "Demos/UniformBufferDemo.hpp"
#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/Color.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Nexus.hpp"

struct DemoInfo
{
	std::string Name;
	Demos::Demo *(*CreationFunction)(Nexus::Application *, const std::string &name, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer);
};

class DemoApplication : public Nexus::Application
{
  public:
	DemoApplication(const Nexus::ApplicationSpecification &spec) : Application(spec)
	{
	}

	virtual ~DemoApplication()
	{
	}

	virtual void Load() override
	{
		m_ImGuiRenderer = std::make_unique<Nexus::ImGuiUtils::ImGuiGraphicsRenderer>(this);

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		int size = 19;

#if defined(__ANDROID__) || defined(ANDROID)
		size = 38;
#endif

		auto &io = ImGui::GetIO();

		std::string fontPath = Nexus::FileSystem::GetFilePathAbsolute("resources/demo/fonts/roboto/roboto-regular.ttf");
		io.FontDefault		 = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
		m_ImGuiRenderer->RebuildFontAtlas();

		RegisterGraphicsDemo<Demos::ClearScreenDemo>("Clear Colour");
		RegisterGraphicsDemo<Demos::TimingDemo>("Timings");
		RegisterGraphicsDemo<Demos::HelloTriangleDemo>("Hello Triangle");
		RegisterGraphicsDemo<Demos::HelloTriangleIndexedDemo>("Hello Triangle Indexed");
		RegisterGraphicsDemo<Demos::TexturingDemo>("Texturing");
		RegisterGraphicsDemo<Demos::BatchingDemo>("Batching");
		RegisterGraphicsDemo<Demos::FramebufferDemo>("Framebuffers");
		RegisterGraphicsDemo<Demos::UniformBufferDemo>("Uniform Buffers");
		RegisterGraphicsDemo<Demos::Demo3D>("3D");
		RegisterGraphicsDemo<Demos::CameraDemo>("Camera");
		RegisterGraphicsDemo<Demos::LightingDemo>("Lighting");
		RegisterGraphicsDemo<Demos::ModelDemo>("Models");
		RegisterGraphicsDemo<Demos::InstancingDemo>("Instancing");
		RegisterGraphicsDemo<Demos::MipmapDemo>("Mipmaps");
		RegisterGraphicsDemo<Demos::CubemapDemo>("Cubemaps");
		RegisterAudioDemo<Demos::AudioDemo>("Audio");
		RegisterScriptingDemo<Demos::PythonDemo>("Python");
		RegisterUtilsDemo<Demos::ClippingAndTriangulationDemo>("Polygon clipping and triangulation");
		RegisterUtilsDemo<Demos::Splines>("Splines");

		m_CommandList = m_GraphicsDevice->CreateCommandList();
	}

	template<typename T>
	void RegisterGraphicsDemo(const std::string &name)
	{
		DemoInfo info;
		info.Name = name;
		info.CreationFunction =
		[](Nexus::Application *app, const std::string &name, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer); };
		m_GraphicsDemos.push_back(info);
	}

	template<typename T>
	void RegisterAudioDemo(const std::string &name)
	{
		DemoInfo info;
		info.Name = name;
		info.CreationFunction =
		[](Nexus::Application *app, const std::string &name, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer); };
		m_AudioDemos.push_back(info);
	}

	template<typename T>
	void RegisterUtilsDemo(const std::string &name)
	{
		DemoInfo info;
		info.Name = name;
		info.CreationFunction =
		[](Nexus::Application *app, const std::string &name, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer); };
		m_UtilsDemos.push_back(info);
	}

	template<typename T>
	void RegisterScriptingDemo(const std::string &name)
	{
		DemoInfo info;
		info.Name = name;
		info.CreationFunction =
		[](Nexus::Application *app, const std::string &name, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer); };
		m_ScriptingDemos.push_back(info);
	}

	virtual void Update(Nexus::TimeSpan time) override
	{
		if (m_CurrentDemo)
			m_CurrentDemo->Update(time);
	}

	void RenderDemoList(std::span<DemoInfo> demos, const std::string &menuName)
	{
		if (ImGui::TreeNodeEx(menuName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
		{
			for (const auto &pair : demos)
			{
				auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;

				if (ImGui::TreeNodeEx(pair.Name.c_str(), flags))
				{
					if (ImGui::IsItemClicked())
					{
						m_CurrentDemo = std::unique_ptr<Demos::Demo>(pair.CreationFunction(this, pair.Name, m_ImGuiRenderer.get()));
						m_CurrentDemo->Load();
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}

	virtual void Render(Nexus::TimeSpan time) override
	{
		m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();

		m_ImGuiRenderer->BeforeLayout(time);

		if (Nexus::Input::IsKeyPressed(Nexus::KeyCode::F11))
		{
			auto window = this->GetPrimaryWindow();
			window->ToggleFullscreen();
		}

		{
			ImGui::Begin("Demos");

			if (m_CurrentDemo)
			{
				if (ImGui::Button("<- Back"))
				{
					m_CurrentDemo = nullptr;
				}

				// required because demo could be deleted in the previous if statement
				if (m_CurrentDemo)
				{
					// render demo name
					std::string label = std::string("Selected Demo - ") + m_CurrentDemo->GetName();
					ImGui::Text("%s", label.c_str());

					std::string apiName = std::string("Running on : ") + std::string(m_GraphicsDevice->GetAPIName());
					ImGui::Text("%s", apiName.c_str());

					// render framerate
					std::stringstream ss;
					float			  fps = ImGui::GetIO().Framerate;
					ss << "Running at " << std::to_string(fps) << " FPS";
					ImGui::Text("%s", ss.str().c_str());
					m_CurrentDemo->RenderUI();
				}
			}
			else
			{
				RenderDemoList(m_GraphicsDemos, "Graphics");
				RenderDemoList(m_AudioDemos, "Audio");
				RenderDemoList(m_ScriptingDemos, "Scripting");
				RenderDemoList(m_UtilsDemos, "Utils");
			}

			ImGui::End();
		}

		if (m_CurrentDemo)
		{
			m_CurrentDemo->Render(time);
		}
		else
		{
			m_CommandList->Begin();

			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
			m_CommandList->ClearColorTarget(0, {0.35f, 0.25f, 0.42f, 1.0f});

			m_CommandList->End();
			m_GraphicsDevice->SubmitCommandList(m_CommandList);
		}

		m_ImGuiRenderer->AfterLayout();

		m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->SwapBuffers();
	}

	virtual void OnResize(Nexus::Point2D<uint32_t> size) override
	{
		if (m_CurrentDemo)
			m_CurrentDemo->OnResize(size);
	}

	virtual bool OnEvent(const Nexus::InputEvent &event, Nexus::Window *window) override
	{
		return false;
	}

	virtual void Unload() override
	{
	}

  private:
	Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
	std::unique_ptr<Demos::Demo>			 m_CurrentDemo = nullptr;
	std::vector<DemoInfo>					 m_GraphicsDemos;
	std::vector<DemoInfo>					 m_AudioDemos;
	std::vector<DemoInfo>					 m_ScriptingDemos;
	std::vector<DemoInfo>					 m_UtilsDemos;

	std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
	Nexus::ApplicationSpecification spec;
	spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::D3D12;
	spec.AudioAPI	 = Nexus::Audio::AudioAPI::OpenAL;

	spec.WindowProperties.Width		= 1280;
	spec.WindowProperties.Height	= 720;
	spec.WindowProperties.Title		= "Demo";
	spec.WindowProperties.Resizable = true;
	spec.WindowProperties.RendersPerSecond = 60;
	spec.WindowProperties.UpdatesPerSecond = {};

	spec.SwapchainSpecification.Samples	   = Nexus::Graphics::SampleCount::SampleCount8;
	spec.SwapchainSpecification.VSyncState = Nexus::Graphics::VSyncState::Enabled;

	return new DemoApplication(spec);
}