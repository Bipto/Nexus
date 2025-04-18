#if defined(NX_PLATFORM_WGL)

	#include "PhysicalDeviceWGL.hpp"

namespace Nexus::Graphics
{
	PhysicalDeviceWGL::PhysicalDeviceWGL(const std::string &deviceName, const std::vector<std::string> &displayNames)
		: m_DeviceName(deviceName),
		  m_DisplayNames(displayNames)
	{
		m_HDC = CreateDC("DISPLAY", displayNames[0].c_str(), nullptr, nullptr);
		if (!m_HDC)
		{
			throw std::runtime_error("Failed to create HDC");
			std::cout << "HDC created successfully\n";
		}

		GL::ContextSpecification spec;
		spec.GLVersion = GL::OpenGLVersion::OpenGL;

		m_OffscreenContext = std::make_unique<GL::OffscreenContextWGL>(spec, this);
		m_OffscreenContext->MakeCurrent();
	}

	PhysicalDeviceWGL::~PhysicalDeviceWGL()
	{
		DeleteDC(m_HDC);
	}

	const std::string &PhysicalDeviceWGL::GetDeviceName() const
	{
		return m_DeviceName;
	}

	const PhysicalDeviceFeatures &PhysicalDeviceWGL::GetPhysicalDeviceFeatures() const
	{
		return m_Features;
	}
	const PhysicalDeviceLimits &PhysicalDeviceWGL::GetPhysicalDeviceLimits() const
	{
		return m_Limits;
	}

	GL::IOffscreenContext *PhysicalDeviceWGL::GetOffscreenContext()
	{
		return m_OffscreenContext.get();
	}

	HDC PhysicalDeviceWGL::GetHDC() const
	{
		return m_HDC;
	}
}	 // namespace Nexus::Graphics

#endif