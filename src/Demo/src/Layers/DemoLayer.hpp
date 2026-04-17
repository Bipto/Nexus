#include "../Demos/Demo.hpp"
#include "Nexus-Core/Application.hpp"
#include "Platform/Layers/Layer.hpp"

class DemoLayer final : public Nexus::ILayer
{
  public:
	DemoLayer(Nexus::Application *app, Nexus::Graphics::CommandQueueHandle commandQueue);
	~DemoLayer() final = default;
	bool OnEvent(const Nexus::Event &event) final;
	void OnRender(Nexus::TimeSpan time, Nexus::IWindow *window) final;
	void OnUpdate(Nexus::TimeSpan time, Nexus::IWindow *window) final;
	void OnTick(Nexus::TimeSpan time, Nexus::IWindow *window) final;
	void SetDemo(std::shared_ptr<Demos::Demo> demo);

  protected:
	Nexus::Application						 *m_Application	 = nullptr;
	Nexus::Graphics::CommandQueueHandle		  m_CommandQueue = {};
	Nexus::Ref<Nexus::Graphics::ICommandList> m_CommandList	 = nullptr;
	std::shared_ptr<Demos::Demo>			  m_Demo		 = nullptr;
};