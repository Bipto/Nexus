#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Platform/D3D12/GraphicsAPI_D3D12.hpp"
#include "Platform/OpenGL/GraphicsAPI_OpenGL.hpp"
#include "Platform/Vulkan/GraphicsAPIVk.hpp"

namespace Nexus::Graphics
{
	IGraphicsAPI *IGraphicsAPI::CreateAPI(const GraphicsAPICreateInfo &createInfo)
	{
		switch (createInfo.API)
		{
			case GraphicsAPI::OpenGL: return new GraphicsAPI_OpenGL(createInfo);
			case GraphicsAPI::D3D12: return new GraphicsAPI_D3D12(createInfo);
			case GraphicsAPI::Vulkan: return new GraphicsAPI_Vk(createInfo);
			default: throw std::runtime_error("Failed to find a valid graphics API");
		}

		return nullptr;
	}

	bool IGraphicsAPI::IsAPISupported(GraphicsAPI api)
	{
		switch (api)
		{
#if defined(NX_PLATFORM_OPENGL)
			case GraphicsAPI::OpenGL:
			{
				return true;
			}
#endif
#if defined(NX_PLATFORM_D3D12)
			case GraphicsAPI::D3D12:
			{
				return true;
			}
#endif
#if defined(NX_PLATFORM_VULKAN)
			case GraphicsAPI::Vulkan:
			{
				return true;
			}
#endif
		}

		return false;
	}

}	 // namespace Nexus::Graphics
