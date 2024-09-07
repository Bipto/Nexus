#include "Control.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::UI {
void Control::SetLocalPosition(const Point2D<float> position) {
  m_LocalPosition = position;
}

void Control::SetSize(const Point2D<float> size) { m_Size = size; }

void Nexus::UI::Control::SetFont(Graphics::Font *font) { m_Font = font; }

void Control::SetFontSize(uint32_t size) { m_FontSize = size; }

void Control::SetBackgroundColour(const glm::vec4 &colour) {
  m_BackgroundColour = colour;
}

void Control::SetForegroundColour(const glm::vec4 &colour) {
  m_ForegroundColour = colour;
}

void Control::SetAutoSize(bool enabled) { m_AutoSize = enabled; }

void Control::SetCornerRounding(const float rounding) {
  m_CornerRounding = rounding;
}

void Control::SetBorderThickness(const float thickness) {
  m_BorderThickness = thickness;
}

void Control::SetBorderColour(const glm::vec4 &colour) {
  m_BorderColour = colour;
}

void Control::SetPadding(const Padding &padding) { m_Padding = padding; }

const Point2D<float> Control::GetLocalPosition() const {
  return m_LocalPosition;
}

const Point2D<float> Control::GetScreenSpacePosition() const {
  Point2D<float> pos = GetLocalPosition();

  if (m_Parent) {
    Point2D<float> parentPos = m_Parent->GetScreenSpacePosition();
    pos.X += parentPos.X;
    pos.Y += parentPos.Y;
  }

  return pos;
}

const Point2D<float> &Control::GetSize() const { return m_Size; }

const Graphics::Font *const Nexus::UI::Control::GetFont() const {
  return m_Font;
}

const uint32_t Control::GetFontSize() const { return m_FontSize; }
const glm::vec4 &Control::GetBackgroundColour() const {
  return m_BackgroundColour;
}

const glm::vec4 &Control::GetForegroundColour() const {
  return m_ForegroundColour;
}

const Canvas *const Control::GetCanvas() const {
  if (m_Parent) {
    return m_Parent->GetCanvas();
  }

  return m_Canvas;
}

const bool Control::IsAutoSized() const { return m_AutoSize; }

const float Control::GetCornerRounding() const { return m_CornerRounding; }

float Control::GetBorderThickness() const { return m_BorderThickness; }

const glm::vec4 &Control::GetBorderColour() const { return m_BorderColour; }

const Padding &Control::GetPadding() const { return m_Padding; }

Graphics::RoundedRectangle Control::GetRoundedRectangle() const {
  auto [x, y] = GetScreenSpacePosition();
  return Graphics::RoundedRectangle(x, y, m_Size.X, m_Size.Y, m_CornerRounding,
                                    m_CornerRounding, m_CornerRounding,
                                    m_CornerRounding);
}

const Graphics::Rectangle<float> Control::GetControlBounds() const {
  Point2D<float> pos = GetScreenSpacePosition();

  return Graphics::Rectangle<float>(
      pos.X, pos.Y, m_Size.X + m_Padding.Left + m_Padding.Right,
      m_Size.Y + m_Padding.Top + m_Padding.Bottom);
}

const Graphics::Rectangle<float> Control::GetContentBounds() const {
  Graphics::Rectangle<float> bounds = GetControlBounds();
  bounds.SetX(bounds.GetLeft() + (m_CornerRounding / 2) + m_Padding.Left);
  bounds.SetY(bounds.GetTop() + (m_CornerRounding / 2) + m_Padding.Top);
  bounds.SetWidth(bounds.GetWidth() + m_CornerRounding + m_Padding.Right);
  bounds.SetHeight(bounds.GetHeight() + m_CornerRounding + m_Padding.Bottom);
  return bounds;
}

const Graphics::Scissor Control::GetScissorRectangle() const {
  Nexus::Graphics::Scissor scissor{};

  if (m_Canvas) {
    scissor.X = 0;
    scissor.Y = 0;
    scissor.Width = m_Canvas->GetSize().X;
    scissor.Height = m_Canvas->GetSize().Y;
  }

  return scissor;
}

const std::vector<glm::vec2> Control::GetDrawableArea() const {
  const Nexus::Point2D<float> &screenSpacePosition = GetScreenSpacePosition();
  const Nexus::Point2D<float> &size = GetSize();

  Nexus::Graphics::RoundedRectangle rrect(
      screenSpacePosition.X, screenSpacePosition.Y, m_Size.X, m_Size.Y,
      m_CornerRounding, m_CornerRounding, m_CornerRounding, m_CornerRounding);
  std::vector<glm::vec2> points = rrect.CreateOutline();

  if (m_Parent) {
    points = Nexus::Utils::SutherlandHodgman(points, GetDrawableArea());
  }

  return points;
}

void Control::AddControl(Control *control) {
  control->m_Parent = this;
  control->SetCanvas(m_Canvas);
  m_Controls.push_back(control);
}

void Control::RemoveControl(Control *control) {
  m_Controls.erase(std::remove(m_Controls.begin(), m_Controls.end(), control),
                   m_Controls.end());
}

const std::vector<Control *> Control::GetControls() const { return m_Controls; }

void Control::SetCanvas(Canvas *canvas) {
  m_Canvas = canvas;

  for (auto &child : m_Controls) {
    child->SetCanvas(canvas);
  }
}
} // namespace Nexus::UI