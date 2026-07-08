#pragma once

#include "RHI/GraphicsAPICreateInfo.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "RHI/IPhysicalDevice.hpp"
#include "RHI/RHI-Core.hpp"

namespace Nexus::Graphics
{

    class NX_RHI_API IGraphicsAPI
    {
      public:
        virtual ~IGraphicsAPI() = default;
        virtual std::vector<std::shared_ptr<IPhysicalDevice>>
        GetPhysicalDevices() = 0;
        virtual Graphics::IGraphicsDevice *CreateGraphicsDevice(
            std::shared_ptr<IPhysicalDevice> device
        ) = 0;
        virtual const GraphicsAPICreateInfo &GetGraphicsAPICreateInfo() const = 0;

        static IGraphicsAPI *CreateAPI(const GraphicsAPICreateInfo &createInfo);
        static bool IsAPISupported(GraphicsAPI api);
    };
} // namespace Nexus::Graphics