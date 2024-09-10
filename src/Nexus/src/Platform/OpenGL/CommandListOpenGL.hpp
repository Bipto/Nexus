#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "GL.hpp"
	#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class CommandListOpenGL : public CommandList
	{
	  public:
		CommandListOpenGL(const CommandListSpecification &spec) : CommandList(spec)
		{
		}

		virtual ~CommandListOpenGL()
		{
		}
	};
}	 // namespace Nexus::Graphics

#endif