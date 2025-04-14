#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Platform/OpenGL/GraphicsAPI_OpenGL.hpp"

namespace Nexus::Graphics
{
	IGraphicsAPI *IGraphicsAPI::CreateAPI(const GraphicsAPICreateInfo &createInfo)
	{
		switch (createInfo.API)
		{
			case GraphicsAPI::OpenGL: return new GraphicsAPI_OpenGL(createInfo);
			case GraphicsAPI::D3D12: break;
			case GraphicsAPI::Vulkan: break;
			default: throw std::runtime_error("Failed to find a valid graphics API");
		}

		return nullptr;
	}
}	 // namespace Nexus::Graphics
