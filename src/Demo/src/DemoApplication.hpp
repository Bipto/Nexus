#pragma once

#include "Nexus-Core/Application.hpp"

class DemoApplication final : public Nexus::Application
{
  public:
	DemoApplication(const Nexus::ApplicationDescription &desc);
	virtual ~DemoApplication() = default;
	void Load() final;
	void Update(Nexus::TimeSpan time) final;
	void Render(Nexus::TimeSpan time) final;
	void OnResize(Nexus::Point2D<uint32_t> size) final;
	void Unload() final;
};