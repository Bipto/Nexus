#include "DemoLayer.hpp"

#include "Nexus-Core/Runtime.hpp"

#include "Platform/Events/EventDispatcher.hpp"

#include <print>

DemoLayer::DemoLayer(Nexus::Application *app, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
	: m_Application(app),
	  m_CommandQueue(commandQueue)
{
	m_CommandList = m_CommandQueue->CreateCommandList();
}

bool DemoLayer::OnEvent(const Nexus::Event &event)
{
	Nexus::EventDispatcher dispatcher = {};
	dispatcher.Subscribe<Nexus::MouseButtonPressedEventArgs>([](const Nexus::MouseButtonPressedEventArgs &args)
															 { std::println("Demo layer clicked!"); });
	dispatcher.Dispatch(event);

	return true;
}

void DemoLayer::OnRender(Nexus::TimeSpan time, Nexus::IWindow *window)
{
	m_CommandList->Begin();

	Nexus::Ref<Nexus::Graphics::ISwapchain>	  swapchain	  = Nexus::GetApplication()->GetPrimarySwapchain();
	Nexus::Ref<Nexus::Graphics::IFramebuffer> framebuffer = swapchain->GetCurrentFramebuffer();
	m_CommandList->SetFramebuffer(framebuffer);
	m_CommandList->ClearColourTarget(0, {0.35f, 0.25f, 0.42f, 1.0f});

	m_CommandList->End();

	m_CommandQueue->SubmitCommandList(m_CommandList);
}

void DemoLayer::OnUpdate(Nexus::TimeSpan time, Nexus::IWindow *window)
{
}

void DemoLayer::OnTick(Nexus::TimeSpan time, Nexus::IWindow *window)
{
}