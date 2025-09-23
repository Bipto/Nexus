#include "Demos/Demo.hpp"

#include "Demos/AudioDemo.hpp"
#include "Demos/BatchingDemo.hpp"
#include "Demos/CameraDemo.hpp"
#include "Demos/ClearRectDemo.hpp"
#include "Demos/ClearScreenDemo.hpp"
#include "Demos/ClippingAndTriangulationDemo.hpp"
#include "Demos/ComputeDemo.hpp"
#include "Demos/ComputeIndirectDemo.hpp"
#include "Demos/CubemapDemo.hpp"
#include "Demos/Demo3D.hpp"
#include "Demos/FramebufferDemo.hpp"
#include "Demos/GeometryShaderDemo.hpp"
#include "Demos/HelloTriangle.hpp"
#include "Demos/HelloTriangleIndexed.hpp"
#include "Demos/HelloTriangleIndirect.hpp"
#include "Demos/HelloTriangleIndirectIndexedDemo.hpp"
#include "Demos/HelloTriangleMeshShaders.hpp"
#include "Demos/HelloTriangleMeshShadersIndirect.hpp"
#include "Demos/InstancingDemo.hpp"
#include "Demos/Lighting.hpp"
#include "Demos/MipmapDemo.hpp"
#include "Demos/Models.hpp"
#include "Demos/RayTracingDemo.hpp"
#include "Demos/Splines.hpp"
#include "Demos/StorageBufferDemo.hpp"
#include "Demos/Texturing.hpp"
#include "Demos/TimingDemo.hpp"
#include "Demos/UniformBufferDemo.hpp"
#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/Color.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Nexus.hpp"

#include "Nexus-Core/EntryPoint.hpp"

struct DemoInfo
{
	std::string Name;
	Demos::Demo *(*CreationFunction)(Nexus::Application *,
									 const std::string						   &name,
									 Nexus::ImGuiUtils::ImGuiGraphicsRenderer  *imGuiRenderer,
									 Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue);
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
		std::vector<Nexus::Graphics::QueueFamilyInfo> queueFamilies = m_GraphicsDevice->GetQueueFamilies();
		for (const Nexus::Graphics::QueueFamilyInfo &queueFamily : queueFamilies)
		{
			if (queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Graphics) &&
				queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Compute) &&
				queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Transfer))
			{
				Nexus::Graphics::CommandQueueDescription queueDesc = {};
				queueDesc.QueueFamilyIndex						   = queueFamily.QueueFamily;
				queueDesc.QueueIndex							   = 0;
				queueDesc.DebugName								   = "Demo Graphics Queue";
				m_CommandQueue									   = m_GraphicsDevice->CreateCommandQueue(queueDesc);
			}
		}

		m_ImGuiRenderer		  = std::make_unique<Nexus::ImGuiUtils::ImGuiGraphicsRenderer>(this, m_CommandQueue);
		ImGuiContext *context = m_ImGuiRenderer->GetContext();
		ImGui::SetCurrentContext(context);

		ImGui::GetStyle().ScrollbarSize = 20.0f;

		ImGuiIO &io = m_ImGuiRenderer->GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		int size = 20;

#if defined(__ANDROID__) || defined(ANDROID)
		size = 42;
