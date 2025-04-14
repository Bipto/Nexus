#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{

	struct GraphicsAPICreateInfo
	{
		GraphicsAPI API	  = GraphicsAPI::OpenGL;
		bool		Debug = false;
	};

	class NX_API IGraphicsAPI
	{
	  public:
		virtual ~IGraphicsAPI()																								= default;
		virtual std::vector<std::shared_ptr<IPhysicalDevice>> GetPhysicalDevices()											= 0;
		virtual Graphics::GraphicsDevice					 *CreateGraphicsDevice(std::shared_ptr<IPhysicalDevice> device) = 0;
		virtual const GraphicsAPICreateInfo					 &GetGraphicsAPICreateInfo() const								= 0;

		static IGraphicsAPI *CreateAPI(const GraphicsAPICreateInfo &createInfo);
	};
}	 // namespace Nexus::Graphics