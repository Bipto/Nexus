#include "../Demos/Demo.hpp"
#include "Nexus-Core/Application.hpp"
#include "Platform/Layers/Layer.hpp"

class DemoLayer final : public Nexus::ILayer
{
  public:
	DemoLayer(Nexus::Application *app, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue);
	~DemoLayer() final = default;
	bool OnEvent(const Nexus::Event &event) final;
	void OnRender(Nexus::TimeSpan time, Nexus::IWindow *window) final;
	void OnUpdate(Nexus::TimeSpan time, Nexus::IWindow *window) final;
	void OnTick(Nexus::TimeSpan time, Nexus::IWindow *window) final;

  protected:
	Nexus::Application						  *m_Application  = nullptr;
	Nexus::Ref<Nexus::Graphics::ICommandQueue> m_CommandQueue = nullptr;
	Nexus::Ref<Nexus::Graphics::ICommandList>  m_CommandList  = nullptr;
	Demos::Demo								  *m_Demo		  = nullptr;
};