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

			m_LuaVM.new_usertype<TimeSpan>("TimeSpan", "GetSeconds", &TimeSpan::GetSeconds);
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
			sol::protected_function func = m_LuaVM["OnUpdate"];
			if (func)
			{
				func(timespan);
			}
		}

		inline void ExecuteOnTick(TimeSpan timespan)
		{
		}

	  private:
		sol::state m_LuaVM = {};
	};
}	 // namespace Nexus::Scripting