#include "PhysicalDeviceEGL.hpp"

namespace Nexus::Graphics
{
	PhysicalDeviceEGL::PhysicalDeviceEGL(bool debug, EGLDisplay display)
	{
		GL::ContextSpecification spec;
		spec.GLVersion = GL::OpenGLVersion::OpenGL;
        spec.VersionMajor = 4;
        spec.VersionMinor = 6;

#if defined(__ANDROID__)
		spec.GLVersion = GL::OpenGLVersion::OpenGLES;
        spec.VersionMajor = 3;
        spec.VersionMinor = 0;
#endif

		spec.Debug = debug;

		m_OffscreenContext = std::make_unique<GL::OffscreenContextEGL>(spec, display);
		m_OffscreenContext->MakeCurrent();

		const GladGLContext &gladContext = m_OffscreenContext->GetContext();
		std::string			 vendor		 = (const char *)gladContext.GetString(GL_VENDOR);
		std::string			 renderer	 = (const char *)gladContext.GetString(GL_RENDERER);

		m_DeviceName = vendor + " " + renderer;
	}

	PhysicalDeviceEGL::~PhysicalDeviceEGL()
	{
	}

	const std::string &PhysicalDeviceEGL::GetDeviceName() const
	{
		return m_DeviceName;
	}

	GL::IOffscreenContext *PhysicalDeviceEGL::GetOffscreenContext()
	{
		return m_OffscreenContext.get();
	}
}	 // namespace Nexus::Graphics
