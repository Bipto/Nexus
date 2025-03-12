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

	void Control::SetName(const std::string &name)
	{
		m_Name = name;
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

	const std::string &Control::GetName() const
	{
		return m_Name;
	}

	bool Control::ContainsMouse() const
	{
		return m_ContainsMouse;
	}

	void Control::SetMousePressedCallback(std::function<void(const MouseButtonPressedEventArgs &, Control *)> func)
	{
		m_OnMousePressedFunc = func;
	}

	void Control::SetMouseReleasedCallback(std::function<void(const MouseButtonReleasedEventArgs &, Control *)> func)
	{
		m_OnMouseReleasedFunc = func;
	}

	void Control::SetMouseMovedCallback(std::function<void(const MouseMovedEventArgs &, Control *)> func)
	{
		m_OnMouseMovedFunc = func;
	}

	void Control::SetMouseScrollCallback(std::function<void(const MouseScrolledEventArgs &, Control *)> func)
	{
		m_OnMouseScrolledFunc = func;
	}

	void Control::SetMouseEnterCallback(std::function<void(Control *)> func)
	{
		m_OnMouseEnterFunc = func;
	}

	void Control::SetMouseLeaveCallback(std::function<void(Control *)> func)
	{
		m_OnMouseLeaveFunc = func;
	}

	void Control::OnMousePressed(const MouseButtonPressedEventArgs &args)
	{
	}

	void Control::OnMouseReleased(const MouseButtonReleasedEventArgs &args)
	{
	}

	void Control::OnMouseMoved(const MouseMovedEventArgs &args)
	{
	}

	void Control::OnMouseScroll(const MouseScrolledEventArgs &args)
	{
	}

	void Control::OnMouseEnter()
	{
	}

	void Control::OnMouseLeave()
	{
	}

	void Control::InvokeOnMousePressed(const MouseButtonPressedEventArgs &args)
	{
		Graphics::Polygon polygon = GetPolygon();
		if (polygon.Contains(args.Position))
		{
			OnMousePressed(args);

			if (m_OnMousePressedFunc)
			{
				m_OnMousePressedFunc(args, this);
			}

			IterateChildren([&](Control *control) { control->InvokeOnMousePressed(args); });
		}
	}

	void Control::InvokeOnMouseReleased(const MouseButtonReleasedEventArgs &args)
	{
		Graphics::Polygon polygon = GetPolygon();
		if (polygon.Contains(args.Position))
		{
			OnMouseReleased(args);

			if (m_OnMouseReleasedFunc)
			{
				m_OnMouseReleasedFunc(args, this);
			}

			IterateChildren([&](Control *control) { control->InvokeOnMouseReleased(args); });
		}
	}

	void Control::InvokeOnMouseMoved(const MouseMovedEventArgs &args)
	{
		Graphics::Polygon polygon = GetPolygon();
		if (polygon.Contains(args.Position))
		{
			OnMouseMoved(args);

			if (m_OnMouseMovedFunc)
			{
				m_OnMouseMovedFunc(args, this);
			}

			if (!m_ContainsMouse)
			{
				if (m_OnMouseEnterFunc)
				{
					m_OnMouseEnterFunc(this);
				}
				OnMouseEnter();
				m_ContainsMouse = true;
			}

			IterateChildren([&](Control *control) { control->InvokeOnMouseMoved(args); });
		}
		else
		{
			if (m_ContainsMouse)
			{
				if (m_OnMouseLeaveFunc)
				{
					m_OnMouseLeaveFunc(this);
				}
				OnMouseLeave();
				m_ContainsMouse = false;
			}
		}
	}

	void Control::InvokeOnMouseScroll(const MouseScrolledEventArgs &args)
	{
		Graphics::Polygon polygon = GetPolygon();
		if (polygon.Contains(args.Position))
		{
			OnMouseScroll(args);
			if (m_OnMouseScrolledFunc)
			{
				m_OnMouseScrolledFunc(args, this);
			}

			IterateChildren([&](Control *control) { control->InvokeOnMouseScroll(args); });
		}
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

	void Control::IterateChildren(std::function<void(Control *)> func)
	{
		for (Control *control : m_Children) { func(control); }
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
			const std::vector<glm::vec2> &points = m_Parent->GetOutline();
			if (points.size() > 3)
			{
				m_Outline = Nexus::Utils::SutherlandHodgman(m_Outline, m_Parent->GetOutline());
			}
		}
	}

	void Control::SetParent(Control *parent)
	{
		m_Parent = parent;
		CalculateBounds();
	}
}	 // namespace Nexus::UI