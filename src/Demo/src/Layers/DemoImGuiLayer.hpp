#pragma once

#include <functional>
#include <memory>

#include "../DemoInfo.hpp"
#include "../Demos/Demo.hpp"

#include "Nexus-Core/ImGui/ImGuiLayer.hpp"
#include "RHI/GraphicsDevice.hpp"

class DemoImGuiLayer : public Nexus::ImGuiLayer
{
  public:
	DemoImGuiLayer(Nexus::Application *app, Nexus::Graphics::CommandQueueHandle commandQueue);
	virtual ~DemoImGuiLayer() override = default;
	void OnImGuiRenderer() final;
	void SetDemoSelectedCallback(std::function<void(std::shared_ptr<Demos::Demo>)> function);

  private:
	void RenderDemoList();
	void RenderDemoInfo();
	void RenderPerformanceInfo();

	template<typename T>
	void RegisterDemo(const std::string &menuName, const std::string &name)
	{
		DemoInfo &info		  = m_DemoInfos[menuName].emplace_back();
		info.Name			  = name;
		info.CreationFunction = [](Nexus::Application						*app,
								   const std::string						&name,
								   Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer,
								   Nexus::Graphics::CommandQueueHandle		 commandQueue) -> Demos::Demo *
		{ return new T(name, app, imGuiRenderer, commandQueue); };
	}

  private:
	std::map<std::string, std::vector<DemoInfo>>	  m_DemoInfos		 = {};
	std::shared_ptr<Demos::Demo>					  m_CurrentDemo		 = nullptr;
	std::function<void(std::shared_ptr<Demos::Demo>)> m_CallbackFunction = {};
	Nexus::Graphics::IGraphicsDevice				 *m_GraphicsDevice	 = nullptr;
};