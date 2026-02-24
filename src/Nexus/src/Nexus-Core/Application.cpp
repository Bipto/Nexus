#include "Nexus-Core/Application.hpp"

#include "OpenAL/OpenAL-API.hpp"

#include "Platform/Timings/Profiler.hpp"

#include "Platform/Input/Input.hpp"
#include "Platform/Logging/Log.hpp"

#include "Nexus-Core/Utils/GraphicsUtils.hpp"
#include "Platform/Events/EventDispatcher.hpp"
#include "Platform/Platform.hpp"

namespace Nexus
{
	Application::Application(const ApplicationDescription &spec)
	{
		m_Description = spec;

		m_Window = Platform::CreatePlatformWindow(spec.WindowProperties);

		m_GraphicsAPI = std::unique_ptr<Graphics::IGraphicsAPI>(Graphics::IGraphicsAPI::CreateAPI(spec.GraphicsCreateInfo));

		std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> physicalDevices = m_GraphicsAPI->GetPhysicalDevices();
		m_GraphicsDevice = std::unique_ptr<Graphics::IGraphicsDevice>(m_GraphicsAPI->CreateGraphicsDevice(physicalDevices[0]));

		// iterate through all available command queues
		std::vector<Nexus::Graphics::QueueFamilyInfo> queueFamilies = m_GraphicsDevice->GetQueueFamilies();
		for (const Nexus::Graphics::QueueFamilyInfo &queueFamily : queueFamilies)
		{
			if (queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Graphics) &&
				queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Compute) &&
				queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Transfer))
			{
				// create graphics queue
				{
					Nexus::Graphics::CommandQueueDescription queueDesc = {};
					queueDesc.QueueFamilyIndex						   = queueFamily.QueueFamily;
					queueDesc.QueueIndex							   = 0;
					queueDesc.DebugName								   = "Application Graphics Queue";
					m_CommandQueueGroup.GraphicsQueue				   = m_GraphicsDevice->CreateCommandQueue(queueDesc);
				}
			}
		}

		Ref<Graphics::ISurface> surface			   = Utils::CreateSurfaceForWindow(m_GraphicsDevice.get(), m_Window);
		m_Description.SwapchainDescription.Surface = surface;

		// hack, this probably needs removing at some point
		m_Description.SwapchainDescription.Width  = m_Description.WindowProperties.Width;
		m_Description.SwapchainDescription.Height = m_Description.WindowProperties.Height;
		m_Swapchain								  = m_CommandQueueGroup.GraphicsQueue->CreateSwapchain(m_Description.SwapchainDescription);

		m_AudioDevice = Nexus::Audio::OpenAL::CreateDevice();

		m_Window->SetRenderFunction([&](Nexus::TimeSpan time) { Render(time); });
		m_Window->SetUpdateFunction([&](Nexus::TimeSpan time) { Update(time); });
		m_Window->SetTickFunction([&](Nexus::TimeSpan time) { Tick(time); });
		m_Window->AddResizeCallback(
			[&](const Nexus::WindowResizedEventArgs &args)
			{
				auto [width, height] = args.Size;
				m_Swapchain->Resize(width, height);
			});
	}

	Application::~Application()
	{
	}

	void Application::MainLoop()
	{
		NX_PROFILE_FUNCTION();

		EventDispatcher messageBus = {};

		{
			NX_PROFILE_SCOPE("Platform::Update");
			Platform::Update();
		}

		if (m_Description.EventDriven)
		{
			NX_PROFILE_SCOPE("Platform::WaitEvent");
			Platform::WaitEvent(m_LayerStack);
		}
		else
		{
			NX_PROFILE_SCOPE("Platform::PollEvents");
			Platform::PollEvents(m_LayerStack);
		}

		if (!Platform::AreAnyWindowsOpen())
		{
			m_Running = false;
		}

		NX_MARK_FRAME_END();
	}

	Nexus::IWindow *Application::GetPrimaryWindow()
	{
		return m_Window;
	}

	Ref<Nexus::Graphics::ISwapchain> Application::GetPrimarySwapchain()
	{
		return m_Swapchain;
	}

	bool Application::IsWindowFocussed()
	{
		return m_Window->IsFocussed();
	}

	WindowState Application::GetCurrentWindowState()
	{
		return m_Window->GetCurrentWindowState();
	}

	void Application::SetIsMouseVisible(bool visible)
	{
		m_Window->SetIsMouseVisible(visible);
	}

	void Application::Close()
	{
		m_Running = false;
	}

	Graphics::IGraphicsDevice *Application::GetGraphicsDevice()
	{
		return m_GraphicsDevice.get();
	}

	Ref<Graphics::ICommandQueue> Application::GetGraphicsCommandQueue()
	{
		return m_CommandQueueGroup.GraphicsQueue;
	}

	Audio::AudioDevice *Application::GetAudioDevice()
	{
		return m_AudioDevice.get();
	}

	bool Application::IsRunning() const
	{
		return m_Running;
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	std::string Application::GetApplicationPath()
	{
		return Platform::GetApplicationPath(m_Description.Organization.c_str(), m_Description.App.c_str());
	}
}	 // namespace Nexus