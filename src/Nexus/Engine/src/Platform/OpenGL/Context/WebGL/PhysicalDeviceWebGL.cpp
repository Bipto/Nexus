#include "PhysicalDeviceWebGL.hpp"

namespace Nexus::Graphics
{
	PhysicalDeviceWebGL::PhysicalDeviceWebGL(const std::string &deviceName) : m_DeviceName(deviceName)
	{
		m_OffscreenContext = std::make_unique<GL::OffscreenContextWebGL>("offscreenContext");
	}

	PhysicalDeviceWebGL::~PhysicalDeviceWebGL()
	{
	}

	const std::string &PhysicalDeviceWebGL::GetDeviceName() const
	{
		return m_DeviceName;
	}

	GL::IOffscreenContext *PhysicalDeviceWebGL::GetOffscreenContext()
	{
		return m_OffscreenContext.get();
	}
}	 // namespace Nexus::Graphics