#pragma once

#include "Nexus-Core/Timings/Timespan.hpp"

namespace Nexus::Scripting
{
	class Script
	{
	  public:
		Script() = default;
		virtual ~Script()
		{
		}

		virtual void OnLoad()
		{
		}

		virtual void OnUpdate(TimeSpan time)
		{
		}

		virtual void OnRender(TimeSpan time)
		{
		}

		virtual void OnUnload()
		{
		}
	};
}	 // namespace Nexus::Scripting