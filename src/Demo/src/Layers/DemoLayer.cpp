#include "DemoLayer.hpp"

#include "Nexus-Core/Runtime.hpp"

#include "Platform/Events/EventDispatcher.hpp"

#include "Profiling/Profiler.hpp"

DemoLayer::DemoLayer(
    Nexus::Application *app, Nexus::Graphics::CommandQueueHandle commandQueue
)
    : m_Application(app), m_CommandQueue(commandQueue)
{
    m_CommandList = m_CommandQueue->CreateCommandList();
}

bool DemoLayer::OnEvent(const Nexus::Event &event)
{
    if (m_Demo)
    {
        m_Demo->OnEvent(event);
    }

    return true;
}

void DemoLayer::OnRender(Nexus::TimeSpan time, Nexus::IWindow *window)
{
    if (m_Demo)
    {
        NX_PROFILE_SCOPE("Render Demo");
        m_Demo->Render(time);
    }
    else
    {
        NX_PROFILE_SCOPE("Clear Screen");
        m_CommandList->Begin();

        Nexus::Graphics::SwapchainHandle swapchain =
            Nexus::GetApplication()->GetPrimarySwapchain();
        Nexus::Graphics::FramebufferHandle framebuffer =
            swapchain->GetCurrentFramebuffer();
        m_CommandList->SetFramebuffer(framebuffer);
        m_CommandList->ClearColourTarget(0, {0.35f, 0.25f, 0.42f, 1.0f});

        m_CommandList->End();

        m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
    }
}

void DemoLayer::OnUpdate(Nexus::TimeSpan time, Nexus::IWindow *window)
{
}

void DemoLayer::OnTick(Nexus::TimeSpan time, Nexus::IWindow *window)
{
}

void DemoLayer::SetDemo(std::shared_ptr<Demos::Demo> demo)
{
    m_Demo = demo;
}