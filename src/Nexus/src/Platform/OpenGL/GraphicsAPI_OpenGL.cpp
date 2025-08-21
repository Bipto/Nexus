#if defined(NX_PLATFORM_OPENGL)

	#include "GraphicsAPI_OpenGL.hpp"

	#include "GraphicsDeviceOpenGL.hpp"
namespace Nexus::Graphics
{

	GraphicsAPI_OpenGL::GraphicsAPI_OpenGL(const GraphicsAPICreateInfo &createInfo) : m_CreateInfo(createInfo)
	{
		NX_VALIDATE(GL::LoadOpenGL(), "Failed to load OpenGL");
	}

	GraphicsAPI_OpenGL::~GraphicsAPI_OpenGL()
	{
	}

	std::vector<std::shared_ptr<IPhysicalDevice>> GraphicsAPI_OpenGL::GetPhysicalDevices()
	{
		return GL::LoadAvailablePhysicalDevices(m_CreateInfo.Debug);
	}

	GraphicsDevice *GraphicsAPI_OpenGL::CreateGraphicsDevice(std::shared_ptr<IPhysicalDevice> device)
	{
		return new GraphicsDeviceOpenGL(device, m_CreateInfo.Debug);
	}

	const GraphicsAPICreateInfo &GraphicsAPI_OpenGL::GetGraphicsAPICreateInfo() const
	{
		return m_CreateInfo;
	}

}	 // namespace Nexus::Graphics

#endif