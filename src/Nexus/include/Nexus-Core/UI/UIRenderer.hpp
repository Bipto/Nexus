#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Renderer/BatchRenderer.hpp"

#include "Nexus-Core/UI/Control.hpp"

namespace Nexus::UI
{
	class UIRenderer
	{
	  public:
		UIRenderer(Graphics::GraphicsDevice *device) : m_Device(device)
		{
			m_BatchRenderer =
				std::make_unique<Graphics::BatchRenderer>(device, Nexus::Graphics::RenderTarget(Nexus::GetApplication()->GetPrimarySwapchain()));
		}
		virtual ~UIRenderer()
		{
		}

		inline void Render(Control *root)
		{
			IWindow *window = Nexus::GetApplication()->GetPrimaryWindow();

			Nexus::Graphics::Viewport vp;
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= window->GetWindowSize().X;
			vp.Height	= window->GetWindowSize().Y;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;

			Nexus::Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = window->GetWindowSize().X;
			scissor.Height = window->GetWindowSize().Y;

			m_BatchRenderer->Begin(vp, scissor);

			RenderControl(m_BatchRenderer.get(), root);

			m_BatchRenderer->End();
		}

	  private:
		void RenderControl(Graphics::BatchRenderer *renderer, Control *control)
		{
			renderer->DrawPolygonFill(control->GetPolygon(), control->GetBackgroundColour());
			//  renderer->DrawRoundedRectangleFill(control->GetBounds(), control->GetBackgroundColour());

			const std::vector<Control *> children = control->GetChildren();
			for (Control *child : children)
			{
				int x = 0;
				RenderControl(renderer, child);
			}
		}

	  private:
		Graphics::GraphicsDevice				*m_Device		 = nullptr;
		std::unique_ptr<Graphics::BatchRenderer> m_BatchRenderer = nullptr;
	};
}	 // namespace Nexus::UI