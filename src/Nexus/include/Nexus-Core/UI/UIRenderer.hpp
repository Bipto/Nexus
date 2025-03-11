#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Renderer/BatchRenderer.hpp"

#include "Nexus-Core/UI/Control.hpp"

namespace Nexus::UI
{
	class NX_API UIRenderer
	{
	  public:
		UIRenderer(Graphics::GraphicsDevice *device);
		virtual ~UIRenderer();

		void Render(Control *root);

	  private:
		void RenderControl(Graphics::BatchRenderer *renderer, Control *control);

	  private:
		Graphics::GraphicsDevice				*m_Device		 = nullptr;
		std::unique_ptr<Graphics::BatchRenderer> m_BatchRenderer = nullptr;
	};
}	 // namespace Nexus::UI