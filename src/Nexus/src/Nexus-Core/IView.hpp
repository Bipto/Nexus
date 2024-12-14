#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Timings/Timer.hpp"
#include "Nexus-Core/Utils/FramerateMonitor.hpp"

namespace Nexus
{
	class IView
	{
	  public:
		virtual void Update() = 0;
		virtual void Close()  = 0;

		virtual Point2D<uint32_t> GetSize() const		  = 0;
		virtual float			  GetDisplayScale() const = 0;

		virtual void SetRendersPerSecond(uint32_t amount) = 0;
		virtual void SetUpdatesPerSecond(uint32_t amount) = 0;
		virtual void SetTicksPerSecond(uint32_t amount)	  = 0;

	  public:
		EventHandler<const WindowResizedEventArgs &> OnResize;
		EventHandler<const WindowMovedEventArgs &>	 OnMove;

		EventHandler<> OnGainFocus;
		EventHandler<> OnLostFocus;
		EventHandler<> OnMaximized;
		EventHandler<> OnMinimized;
		EventHandler<> OnRestored;
		EventHandler<> OnShow;
		EventHandler<> OnHide;
		EventHandler<> OnExpose;

		EventHandler<const KeyPressedEventArgs &>  OnKeyPressed;
		EventHandler<const KeyReleasedEventArgs &> OnKeyReleased;

		EventHandler<const TextInputEventArgs &> OnTextInput;
		EventHandler<const TextEditEventArgs &>	 OnTextEdit;

		EventHandler<const MouseButtonPressedEventArgs &>  OnMousePressed;
		EventHandler<const MouseButtonReleasedEventArgs &> OnMouseReleased;
		EventHandler<const MouseMovedEventArgs &>		   OnMouseMoved;
		EventHandler<const MouseScrolledEventArgs &>	   OnScroll;
		EventHandler<>									   OnMouseEnter;
		EventHandler<>									   OnMouseLeave;

		EventHandler<const FileDropEventArgs &> OnFileDrop;

		EventHandler<TimeSpan> OnRender;
		EventHandler<TimeSpan> OnUpdate;
		EventHandler<TimeSpan> OnTick;

	  private:
	};
}	 // namespace Nexus