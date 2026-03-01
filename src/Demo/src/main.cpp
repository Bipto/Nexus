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
#include "Nexus-Core/Graphics/Color.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Nexus-Core/EntryPoint.hpp"

#include "DemoInfo.hpp"
#include "Layers/DemoImGuiLayer.hpp"
#include "Layers/DemoLayer.hpp"

class DemoApplication : public Nexus::Application
{
  public:
	DemoApplication(const Nexus::ApplicationDescription &spec) : Application(spec)
	{
	}

	virtual ~DemoApplication()
	{
	}

	virtual void Load() override
	{
		m_CommandQueue = m_CommandQueueGroup.GraphicsQueue;

		m_LayerStack.AddOverlay<DemoImGuiLayer>(this, m_CommandQueue);
		m_LayerStack.AddLayer<DemoLayer>(this, m_CommandQueue);

		//		m_ImGuiRenderer		  = std::make_unique<Nexus::ImGuiUtils::ImGuiGraphicsRenderer>(this, m_CommandQueue);
		//		ImGuiContext *context = m_ImGuiRenderer->GetContext();
		//		ImGui::SetCurrentContext(context);
		//
		//		ImGui::GetStyle().ScrollbarSize = 20.0f;
		//
		//		ImGuiIO &io = m_ImGuiRenderer->GetIO();
		//		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//
		//		int size = 20;
		//
		// #if defined(__ANDROID__) || defined(ANDROID)
		//		size = 42;
		// #endif
		//
		//		std::string fontPath = Nexus::FileSystem::GetFilePathAbsolute("resources/demo/fonts/roboto/roboto-regular.ttf");
		//		io.FontDefault		 = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
		//		m_ImGuiRenderer->RebuildFontAtlas();
		//
		//
		//		m_CommandList = m_CommandQueue->CreateCommandList();
	}

	virtual void Update(Nexus::TimeSpan time) override
	{
		if (m_CurrentDemo)
			m_CurrentDemo->Update(time);
	}

	virtual void Render(Nexus::TimeSpan time) override
	{
		NX_PROFILE_FUNCTION();

		/*m_ImGuiRenderer->BeforeLayout(time);

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

			Nexus::Ref<Nexus::Graphics::ISwapchain>	  swapchain	  = Nexus::GetApplication()->GetPrimarySwapchain();
			Nexus::Ref<Nexus::Graphics::IFramebuffer> framebuffer = swapchain->GetCurrentFramebuffer();
			m_CommandList->SetFramebuffer(framebuffer);
			m_CommandList->ClearColourTarget(0, {0.35f, 0.25f, 0.42f, 1.0f});

			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
		}

		m_ImGuiRenderer->AfterLayout();
		m_GraphicsDevice->WaitForIdle();

		{
			NX_PROFILE_SCOPE("CommandQueue::Present");
			Nexus::GetApplication()->GetPrimarySwapchain()->SwapBuffers(Nexus::Graphics::SwapchainPresentDescription {});
		}*/
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

	Nexus::Ref<Nexus::Graphics::ICommandList>	 m_CommandList = nullptr;
	std::unique_ptr<Demos::Demo>				 m_CurrentDemo = nullptr;
	std::map<std::string, std::vector<DemoInfo>> m_Demos	   = {};

	std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
	Nexus::ApplicationDescription desc;

	desc.GraphicsCreateInfo.API	  = Nexus::Graphics::GraphicsAPI::OpenGL;
	desc.GraphicsCreateInfo.Debug = true;

	desc.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;

	desc.WindowProperties.Width			   = 1280;
	desc.WindowProperties.Height		   = 720;
	desc.WindowProperties.Title			   = "Demo";
	desc.WindowProperties.Flags			   = WindowFlags_Resizable | WindowFlags_HighPixelDensity;
	desc.WindowProperties.RendersPerSecond = {};
	desc.WindowProperties.UpdatesPerSecond = {};

	desc.SwapchainDescription.Samples		   = 1;
	desc.SwapchainDescription.ImagePresentMode = Nexus::Graphics::PresentMode::Immediate;

	desc.Organization = "Nexus";
	desc.App		  = "Demo";

	return new DemoApplication(desc);
}