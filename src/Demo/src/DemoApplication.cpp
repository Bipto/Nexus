#include "DemoApplication.hpp"

#include "Layers/DemoImGuiLayer.hpp"
#include "Layers/DemoLayer.hpp"

DemoApplication::DemoApplication(const Nexus::ApplicationDescription &desc) : Application(desc)
{
}

void DemoApplication::Load()
{
	DemoImGuiLayer *imGuiLayer = m_LayerStack.AddOverlay<DemoImGuiLayer>(this, m_CommandQueueGroup.GraphicsQueue);
	DemoLayer	   *demoLayer  = m_LayerStack.AddLayer<DemoLayer>(this, m_CommandQueueGroup.GraphicsQueue);
	imGuiLayer->SetDemoSelectedCallback([demoLayer](std::shared_ptr<Demos::Demo> demo) { demoLayer->SetDemo(demo); });
}

void DemoApplication::Update(Nexus::TimeSpan time)
{
}

void DemoApplication::Render(Nexus::TimeSpan time)
{
}

void DemoApplication::OnResize(Nexus::Point2D<uint32_t> size)
{
}

void DemoApplication::Unload()
{
}