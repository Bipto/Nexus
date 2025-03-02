#pragma once

#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Timings/Timer.hpp"

namespace Nexus
{
	class Layer
	{
	  public:
		Layer()
		{
		}
		virtual ~Layer()
		{
		}

		virtual void OnAttach()
		{
		}

		virtual void OnDetach()
		{
		}

		virtual void OnUpdate(TimeSpan time)
		{
		}

		virtual void OnRender(TimeSpan time)
		{
		}

		virtual void OnTick(TimeSpan time)
		{
		}

		virtual void OnWindowResize(const WindowResizedEventArgs &args)
		{
		}
		virtual void OnWindowMoved(const WindowMovedEventArgs &args)
		{
		}
		virtual void OnFocusGain()
		{
		}
		virtual void OnFocusLost()
		{
		}
		virtual void OnMaximize()
		{
		}
		virtual void OnMinimize()
		{
		}
		virtual void OnRestore()
		{
		}
		virtual void OnShow()
		{
		}
		virtual void OnHide()
		{
		}
		virtual void OnExpose()
		{
		}
		virtual void OnKeyPressed(const KeyPressedEventArgs &args)
		{
		}
		virtual void OnKeyReleased(const KeyReleasedEventArgs &args)
		{
		}
		virtual void OnTextInput(const TextInputEventArgs &args)
		{
		}
		virtual void OnTextEdit(const TextEditEventArgs &args)
		{
		}
		virtual void OnMouseButtonPressed(const MouseButtonPressedEventArgs &args)
		{
		}
		virtual void OnMouseButtonReleased(const MouseButtonReleasedEventArgs &args)
		{
		}
		virtual void OnMouseMoved(const MouseMovedEventArgs &args)
		{
		}
		virtual void OnScroll(const MouseScrolledEventArgs &args)
		{
		}
		virtual void OnMouseEnter()
		{
		}
		virtual void OnMouseLeave()
		{
		}
	};
}	 // namespace Nexus