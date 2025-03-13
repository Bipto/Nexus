#pragma once

#include "Nexus-Core/Timings/TimeSpan.hpp"
#include "sol/sol.hpp"

namespace Nexus::Scripting
{
	class LuaScript
	{
	  public:
		LuaScript(const std::string &text)
		{
			m_LuaVM.open_libraries(sol::lib::base);
			m_LuaVM.script(text);
		}

		inline void ExecuteLoad()
		{
			sol::protected_function func = m_LuaVM["OnLoad"];
			if (func)
			{
				func();
			}
		}

		inline void ExecuteOnRender(TimeSpan timespan)
		{
		}

		inline void ExecuteOnUpdate(TimeSpan timespan)
		{
		}

		inline void ExecuteOnTick(TimeSpan timespan)
		{
		}

	  private:
		sol::state m_LuaVM = {};
	};
}	 // namespace Nexus::Scripting