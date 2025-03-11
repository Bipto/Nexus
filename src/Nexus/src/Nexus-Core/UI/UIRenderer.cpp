#include "Nexus-Core/UI/UIRenderer.hpp"

#include "Nexus-Core/Runtime.hpp"

namespace Nexus::UI
{
	UIRenderer::UIRenderer(Graphics::GraphicsDevice *device) : m_Device(device)
	{
		m_BatchRenderer =
			std::make_unique<Graphics::BatchRenderer>(device, Nexus::Graphics::RenderTarget(Nexus::GetApplication()->GetPrimarySwapchain()));
	}

	UIRenderer::~UIRenderer()
	{
	}

	void UIRenderer::Render(Control *root)
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

	void UIRenderer::RenderControl(Graphics::BatchRenderer *renderer, Control *control)
	{
		const Style &style = control->GetStyle();
		control->CalculateBounds();
		renderer->DrawPolygonFill(control->GetPolygon(), style.BackgroundColour);
		//  renderer->DrawRoundedRectangleFill(control->GetBounds(), control->GetBackgroundColour());

		const std::vector<Control *> children = control->GetChildren();
		for (Control *child : children)
		{
			int x = 0;
			RenderControl(renderer, child);
		}
	}
}	 // namespace Nexus::UI