#pragma once

#include "Platform/OpenGL/Context/Glad2/OffscreenContextGlad2.hpp"
#include "Platform/OpenGL/ContextDescription.hpp"

#include "Platform/Windows/WindowsInclude.hpp"
#include "glad/wgl.h"

namespace Nexus::GL
{
	class OffscreenContextWGL final : public OffscreenContextGlad2
	{
	  public:
		OffscreenContextWGL(const ContextDescription &spec, Graphics::IPhysicalDevice *device);
		virtual ~OffscreenContextWGL();
		bool				 MakeCurrent() final;
		bool				 Validate() final;
		const GladGLContext &GetContext() const final;

		HGLRC GetHGLRC();

	  private:
		std::tuple<HPBUFFERARB, HDC, HGLRC> CreatePBufferContext(HDC hdc, const ContextDescription &spec);

	  private:
		HGLRC		m_HGLRC	  = {};
		HPBUFFERARB m_PBuffer = {};
		HDC			m_HDC	  = {};

	  private:
		inline static bool s_GLFunctionsLoaded = false;
	};
}	 // namespace Nexus::GL