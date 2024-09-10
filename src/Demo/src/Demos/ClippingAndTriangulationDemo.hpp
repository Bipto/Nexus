#pragma once

#pragma once

#include "Demo.hpp"

namespace Demos
{
	class ClippingAndTriangulationDemo : public Demo
	{
	  public:
		ClippingAndTriangulationDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~ClippingAndTriangulationDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
			m_BatchRenderer =
				new Nexus::Graphics::BatchRenderer(m_GraphicsDevice,
												   Nexus::Graphics::RenderTarget {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});

			r1 = Nexus::Graphics::RoundedRectangle({450, 400}, {250, 250}, 15.0f, 15.0f, 15.0f, 15.0f);
			r1.SetPointsPerCorner(8);
			r2 = Nexus::Graphics::RoundedRectangle({350, 400}, {400, 400}, 35.0f, 35.0f, 35.0f, 35.0f);
			r2.SetPointsPerCorner(8);
		}

		virtual void Update(Nexus::Time time) override
		{
			// move rectangle
			{
				if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::A))
				{
					r1.SetX(r1.GetLeft() - 5);
				}

				if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::D))
				{
					r1.SetX(r1.GetLeft() + 5);
				}

				if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::W))
				{
					r1.SetY(r1.GetTop() - 5);
				}

				if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::S))
				{
					r1.SetY(r1.GetTop() + 5);
				}
			}

			// select point
			{
				const auto &points = r2.CreateOutline();
				{
					if (Nexus::Input::IsKeyReleased(Nexus::KeyCode::KeyLeft))
					{
						if (m_SelectedPoint == 0)
						{
							m_SelectedPoint = points.size();
						}

						m_SelectedPoint--;
					}

					if (Nexus::Input::IsKeyReleased(Nexus::KeyCode::KeyRight))
					{
						m_SelectedPoint++;

						if (m_SelectedPoint == points.size())
						{
							m_SelectedPoint = 0;
						}
					}
				}
			}
		}

		void DrawPolygonFill(const std::vector<glm::vec2> &points, const glm::vec4 &color)
		{
			for (size_t i = 0; i < points.size(); i++)
			{
				glm::vec2 p1 = points[i];
				glm::vec2 p2 = points[(i + 1) % points.size()];

				m_BatchRenderer->DrawLine(p1, p2, color, 1.0f);
			}
		}

		virtual void Render(Nexus::Time time) override
		{
			m_CommandList->Begin();
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
			m_CommandList->End();
			m_GraphicsDevice->SubmitCommandList(m_CommandList);

			const auto &windowSize = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize();

			Nexus::Graphics::Viewport vp;
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= windowSize.X;
			vp.Height	= windowSize.Y;
			vp.MinDepth = 0;
			vp.MaxDepth = 1;

			Nexus::Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = windowSize.X;
			scissor.Height = windowSize.Y;

			m_BatchRenderer->Begin(vp, scissor);

			m_BatchRenderer->DrawRoundedRectangleFill(r2, {1.0f, 0.0f, 0.0f, 1.0f});
			m_BatchRenderer->DrawRoundedRectangleFill(r1, {0.0f, 0.0f, 1.0f, 1.0f});

			const auto				&points = r1.ClipAgainst(r2);
			Nexus::Graphics::Polygon poly	= Nexus::Utils::GeneratePolygon(points);
			m_BatchRenderer->DrawPolygonFill(poly, {0.0f, 1.0f, 0.0f, 1.0f});

			m_BatchRenderer->End();
		}

		virtual void OnResize(Nexus::Point2D<uint32_t> size) override
		{
		}

		virtual void RenderUI() override
		{
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		glm::vec3								 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

		Nexus::Graphics::BatchRenderer *m_BatchRenderer = nullptr;

		Nexus::Graphics::RoundedRectangle r1;
		Nexus::Graphics::RoundedRectangle r2;

		int m_SelectedPoint = 0;
	};
}	 // namespace Demos