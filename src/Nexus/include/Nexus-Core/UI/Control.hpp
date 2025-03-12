#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/RoundedRectangle.hpp"
#include "Nexus-Core/UI/DockMode.hpp"
#include "Nexus-Core/UI/Style.hpp"

#include "Nexus-Core/Input/Events.hpp"

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
		void SetName(const std::string &name);

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
		const std::string			 &GetName() const;
		bool						  ContainsMouse() const;

		void SetMousePressedCallback(std::function<void(const MouseButtonPressedEventArgs &, Control *)> func);
		void SetMouseReleasedCallback(std::function<void(const MouseButtonReleasedEventArgs &, Control *)> func);
		void SetMouseMovedCallback(std::function<void(const MouseMovedEventArgs &, Control *)> func);
		void SetMouseScrollCallback(std::function<void(const MouseScrolledEventArgs &, Control *)> func);
		void SetMouseEnterCallback(std::function<void(Control *)> func);
		void SetMouseLeaveCallback(std::function<void(Control *)> func);

		virtual void OnMousePressed(const MouseButtonPressedEventArgs &args);
		virtual void OnMouseReleased(const MouseButtonReleasedEventArgs &args);
		virtual void OnMouseMoved(const MouseMovedEventArgs &args);
		virtual void OnMouseScroll(const MouseScrolledEventArgs &args);
		virtual void OnMouseEnter();
		virtual void OnMouseLeave();

		void InvokeOnMousePressed(const MouseButtonPressedEventArgs &args);
		void InvokeOnMouseReleased(const MouseButtonReleasedEventArgs &args);
		void InvokeOnMouseMoved(const MouseMovedEventArgs &args);
		void InvokeOnMouseScroll(const MouseScrolledEventArgs &args);

		void SetStyle(const Style &style);
		void SetDockMode(DockMode dockMode);

	  private:
		virtual void OnResize(uint32_t width, uint32_t height);
		void		 IterateChildren(std::function<void(Control *)> func);

	  public:
		void CalculateBounds();

	  private:
		void SetParent(Control *parent);

	  protected:
		Nexus::Point2D<uint32_t> m_Position = {0, 0};
		Nexus::Point2D<uint32_t> m_Size		= {0, 0};

		Style	 m_Style	= {};
		DockMode m_DockMode = DockMode::None;

		Graphics::RoundedRectangle m_Bounds	 = {};
		std::vector<glm::vec2>	   m_Outline = {};

		Control				  *m_Parent		   = nullptr;
		std::vector<Control *> m_Children	   = {};
		std::string			   m_Name		   = "Control";
		bool				   m_ContainsMouse = false;

		std::function<void(const MouseButtonPressedEventArgs &, Control *)>	 m_OnMousePressedFunc  = {};
		std::function<void(const MouseButtonReleasedEventArgs &, Control *)> m_OnMouseReleasedFunc = {};
		std::function<void(const MouseMovedEventArgs &, Control *)>			 m_OnMouseMovedFunc	   = {};
		std::function<void(const MouseScrolledEventArgs &, Control *)>		 m_OnMouseScrolledFunc = {};
		std::function<void(Control *)>										 m_OnMouseEnterFunc	   = {};
		std::function<void(Control *)>										 m_OnMouseLeaveFunc	   = {};
	};
}	 // namespace Nexus::UI