#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/RoundedRectangle.hpp"

namespace Nexus::UI
{
	struct Rounding
	{
		float TopLeft	  = 0.0f;
		float TopRight	  = 0.0f;
		float BottomLeft  = 0.0f;
		float BottomRight = 0.0f;
	};

	class Control
	{
	  public:
		Control()
		{
		}

		virtual ~Control()
		{
			for (auto child : m_Children) { delete child; }
			m_Children.clear();
		}

		inline void SetBackgroundColour(const glm::vec4 &backgroundColour)
		{
			m_BackgroundColour = backgroundColour;
		}

		inline void SetForegroundColour(const glm::vec4 &foregroundColour)
		{
			m_ForegroundColour = foregroundColour;
		}

		inline void SetPosition(const Nexus::Point2D<uint32_t> &position)
		{
			m_Position = position;
			CalculateBounds();
		}

		inline void SetSize(const Nexus::Point2D<uint32_t> &size)
		{
			m_Size = size;
			CalculateBounds();
		}

		inline void SetRounding(float rounding)
		{
			SetRounding(rounding, rounding, rounding, rounding);
		}

		inline void SetRounding(float topLeft, float topRight, float bottomLeft, float bottomRight)
		{
			m_Rounding.TopLeft	   = topLeft;
			m_Rounding.TopRight	   = topRight;
			m_Rounding.BottomLeft  = bottomLeft;
			m_Rounding.BottomRight = bottomRight;
			CalculateBounds();
		}

		inline void AddChild(Control *control)
		{
			control->SetParent(this);
			m_Children.push_back(control);
			control->CalculateBounds();
		}

		inline void RemoveChild(Control *control)
		{
			m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), control), m_Children.end());
		}

		inline glm::vec4 GetBackgroundColour() const
		{
			return m_BackgroundColour;
		}

		inline glm::vec4 GetForegroundColour() const
		{
			return m_ForegroundColour;
		}

		inline Nexus::Point2D<uint32_t> GetPosition() const
		{
			return m_Position;
		}

		inline Nexus::Point2D<uint32_t> GetSize() const
		{
			return m_Size;
		}

		inline Rounding GetRounding() const
		{
			return m_Rounding;
		}

		inline Graphics::RoundedRectangle GetBounds() const
		{
			return m_Bounds;
		}

		inline const std::vector<Control *> &GetChildren() const
		{
			return m_Children;
		}

		inline Control *GetParent() const
		{
			return m_Parent;
		}

		inline const std::vector<glm::vec2> &GetOutline() const
		{
			return m_Outline;
		}

		inline Graphics::Polygon GetPolygon() const
		{
			return Nexus::Utils::GeneratePolygon(m_Outline);
		}

	  private:
		inline void CalculateBounds()
		{
			m_Bounds = Graphics::RoundedRectangle(m_Position.To<float>(),
												  m_Size.To<float>(),
												  m_Rounding.TopLeft,
												  m_Rounding.TopRight,
												  m_Rounding.BottomLeft,
												  m_Rounding.BottomRight);
			m_Bounds.SetPointsPerCorner(8);
			m_Outline = m_Bounds.CreateOutline();

			if (m_Parent)
			{
				m_Outline = Nexus::Utils::SutherlandHodgman(m_Outline, m_Parent->GetOutline());
			}
		}

		inline void SetParent(Control *parent)
		{
			m_Parent = parent;
			CalculateBounds();
		}

	  private:
		glm::vec4				   m_BackgroundColour = {1, 1, 1, 1};
		glm::vec4				   m_ForegroundColour = {0, 0, 0, 1};
		Nexus::Point2D<uint32_t>   m_Position		  = {0, 0};
		Nexus::Point2D<uint32_t>   m_Size			  = {0, 0};
		Rounding				   m_Rounding		  = {};
		Graphics::RoundedRectangle m_Bounds			  = {};
		std::vector<glm::vec2>	   m_Outline		  = {};
		Control					  *m_Parent			  = nullptr;
		std::vector<Control *>	   m_Children		  = {};
	};
}	 // namespace Nexus::UI