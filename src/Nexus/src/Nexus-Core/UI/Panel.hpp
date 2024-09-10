#pragma once

#include "Control.hpp"

namespace Nexus::UI
{
	class Panel : public Control
	{
	  public:
		virtual void OnUpdate() override;
		virtual void OnRender(Nexus::Graphics::BatchRenderer *renderer) override;
		virtual void OnAutoSize() override;
		virtual void HandleMouseClick(const MouseClick &e) override;
	};
}	 // namespace Nexus::UI