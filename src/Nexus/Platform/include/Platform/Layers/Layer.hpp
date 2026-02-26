#pragma once

#include "Platform/Events/Event.hpp"
#include "Platform/Timings/Timespan.hpp"

namespace Nexus
{
	class IWindow;

	class ILayer
	{
	  public:
		virtual ~ILayer()											 = default;
		virtual bool OnEvent(const Event &event)					 = 0;
		virtual void OnRender(Nexus::TimeSpan time, IWindow *window) = 0;
		virtual void OnUpdate(Nexus::TimeSpan time, IWindow *window) = 0;
		virtual void OnTick(Nexus::TimeSpan time, IWindow *window)	 = 0;
	};
}	 // namespace Nexus
