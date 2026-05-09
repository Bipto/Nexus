#pragma once

#include <cstdint>
#include <expected>
#include <functional>

#include "Platform/OpenGL/OpenGLFunctionContext.hpp"

#include "RHI/Texture.hpp"

namespace Nexus::GL
{
	class IGLContext
	{
	  public:
		virtual ~IGLContext() = default;

		virtual bool				 MakeCurrent()		= 0;
		virtual bool				 Validate()			= 0;
		virtual const GladGLContext &GetContext() const = 0;

		void Execute(std::function<void(const GladGLContext &context)> function)
		{
			MakeCurrent();
			const GladGLContext &context = GetContext();
			function(context);
		}
	};
}	 // namespace Nexus::GL