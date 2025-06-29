#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "glad/gl.h"

namespace Nexus::GL
{
	class OpenGLFunctionContext
	{
	  public:
		OpenGLFunctionContext();
		virtual ~OpenGLFunctionContext();

		bool Load();

		void ExecuteCommands(std::function<void(const GladGLContext &context)> function);

		const GladGLContext &GetContext() const
		{
			return m_Context;
		}

	  private:
		GladGLContext m_Context = {};
	};
}	 // namespace Nexus::GL