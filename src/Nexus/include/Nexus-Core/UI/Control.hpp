#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/RoundedRectangle.hpp"
#include "Nexus-Core/UI/DockMode.hpp"
#include "Nexus-Core/UI/Style.hpp"

namespace Nexus::UI
{
	class NX_API Control
	{
	  public:
		Control();
		virtual ~Control();

		void SetBackgroundColour(const glm::vec4 &backgroundColour);
		void SetForegroundColour(const glm::vec4 &foregroundColour);
		void SetPosition(const Nexus::Point2D<uint32_t> &position);
		void SetSize(const Nexus::Point2D<uint32_t> &size);
		void SetRounding(float rounding);
		void SetRounding(float topLeft, float topRight, float bottomLeft, float bottomRight);

		void AddChild(Control *control);
		void RemoveChild(Control *control);

		const Style					 &GetStyle() const;
		DockMode					  GetDockMode() const;
		Nexus::Point2D<uint32_t>	  GetPosition() const;
		Nexus::Point2D<uint32_t>	  GetScreenSpacePosition() const;
		Nexus::Point2D<uint32_t>	  GetSize() const;
		Graphics::RoundedRectangle	  GetBounds() const;
		const std::vector<Control *> &GetChildren() const;
		Control						 *GetParent() const;
		const std::vector<glm::vec2> &GetOutline() const;
		Graphics::Polygon			  GetPolygon() const;

		void SetStyle(const Style &style);
		void SetDockMode(DockMode dockMode);

	  private:
		virtual void OnResize(uint32_t width, uint32_t height);
		void		 CalculateBounds();
		void		 SetParent(Control *parent);

	  protected:
		Nexus::Point2D<uint32_t> m_Position = {0, 0};
		Nexus::Point2D<uint32_t> m_Size		= {0, 0};

		Style	 m_Style	= {};
		DockMode m_DockMode = DockMode::None;

		Graphics::RoundedRectangle m_Bounds	 = {};
		std::vector<glm::vec2>	   m_Outline = {};

		Control				  *m_Parent	  = nullptr;
		std::vector<Control *> m_Children = {};
	};
}	 // namespace Nexus::UI