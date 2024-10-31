#if defined(NX_PLATFORM_WEBGL)

	#include "FBO_WebGL.hpp"

namespace Nexus::GL
{
	Nexus::GL::FBO_WebGL::FBO_WebGL(const std::string &name, const ContextSpecification &spec) : m_Name(name), m_Specification(spec)
	{
		EmscriptenWebGLContextAttributes attr = {};
		emscripten_webgl_init_context_attributes(&attr);

		m_Context = emscripten_webgl_create_context(name.c_str(), &attr);
	}

	bool FBO_WebGL::MakeCurrent()
	{
		return emscripten_webgl_make_context_current(m_Context);
	}

	void FBO_WebGL::Swap()
	{
		MakeCurrent();
		emscripten_webgl_commit_frame();
	}

	void FBO_WebGL::SetVSync(bool enabled)
	{
		m_Specification.Vsync = enabled;
	}

	const ContextSpecification &FBO_WebGL::GetSpecification() const
	{
		return m_Specification;
	}

}	 // namespace Nexus::GL

#endif