#pragma once

#pragma once

#include "Demo.hpp"

#include "Nexus/Graphics/CatmullRom.hpp"

namespace Demos
{
    class Splines : public Demo
    {
    public:
        Splines(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
            : Demo(name, app, imGuiRenderer)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();
            m_BatchRenderer = new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});

            m_Spline.SetPoints({{100, 410}, {400, 410}, {700, 410}, {1000, 410}});
            // m_Spline.SetPoints({{{100, 410}, {200, 410}, {300, 410}, {400, 410}, {500, 410}, {600, 410}, {700, 410}, {800, 410}, {900, 410}, {1000, 410}}});
            m_Spline.SetLooped(true);
        }

        virtual ~Splines()
        {
        }

        virtual void Update(Nexus::Time time) override
        {
            if (Nexus::Input::IsKeyReleased(Nexus::KeyCode::KeyLeft))
            {
                if (m_SelectedPoint == 0)
                {
                    m_SelectedPoint = m_Spline.GetPoints().size();
                }

                m_SelectedPoint--;
            }

            if (Nexus::Input::IsKeyReleased(Nexus::KeyCode::KeyRight))
            {
                m_SelectedPoint++;

                if (m_SelectedPoint == m_Spline.GetPoints().size())
                {
                    m_SelectedPoint = 0;
                }
            }

            std::vector<Nexus::Point2D<float>> points = m_Spline.GetPoints();
            Nexus::Point2D<float> &point = points.at(m_SelectedPoint);
            if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::W))
            {
                point.Y -= 250.0f * time.GetSeconds();
            }

            if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::S))
            {
                point.Y += 250.0f * time.GetSeconds();
            }

            if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::A))
            {
                point.X -= 250.0f * time.GetSeconds();
            }

            if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::D))
            {
                point.X += 250.0f * time.GetSeconds();
            }

            m_Spline.SetPoints(points);
        }

        virtual void Render(Nexus::Time time) override
        {
            m_CommandList->Begin();
            m_CommandList->SetRenderTarget({m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
            m_CommandList->ClearColorTarget(0, {m_ClearColour.r,
                                                m_ClearColour.g,
                                                m_ClearColour.b,
                                                1.0f});
            m_CommandList->End();
            m_GraphicsDevice->SubmitCommandList(m_CommandList);

            const auto &windowSize = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize();

            Nexus::Graphics::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = windowSize.X;
            vp.Height = windowSize.Y;
            vp.MinDepth = 0;
            vp.MaxDepth = 1;

            Nexus::Graphics::Scissor scissor;
            scissor.X = 0;
            scissor.Y = 0;
            scissor.Width = windowSize.X;
            scissor.Height = windowSize.Y;

            m_BatchRenderer->Begin(vp, scissor);

            // render splines
            {
                const auto &points = m_Spline.GetPoints();

                for (int i = 0; i < points.size(); i++)
                {
                    Nexus::Graphics::Circle<float> circle(points.at(i), 25);
                    m_BatchRenderer->DrawCircleFill(circle, {1.0f, 1.0f, 1.0f, 1.0f}, 16);
                }

                {
                    const auto &selectedPoint = points.at(m_SelectedPoint);
                    Nexus::Graphics::Circle<float> circle(selectedPoint, 25);
                    m_BatchRenderer->DrawCircleFill(circle, {1.0f, 1.0f, 0.0f, 1.0f}, 16);
                }

                for (float t = 0.0f; t < m_Spline.GetNumberOfPoints() - 0.05f; t += 0.05f)
                {
                    float t0 = t;
                    float t1 = t + 0.05f;

                    Nexus::Point2D<float> pos0 = m_Spline.GetPoint(t0);
                    Nexus::Point2D<float> pos1 = m_Spline.GetPoint(t1);

                    m_BatchRenderer->DrawLine({pos0.X, pos0.Y}, {pos1.X, pos1.Y}, {1.0f, 0.0f, 0.0f, 1.0f}, 5.0f);
                }
            }

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
        glm::vec3 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

        Nexus::Graphics::BatchRenderer *m_BatchRenderer = nullptr;
        Nexus::Graphics::CatmullRom<float> m_Spline;
        int m_SelectedPoint = 0;
    };
}