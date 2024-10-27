#pragma once

#if defined(NX_PLATFORM_WEBGL)

	#include "Nexus-Core/nxpch.hpp"

	#include "Platform/OpenGL/FBO.hpp"

	#include <emscripten.h>
	#include <emscripten/html5.h>

namespace Nexus::GL
{
	class FBO_WebGL : public FBO
	{
	  public:
		FBO_WebGL(const std::string &name, const ContextSpecification &spec);
		virtual bool						MakeCurrent() override;
		virtual void						Swap() override;
		virtual void						SetVSync(bool enabled) override;
		virtual const ContextSpecification &GetSpecification() const override;

	  private:
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_Context		= {};
		std::string						m_Name			= {};
		ContextSpecification			m_Specification = {};
	}
}	 // namespace Nexus::GL

#endif