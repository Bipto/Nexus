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
            m_CommandList = m_GraphicsDevice->CreateCommandList();
            m_BatchRenderer = new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
            r1 = Nexus::Graphics::RoundedRectangle({450, 400}, {250, 250}, 5.0f, 5.0f, 5.0f, 5.0f);
            r1.SetPointsPerCorner(2);
            r2 = Nexus::Graphics::RoundedRectangle({350, 400}, {400, 400}, 25.0f, 25.0f, 25.0f, 25.0f);
            r2.SetPointsPerCorner(2);
        }

        virtual ~ClippingAndTriangulationDemo()
        {
        }

        virtual void Update(Nexus::Time time) override
        {
            if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::KeyLeft))
            {
                r1.SetX(r1.GetLeft() - 5);
            }

            if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::KeyRight))
            {
                r1.SetX(r1.GetLeft() + 5);
            }

            if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::KeyUp))
            {
                r1.SetY(r1.GetTop() - 5);
            }

            if (Nexus::Input::IsKeyHeld(Nexus::KeyCode::KeyDown))
            {
                r1.SetY(r1.GetTop() + 5);
            }
        }

        void DrawPolygon(const std::vector<glm::vec2> &points, const glm::vec4 &color)
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

            m_BatchRenderer->DrawRoundedRectangleFill(r2, {1.0f, 0.0f, 0.0f, 1.0f});
            m_BatchRenderer->DrawRoundedRectangleFill(r1, {0.0f, 0.0f, 1.0f, 1.0f});

            std::vector<glm::vec2> clip = {{500, 100}, {600, 300}, {400, 300}};
            // std::vector<glm::vec2> clip = {{r2.GetLeft(), r2.GetTop()}, {r2.GetRight(), r2.GetTop()}, {r2.GetRight(), r2.GetBottom()}, {r2.GetLeft(), r2.GetBottom()}};
            DrawPolygon(clip, {1.0f, 0.0f, 0.0f, 1.0f});

            std::vector<glm::vec2> rectPoints = Nexus::Utils::SutherlandHodgman(r1.CreateBorder(), clip);

            if (rectPoints.size() > 0)
            {
                std::vector<uint32_t> rectIndices;
                Nexus::Utils::Triangulate(rectPoints, rectIndices);

                if (rectIndices.size() > 0)
                {
                    std::vector<Nexus::Graphics::Triangle2D> triangles;
                    for (size_t i = 0; i < rectIndices.size(); i += 3)
                    {
                        Nexus::Graphics::Triangle2D tri;
                        tri.A = rectPoints[rectIndices[i]];
                        tri.B = rectPoints[rectIndices[(i + 1) % rectIndices.size()]];
                        tri.C = rectPoints[rectIndices[(i + 2) % rectIndices.size()]];
                        triangles.push_back(tri);
                    }

                    Nexus::Graphics::Polygon polygon(triangles);
                    m_BatchRenderer->DrawPolygon(polygon, {0.0f, 1.0f, 1.0f, 1.0f});
                }
            }

            std::vector<glm::vec2> poly = {{300, 200}, {200, 250}, {100, 150}};
            std::vector<glm::vec2> clipTri = {{200, 100}, {300, 300}, {100, 300}};

            std::vector<glm::vec2> clippedPoints = Nexus::Utils::SutherlandHodgman(poly, clipTri);
            std::vector<Nexus::Graphics::Triangle2D> triangles;

            std::vector<uint32_t> indices;
            Nexus::Utils::Triangulate(clippedPoints, indices);

            for (size_t i = 0; i < indices.size(); i += 3)
            {
                Nexus::Graphics::Triangle2D tri;
                tri.A = clippedPoints[indices[i]];
                tri.B = clippedPoints[indices[(i + 1) % indices.size()]];
                tri.C = clippedPoints[indices[(i + 2) % indices.size()]];
                triangles.push_back(tri);
            }

            DrawPolygon(poly, {0.0f, 1.0f, 0.0f, 1.0f});
            DrawPolygon(clipTri, {1.0f, 0.0f, 0.0f, 1.0f});

            Nexus::Graphics::Polygon polygon(triangles);
            m_BatchRenderer->DrawPolygon(polygon, {0.0f, 0.0f, 1.0f, 1.0f});
            DrawPolygon(clippedPoints, {1.0f, 1.0f, 0.0f, 1.0f});

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

        Nexus::Graphics::RoundedRectangle r1;
        Nexus::Graphics::RoundedRectangle r2;
    };
}