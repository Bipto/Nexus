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
#if defined(NX_PLATFORM_OPENGL)
			case GraphicsAPI::OpenGL: return new GraphicsAPI_OpenGL(createInfo);
#endif

#if defined(NX_PLATFORM_D3D12)
			case GraphicsAPI::D3D12: return new GraphicsAPI_D3D12(createInfo);
#endif

#if defined(NX_PLATFORM_VULKAN)
			case GraphicsAPI::Vulkan: return new GraphicsAPI_Vk(createInfo);
#endif

			default:
			{
				NX_ERROR("Attempting to create unsupported graphics API");
				throw std::runtime_error("Failed to find a valid graphics API");
			}
		}

		return nullptr;
	}

	bool IGraphicsAPI::IsAPISupported(GraphicsAPI api)
	{
		switch (api)
		{
			case GraphicsAPI::OpenGL:
			{
#if defined(NX_PLATFORM_OPENGL)
				return true;
#else
				return false;
#endif
			}

			case GraphicsAPI::D3D12:
			{
#if defined(NX_PLATFORM_D3D12)
				return true;

#else
				return false;
#endif
			}

			case GraphicsAPI::Vulkan:
			{
#if defined(NX_PLATFORM_VULKAN)
				return true;
#else
				return false;
#endif
			}
		}

		return false;
	}

}	 // namespace Nexus::Graphics
