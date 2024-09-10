#pragma once

#include "Panel.hpp"

namespace Editor
{
	class SceneHierarchy : public Panel
	{
	  public:
		virtual void OnLoad() override;
		virtual void OnRender() override;

	  private:
		static void RenderControls();
	};
}	 // namespace Editor