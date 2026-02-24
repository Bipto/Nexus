#pragma once

#include <concepts>

#include "Platform/Layers/Layer.hpp"

namespace Nexus
{
	template<typename T>
	concept LayerType = std::is_base_of_v<ILayer, T>;

	class LayerStack
	{
	  public:
		LayerStack()  = default;
		~LayerStack() = default;

		template<LayerType Layer, typename... Args>
		ILayer *AddLayer(Args &&...args)
		{
			auto	layer = std::make_unique<Layer>(std::forward<Args>(args)...);
			ILayer *ptr	  = layer.get();
			m_Layers.push_back(std::move(layer));
			return ptr;
		}

		void RemoveLayer(ILayer *layer)
		{
			auto it = std::find_if(m_Layers.begin(), m_Layers.end(), [layer](const std::unique_ptr<ILayer> &ptr) { return ptr.get() == layer; });

			if (it != m_Layers.end())
				m_Layers.erase(it);
		}

		template<LayerType Layer>
		void RemoveLayer()
		{
			auto it = std::find_if(m_Layers.begin(),
								   m_Layers.end(),
								   [](const std::unique_ptr<ILayer> &ptr) { return dynamic_cast<Layer *>(ptr.get()) != nullptr; });

			if (it != m_Layers.end())
				m_Layers.erase(it);
		}

		template<LayerType Layer, typename... Args>
		ILayer *AddOverlay(Args &&...args)
		{
			auto	layer = std::make_unique<Layer>(std::forward<Args>(args)...);
			ILayer *ptr	  = layer.get();
			m_Overlays.push_back(std::move(layer));
			return ptr;
		}

		void RemoveOverlay(ILayer *layer)
		{
			auto it = std::find_if(m_Overlays.begin(), m_Overlays.end(), [layer](const std::unique_ptr<ILayer> &ptr) { return ptr.get() == layer; });

			if (it != m_Overlays.end())
				m_Overlays.erase(it);
		}

		template<LayerType Layer>
		void RemoveOverlay()
		{
			auto it = std::find_if(m_Overlays.begin(),
								   m_Overlays.end(),
								   [](const std::unique_ptr<ILayer> &ptr) { return dynamic_cast<Layer *>(ptr.get()) != nullptr; });

			if (it != m_Overlays.end())
				m_Overlays.erase(it);
		}

		void OnEvent(const Event &event)
		{
			for (auto it = m_Overlays.rbegin(); it != m_Overlays.rend(); ++it)
			{
				if ((*it)->OnEvent(event))
					return;
			}

			for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
			{
				if ((*it)->OnEvent(event))
					return;
			}
		}

	  private:
		std::vector<std::unique_ptr<ILayer>> m_Layers	= {};
		std::vector<std::unique_ptr<ILayer>> m_Overlays = {};
	};
}	 // namespace Nexus