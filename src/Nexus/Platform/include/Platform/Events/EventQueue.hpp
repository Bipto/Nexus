#pragma once

#include <memory>
#include <vector>

#include "Platform/Events/Event.hpp"
#include "Platform/Layers/LayerStack.hpp"

namespace Nexus
{
	class EventQueue
	{
	  public:
		template<EventType E, typename... Args>
		E &Add(Args &&...args)
		{
			auto &ref = *m_Events.emplace_back(std::make_unique<E>(std::forward<Args>(args)...));
			return static_cast<E &>(ref);
		}

		void DispatchEvents(LayerStack &layerStack)
		{
			for (const auto &event : m_Events) { layerStack.OnEvent(*event); }

			m_Events.clear();
		}

	  private:
		std::vector<std::unique_ptr<Event>> m_Events;
	};

}	 // namespace Nexus