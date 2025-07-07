#pragma once

#include "Platform/OpenGL/OpenGLFunctionContext.hpp"

namespace Nexus::GL
{
	class IGLContext
	{
	  public:
		virtual ~IGLContext()
		{
		}

		virtual bool MakeCurrent() = 0;
		virtual bool Validate()	   = 0;
		virtual const GladGLContext &GetContext() const = 0;
	};
}	 // namespace Nexus::GL