#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Platform/D3D12/GraphicsAPI_D3D12.hpp"
#include "Platform/OpenGL/GraphicsAPI_OpenGL.hpp"

namespace Nexus::Graphics
{
	IGraphicsAPI *IGraphicsAPI::CreateAPI(const GraphicsAPICreateInfo &createInfo)
	{
		switch (createInfo.API)
		{
			case GraphicsAPI::OpenGL: return new GraphicsAPI_OpenGL(createInfo);
			case GraphicsAPI::D3D12: return new GraphicsAPI_D3D12(createInfo);
			case GraphicsAPI::Vulkan: break;
			default: throw std::runtime_error("Failed to find a valid graphics API");
		}

		return nullptr;
	}
}	 // namespace Nexus::Graphics
