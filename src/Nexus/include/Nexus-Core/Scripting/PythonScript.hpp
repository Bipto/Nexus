#pragma once

#include "pocketpy.h"

#include "Nexus-Core/Timings/Timespan.hpp"

namespace Nexus::Scripting
{
	class PythonScript
	{
	  public:
		PythonScript()
		{
			py_initialize();

			py_exec("print('Hello World')", "<string>", EXEC_MODE, nullptr);

			py_finalize();
		}

		virtual ~PythonScript()
		{
		}

		inline void OnLoad()
		{
		}

		inline void OnUpdate()
		{
		}

		inline void OnRender()
		{
		}

		inline void OnTick()
		{
		}

		inline void OnUnload()
		{
		}

	  private:
	};
}	 // namespace Nexus::Scripting