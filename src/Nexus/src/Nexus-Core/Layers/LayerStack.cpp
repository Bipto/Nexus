#include "LayerStack.hpp"

namespace Nexus
{
	Nexus::LayerStack::~LayerStack()
	{
		for (Layer *layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer *layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
		layer->OnAttach();
	}

	void LayerStack::PushOverlay(Layer *overlay)
	{
		m_Layers.push_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer *layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (it != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer *overlay)
	{
		auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}

	void LayerStack::OnRender(TimeSpan time)
	{
		for (Layer *layer : m_Layers) { layer->OnRender(time); }
	}

	void LayerStack::OnUpdate(TimeSpan time)
	{
		for (Layer *layer : m_Layers) { layer->OnRender(time); }
	}

	void LayerStack::OnTick(TimeSpan time)
	{
		for (Layer *layer : m_Layers) { layer->OnTick(time); }
	}

	void LayerStack::OnWindowResize(const WindowResizedEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnWindowResize(args); });
	}

	void LayerStack::OnWindowMoved(const WindowMovedEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnWindowMoved(args); });
	}

	void LayerStack::OnFocusGain()
	{
		DispatchEvent([&](Layer *layer) { layer->OnFocusGain(); });
	}

	void LayerStack::OnFocusLost()
	{
		DispatchEvent([&](Layer *layer) { layer->OnFocusLost(); });
	}

	void LayerStack::OnMaximize()
	{
		DispatchEvent([&](Layer *layer) { layer->OnMaximize(); });
	}

	void LayerStack::OnMinimize()
	{
		DispatchEvent([&](Layer *layer) { layer->OnMinimize(); });
	}

	void LayerStack::OnRestore()
	{
		DispatchEvent([&](Layer *layer) { layer->OnRestore(); });
	}

	void LayerStack::OnShow()
	{
		DispatchEvent([&](Layer *layer) { layer->OnShow(); });
	}

	void LayerStack::OnHide()
	{
		DispatchEvent([&](Layer *layer) { layer->OnHide(); });
	}

	void LayerStack::OnExpose()
	{
		DispatchEvent([&](Layer *layer) { layer->OnExpose(); });
	}

	void LayerStack::OnKeyPressed(const KeyPressedEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnKeyPressed(args); });
	}

	void LayerStack::OnKeyReleased(const KeyReleasedEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnKeyReleased(args); });
	}

	void LayerStack::OnTextInput(const TextInputEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnTextInput(args); });
	}

	void LayerStack::OnTextEdit(const TextEditEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnTextEdit(args); });
	}

	void LayerStack::OnMouseButtonPressed(const MouseButtonPressedEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnMouseButtonPressed(args); });
	}

	void LayerStack::OnMouseButtonReleased(const MouseButtonReleasedEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnMouseButtonReleased(args); });
	}

	void LayerStack::OnMouseMoved(const MouseMovedEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnMouseMoved(args); });
	}

	void LayerStack::OnScroll(const MouseScrolledEventArgs &args)
	{
		DispatchEvent([&](Layer *layer) { layer->OnScroll(args); });
	}

	void LayerStack::OnMouseEnter()
	{
		DispatchEvent([&](Layer *layer) { layer->OnMouseEnter(); });
	}

	void LayerStack::OnMouseLeave()
	{
		DispatchEvent([&](Layer *layer) { layer->OnMouseLeave(); });
	}

	std::vector<Layer *>::iterator LayerStack::begin()
	{
		return m_Layers.begin();
	}

	std::vector<Layer *>::iterator LayerStack::end()
	{
		return m_Layers.end();
	}

	std::vector<Layer *>::reverse_iterator LayerStack::rbegin()
	{
		return m_Layers.rbegin();
	}

	std::vector<Layer *>::reverse_iterator LayerStack::rend()
	{
		return m_Layers.rend();
	}

	std::vector<Layer *>::const_iterator LayerStack::begin() const
	{
		return m_Layers.begin();
	}

	std::vector<Layer *>::const_iterator LayerStack::end() const
	{
		return m_Layers.end();
	}

	std::vector<Layer *>::const_reverse_iterator LayerStack::rbegin() const
	{
		return m_Layers.rbegin();
	}

	std::vector<Layer *>::const_reverse_iterator LayerStack::rend() const
	{
		return m_Layers.rend();
	}

	void LayerStack::DispatchEvent(std::function<void(Layer *layer)> function)
	{
		for (size_t i = m_Layers.size(); i--;)
		{
			Layer *layer = m_Layers[i];
			function(layer);
		}
	}
}	 // namespace Nexus
