#pragma once

#include <sol/sol.hpp>

namespace Nexus::Scripting
{
	class LuaScript
	{
	  public:
		LuaScript()
		{
			m_State = std::make_shared<sol::state>();
			m_State->open_libraries(sol::lib::base);
		}

		virtual ~LuaScript()
		{
		}

		inline void OnLoad()
		{
			m_State->script("print('Loading in Lua!')");
		}

		inline void OnUpdate()
		{
			m_State->script("print('Updating in Lua!')");
		}

		inline void OnRender()
		{
			m_State->script("print('Rendering in Lua!')");
		}

		inline void OnTick()
		{
			m_State->script("print('Ticking in Lua!')");
		}

		inline void OnUnload()
		{
			m_State->script("print('Unloading in Lua!')");
		}

	  private:
		std::shared_ptr<sol::state> m_State = nullptr;
	};
}	 // namespace Nexus::Scripting