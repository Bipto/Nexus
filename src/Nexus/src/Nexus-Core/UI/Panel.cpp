#include "Panel.hpp"

namespace Nexus::UI
{
void Panel::OnUpdate()
{
    for (Control *ctrl : m_Controls)
    {
        ctrl->OnUpdate();
    }
}

void Panel::OnRender(Graphics::BatchRenderer *renderer)
{
    const Canvas *canvas = GetCanvas();

    Nexus::Graphics::Viewport vp;
    vp.X = canvas->GetPosition().X;
    vp.Y = canvas->GetPosition().Y;
    vp.Width = canvas->GetSize().X;
    vp.Height = canvas->GetSize().Y;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    Nexus::Graphics::RoundedRectangle rrect = GetRoundedRectangle();
    std::vector<glm::vec2> points = rrect.CreateOutline();

    Nexus::Graphics::Scissor scissor = GetScissorRectangle();

    renderer->Begin(vp, scissor);

    if (m_Parent)
    {
        Graphics::RoundedRectangle parentRect = m_Parent->GetRoundedRectangle();
        points = rrect.ClipAgainst(parentRect);
    }

    auto poly = Nexus::Utils::GeneratePolygon(points);

    renderer->DrawPolygonFill(poly, m_BackgroundColour);

    renderer->End();

    for (Control *ctrl : m_Controls)
    {
        ctrl->OnRender(renderer);
    }
}

void Panel::OnAutoSize()
{
}

void Panel::HandleMouseClick(const MouseClick &e)
{
}
} // namespace Nexus::UI