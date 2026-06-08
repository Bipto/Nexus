#pragma once

#include <expected>
#include <functional>

#include "glad/gl.h"

#include "Platform/OpenGL/Context/IOffscreenContext.hpp"

namespace Nexus::GL
{
	class OffscreenContextGlad2 : public IOffscreenContext
	{
	  public:
		OffscreenContextGlad2();
		virtual ~OffscreenContextGlad2();

		bool Load();

		void ExecuteCommands(std::function<void(const GladGLContext &context)> function);

		const GladGLContext &GetContext() const;

	  protected:
		GladGLContext m_Context = {};
	};
}	 // namespace Nexus::GL