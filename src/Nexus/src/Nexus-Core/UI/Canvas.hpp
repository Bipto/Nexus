#pragma once

#include "Nexus-Core/Layers/Layer.hpp"
#include "Nexus.hpp"

#include "Nexus-Core/Renderer/BatchRenderer.hpp"

namespace Nexus
{
	class Canvas : public Layer
	{
	  public:
		Canvas(Graphics::GraphicsDevice *device) : m_Device(device)
		{
			m_BatchRenderer = std::unique_ptr<Nexus::Graphics::BatchRenderer>(
			new Nexus::Graphics::BatchRenderer(device, Nexus::Graphics::RenderTarget(device->GetPrimaryWindow()->GetSwapchain())));
		}

		virtual ~Canvas()
		{
		}

		virtual void OnRender(TimeSpan time) override
		{
			auto [width, height] = m_Device->GetPrimaryWindow()->GetWindowSize();

			Graphics::Viewport vp;
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= width;
			vp.Height	= height;
			vp.MinDepth = 0;
			vp.MaxDepth = 1;

			Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = width;
			scissor.Height = height;

			m_BatchRenderer->Begin(vp, scissor);

			Graphics::Rectangle<float> rect(0, 0, width, height);
			m_BatchRenderer->DrawQuadFill(rect, {1.0f, 0.0f, 0.0f, 1.0f});

			m_BatchRenderer->End();
		}

	  private:
		Graphics::GraphicsDevice					   *m_Device		= nullptr;
		std::unique_ptr<Nexus::Graphics::BatchRenderer> m_BatchRenderer = nullptr;
	};
}	 // namespace Nexus