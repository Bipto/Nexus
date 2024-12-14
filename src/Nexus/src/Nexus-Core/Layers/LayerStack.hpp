#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Layer.hpp"
#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"

namespace Nexus
{
	class LayerStack
	{
	  public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);
		void PopLayer(Layer *layer);
		void PopOverlay(Layer *overlay);

		void OnRender(TimeSpan time);
		void OnUpdate(TimeSpan time);
		void OnTick(TimeSpan time);

		void OnWindowResize(const WindowResizedEventArgs &args);
		void OnWindowMoved(const WindowMovedEventArgs &args);
		void OnFocusGain();
		void OnFocusLost();
		void OnMaximize();
		void OnMinimize();
		void OnRestore();
		void OnShow();
		void OnHide();
		void OnExpose();
		void OnKeyPressed(const KeyPressedEventArgs &args);
		void OnKeyReleased(const KeyReleasedEventArgs &args);
		void OnTextInput(const TextInputEventArgs &args);
		void OnTextEdit(const TextEditEventArgs &args);
		void OnMouseButtonPressed(const MouseButtonPressedEventArgs &args);
		void OnMouseButtonReleased(const MouseButtonReleasedEventArgs &args);
		void OnMouseMoved(const MouseMovedEventArgs &args);
		void OnScroll(const MouseScrolledEventArgs &args);
		void OnMouseEnter();
		void OnMouseLeave();

		std::vector<Layer *>::iterator				 begin();
		std::vector<Layer *>::iterator				 end();
		std::vector<Layer *>::reverse_iterator		 rbegin();
		std::vector<Layer *>::reverse_iterator		 rend();
		std::vector<Layer *>::const_iterator		 begin() const;
		std::vector<Layer *>::const_iterator		 end() const;
		std::vector<Layer *>::const_reverse_iterator rbegin() const;
		std::vector<Layer *>::const_reverse_iterator rend() const;

		void DispatchEvent(std::function<void(Layer *layer)> function);

	  private:
		std::vector<Layer *> m_Layers;
		uint32_t			 m_LayerInsertIndex = 0;
	};
}	 // namespace Nexus