#endif

		std::string fontPath = Nexus::FileSystem::GetFilePathAbsolute("resources/demo/fonts/roboto/roboto-regular.ttf");
		io.FontDefault		 = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
		m_ImGuiRenderer->RebuildFontAtlas();

		RegisterGraphicsDemo<Demos::ClearScreenDemo>("Clear Colour");
		RegisterGraphicsDemo<Demos::ClearRectDemo>("Clear Rects");
		RegisterGraphicsDemo<Demos::TimingDemo>("Timings");
		RegisterGraphicsDemo<Demos::HelloTriangleDemo>("Hello Triangle");
		RegisterGraphicsDemo<Demos::HelloTriangleIndexedDemo>("Hello Triangle Indexed");
		RegisterGraphicsDemo<Demos::HelloTriangleIndirectDemo>("Hello Triangle Indirect");
		RegisterGraphicsDemo<Demos::HelloTriangleIndirectIndexedDemo>("Hello Triangle Indexed Indirect");

		const Nexus::Graphics::DeviceFeatures deviceFeatures = m_GraphicsDevice->GetPhysicalDeviceFeatures();
		if (deviceFeatures.SupportsMeshTaskShaders)
		{
			RegisterGraphicsDemo<Demos::HelloTriangleMeshShadersDemo>("Hello Triangle Mesh Shaders");
			RegisterGraphicsDemo<Demos::HelloTriangleMeshShadersIndirect>("Hello Triangle Mesh Shaders Indirect");
		}

		RegisterGraphicsDemo<Demos::TexturingDemo>("Texturing");
		RegisterGraphicsDemo<Demos::BatchingDemo>("Batching");
		RegisterGraphicsDemo<Demos::FramebufferDemo>("Framebuffers");
		RegisterGraphicsDemo<Demos::UniformBufferDemo>("Uniform Buffers");
		RegisterGraphicsDemo<Demos::StorageBufferDemo>("Storage Buffers");
		RegisterGraphicsDemo<Demos::Demo3D>("3D");
		RegisterGraphicsDemo<Demos::CameraDemo>("Camera");
		RegisterGraphicsDemo<Demos::LightingDemo>("Lighting");
		RegisterGraphicsDemo<Demos::ModelDemo>("Models");
		RegisterGraphicsDemo<Demos::InstancingDemo>("Instancing");
		RegisterGraphicsDemo<Demos::MipmapDemo>("Mipmaps");
		RegisterGraphicsDemo<Demos::CubemapDemo>("Cubemaps");
		RegisterGraphicsDemo<Demos::ComputeDemo>("Compute");
		RegisterGraphicsDemo<Demos::ComputeIndirectDemo>("Compute Indirect");

		// geometry shaders have some issues with SPIRV-Cross HLSL backend
		if (m_GraphicsDevice->GetGraphicsAPI() != Nexus::Graphics::GraphicsAPI::D3D12)
		{
			RegisterGraphicsDemo<Demos::GeometryShaderDemo>("Geometry Shader");
		}

		RegisterGraphicsDemo<Demos::RayTracingDemo>("Ray Tracing");

		RegisterAudioDemo<Demos::AudioDemo>("Audio");
		RegisterUtilsDemo<Demos::ClippingAndTriangulationDemo>("Polygon clipping and triangulation");
		RegisterUtilsDemo<Demos::Splines>("Splines");

		m_CommandList = m_GraphicsDevice->CreateCommandList();
	}

	template<typename T>
	void RegisterGraphicsDemo(const std::string &name)
	{
		DemoInfo info;
		info.Name			  = name;
		info.CreationFunction = [](Nexus::Application						 *app,
								   const std::string						 &name,
								   Nexus::ImGuiUtils::ImGuiGraphicsRenderer	 *imGuiRenderer,
								   Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer, commandQueue); };
		m_GraphicsDemos.push_back(info);
	}

	template<typename T>
	void RegisterAudioDemo(const std::string &name)
	{
		DemoInfo info;
		info.Name			  = name;
		info.CreationFunction = [](Nexus::Application						 *app,
								   const std::string						 &name,
								   Nexus::ImGuiUtils::ImGuiGraphicsRenderer	 *imGuiRenderer,
								   Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer, commandQueue); };
		m_AudioDemos.push_back(info);
	}

	template<typename T>
	void RegisterUtilsDemo(const std::string &name)
	{
		DemoInfo info;
		info.Name			  = name;
		info.CreationFunction = [](Nexus::Application						 *app,
								   const std::string						 &name,
								   Nexus::ImGuiUtils::ImGuiGraphicsRenderer	 *imGuiRenderer,
								   Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer, commandQueue); };
		m_UtilsDemos.push_back(info);
	}

	template<typename T>
	void RegisterScriptingDemo(const std::string &name)
	{
		DemoInfo info;
		info.Name			  = name;
		info.CreationFunction = [](Nexus::Application						 *app,
								   const std::string						 &name,
								   Nexus::ImGuiUtils::ImGuiGraphicsRenderer	 *imGuiRenderer,
								   Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer, commandQueue); };
		m_ScriptingDemos.push_back(info);
	}

	virtual void Update(Nexus::TimeSpan time) override
	{
		if (m_CurrentDemo)
			m_CurrentDemo->Update(time);
	}

	void RenderDemoList(std::span<DemoInfo> demos, const std::string &menuName)
	{
		NX_PROFILE_FUNCTION();

		if (ImGui::TreeNodeEx(menuName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
		{
			for (const auto &pair : demos)
			{
				auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;

				if (ImGui::TreeNodeEx(pair.Name.c_str(), flags))
				{
					if (ImGui::IsItemClicked())
					{
						m_CurrentDemo = std::unique_ptr<Demos::Demo>(pair.CreationFunction(this, pair.Name, m_ImGuiRenderer.get(), m_CommandQueue));
						m_CurrentDemo->Load();
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}

	void RenderDemoInfo()
	{
		NX_PROFILE_FUNCTION();

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

				std::string info = m_CurrentDemo->GetInfo();
				if (!info.empty())
				{
					std::string description = std::string("Description: ") + info;
					ImGui::Text("%s", description.c_str());
				}

				ImGui::Separator();

				std::shared_ptr<Nexus::Graphics::IPhysicalDevice> physicalDevice = m_GraphicsDevice->GetPhysicalDevice();
				std::string apiName = std::string("Running on : ") + std::string(m_GraphicsDevice->GetAPIName());
				ImGui::Text("%s", apiName.c_str());
				std::string deviceName = std::string("Device: ") + physicalDevice->GetDeviceName();
				ImGui::Text("%s", deviceName.c_str());

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
	}

	void RenderPerformanceInfo()
	{
		NX_PROFILE_FUNCTION();
		if (ImGui::CollapsingHeader("Performance"))
		{
			const auto &results = Nexus::Timings::Profiler::Get().GetResults();
			for (const auto &profileResult : results)
			{
				std::string output = std::string(profileResult.Name) + std::string(": ") +
									 std::to_string(profileResult.Time.GetMilliseconds<float>()) + std::string(" Ms");
				ImGui::Text("%s", output.c_str());
			}
		}

		Nexus::Timings::Profiler::Get().Reset();
	}

	virtual void Render(Nexus::TimeSpan time) override
	{
		NX_PROFILE_FUNCTION();

		m_ImGuiRenderer->BeforeLayout(time);

		if (Nexus::Input::IsKeyDown(Nexus::ScanCode::F11))
		{
			auto window = this->GetPrimaryWindow();
			window->ToggleFullscreen();
		}

		{
			NX_PROFILE_SCOPE("Render UI");
			ImGui::Begin("Demos");
			RenderDemoInfo();
			RenderPerformanceInfo();
			ImGui::End();
		}

		if (m_CurrentDemo)
		{
			NX_PROFILE_SCOPE("Render Demo");
			m_CurrentDemo->Render(time);
		}
		else
		{
			NX_PROFILE_SCOPE("Clear Screen");
			m_CommandList->Begin();

			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {Nexus::GetApplication()->GetPrimarySwapchain()});
			m_CommandList->ClearColourTarget(0, {0.35f, 0.25f, 0.42f, 1.0f});

			Nexus::Graphics::MemoryBarrierDesc barrierDesc = {};
			barrierDesc.BeforeAccess					   = Nexus::Graphics::BarrierAccess::ColourAttachmentRead;
			barrierDesc.AfterAccess						   = Nexus::Graphics::BarrierAccess::ColourAttachmentWrite;
			barrierDesc.BeforeStage						   = Nexus::Graphics::BarrierPipelineStage::AllCommands;
			barrierDesc.AfterStage						   = Nexus::Graphics::BarrierPipelineStage::ColourAttachmentOutput;
			m_CommandList->SubmitMemoryBarrier(barrierDesc);

			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
			// m_GraphicsDevice->WaitForIdle();
		}

		m_ImGuiRenderer->AfterLayout();

		{
			NX_PROFILE_SCOPE("CommandQueue::Present");
			m_CommandQueue->Present(Nexus::GetApplication()->GetPrimarySwapchain());
		}
	}

	virtual void OnResize(Nexus::Point2D<uint32_t> size) override
	{
		NX_PROFILE_FUNCTION();

		if (m_CurrentDemo)
			m_CurrentDemo->OnResize(size);
	}

	virtual void Unload() override
	{
	}

  private:
	Nexus::Ref<Nexus::Graphics::ICommandQueue> m_CommandQueue = nullptr;

	Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList	  = nullptr;
	std::unique_ptr<Demos::Demo>			 m_CurrentDemo	  = nullptr;
	std::vector<DemoInfo>					 m_GraphicsDemos  = {};
	std::vector<DemoInfo>					 m_AudioDemos	  = {};
	std::vector<DemoInfo>					 m_ScriptingDemos = {};
	std::vector<DemoInfo>					 m_UtilsDemos	  = {};

	std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
	Nexus::ApplicationSpecification spec;

	spec.GraphicsCreateInfo.API	  = Nexus::Graphics::GraphicsAPI::OpenGL;
	spec.GraphicsCreateInfo.Debug = true;

	spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;

	spec.WindowProperties.Width			   = 1280;
	spec.WindowProperties.Height		   = 720;
	spec.WindowProperties.Title			   = "Demo";
	spec.WindowProperties.Resizable		   = true;
	spec.WindowProperties.RendersPerSecond = {};
	spec.WindowProperties.UpdatesPerSecond = {};

	spec.SwapchainDescription.Samples		   = 1;
	spec.SwapchainDescription.ImagePresentMode = Nexus::Graphics::PresentMode::Immediate;

	spec.Organization = "Nexus";
	spec.App		  = "Demo";

	return new DemoApplication(spec);
}