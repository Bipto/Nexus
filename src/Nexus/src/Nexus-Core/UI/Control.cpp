#include "Nexus-Core/UI/Control.hpp"

namespace Nexus::UI
{
	Control::Control()
	{
	}

	Control::~Control()
	{
		for (auto child : m_Children) { delete child; }
		m_Children.clear();
	}

	void Control::SetBackgroundColour(const glm::vec4 &backgroundColour)
	{
		m_Style.BackgroundColour = backgroundColour;
	}

	void Control::SetForegroundColour(const glm::vec4 &foregroundColour)
	{
		m_Style.ForegroundColour = foregroundColour;
	}

	void Control::SetPosition(const Nexus::Point2D<uint32_t> &position)
	{
		m_Position = position;
		CalculateBounds();
		OnResize(m_Size.X, m_Size.Y);
	}

	void Control::SetSize(const Nexus::Point2D<uint32_t> &size)
	{
		m_Size = size;
		CalculateBounds();
		OnResize(size.X, size.Y);
	}

	void Control::SetRounding(float rounding)
	{
		SetRounding(rounding, rounding, rounding, rounding);
	}

	void Control::SetRounding(float topLeft, float topRight, float bottomLeft, float bottomRight)
	{
		m_Style.CornerRounding.TopLeft	   = topLeft;
		m_Style.CornerRounding.TopRight	   = topRight;
		m_Style.CornerRounding.BottomLeft  = bottomLeft;
		m_Style.CornerRounding.BottomRight = bottomRight;
		CalculateBounds();
	}

	void Control::AddChild(Control *control)
	{
		control->SetParent(this);
		m_Children.push_back(control);
		control->CalculateBounds();
	}

	void Control::RemoveChild(Control *control)
	{
		m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), control), m_Children.end());
	}

	const Style &Control::GetStyle() const
	{
		return m_Style;
	}

	DockMode Control::GetDockMode() const
	{
		return m_DockMode;
	}

	Nexus::Point2D<uint32_t> Control::GetPosition() const
	{
		return m_Position;
	}

	Nexus::Point2D<uint32_t> Control::GetScreenSpacePosition() const
	{
		Nexus::Point2D<uint32_t> screenSpacePos = GetPosition();

		if (m_Parent)
		{
			Nexus::Point2D<uint32_t> parentPos = m_Parent->GetScreenSpacePosition();
			screenSpacePos.X += parentPos.X;
			screenSpacePos.Y += parentPos.Y;
		}

		return screenSpacePos;
	}

	Nexus::Point2D<uint32_t> Control::GetSize() const
	{
		return m_Size;
	}

	Graphics::RoundedRectangle Control::GetBounds() const
	{
		return m_Bounds;
	}

	const std::vector<Control *> &Control::GetChildren() const
	{
		return m_Children;
	}

	Control *Control::GetParent() const
	{
		return m_Parent;
	}

	const std::vector<glm::vec2> &Control::GetOutline() const
	{
		return m_Outline;
	}

	Graphics::Polygon Control::GetPolygon() const
	{
		return Nexus::Utils::GeneratePolygon(m_Outline);
	}

	void Control::SetStyle(const Style &style)
	{
		m_Style = style;
	}

	void Control::SetDockMode(DockMode dockMode)
	{
		m_DockMode = dockMode;
	}

	void Control::OnResize(uint32_t width, uint32_t height)
	{
	}

	void Control::CalculateBounds()
	{
		Nexus::Point2D<uint32_t> screenSpacePos = GetScreenSpacePosition();

		m_Bounds = Graphics::RoundedRectangle(screenSpacePos.To<float>(),
											  m_Size.To<float>(),
											  m_Style.CornerRounding.TopLeft,
											  m_Style.CornerRounding.TopRight,
											  m_Style.CornerRounding.BottomLeft,
											  m_Style.CornerRounding.BottomRight);
		m_Bounds.SetPointsPerCorner(8);
		m_Outline = m_Bounds.CreateOutline();

		if (m_Parent)
		{
			m_Outline = Nexus::Utils::SutherlandHodgman(m_Outline, m_Parent->GetOutline());
		}
	}

	void Control::SetParent(Control *parent)
	{
		m_Parent = parent;
		CalculateBounds();
	}
}	 // namespace Nexus::UI