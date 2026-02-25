#pragma once

#include <concepts>
#include <ranges>

#include "Platform/Layers/Layer.hpp"

namespace Nexus
{
	class IWindow;

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
			auto dispatch = [&](auto &container)
			{
				for (auto &layer : std::views::reverse(container))
					if (layer->OnEvent(event))
						return true;
				return false;
			};
			if (dispatch(m_Overlays))
				return;
			if (dispatch(m_Layers))
				return;
		}

		void OnRender(Nexus::TimeSpan time, IWindow *window)
		{
			auto dispatch = [&](auto &container)
			{
				for (auto &layer : container | std::views::all) layer->OnRender(time, window);
			};
			dispatch(m_Layers);
			dispatch(m_Overlays);
		}

		void OnUpdate(Nexus::TimeSpan time, IWindow *window)
		{
			auto dispatch = [&](auto &container)
			{
				for (auto &layer : container | std::views::all) layer->OnUpdate(time, window);
			};
			dispatch(m_Layers);
			dispatch(m_Overlays);
		}
		void OnTick(Nexus::TimeSpan time, IWindow *window)
		{
			auto dispatch = [&](auto &container)
			{
				for (auto &layer : container | std::views::all) layer->OnTick(time, window);
			};
			dispatch(m_Layers);
			dispatch(m_Overlays);
		}

	  private:
		std::vector<std::unique_ptr<ILayer>> m_Layers	= {};
		std::vector<std::unique_ptr<ILayer>> m_Overlays = {};
	};
}	 // namespace Nexus