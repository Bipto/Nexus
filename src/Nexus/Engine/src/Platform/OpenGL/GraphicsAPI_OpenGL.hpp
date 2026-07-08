#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "RHI/IGraphicsAPI.hpp"

namespace Nexus::Graphics
{
    class GraphicsAPI_OpenGL : public IGraphicsAPI
    {
      public:
        GraphicsAPI_OpenGL(const GraphicsAPICreateInfo &createInfo);
        virtual ~GraphicsAPI_OpenGL();
        virtual std::vector<std::shared_ptr<IPhysicalDevice>>
        GetPhysicalDevices() override;
        virtual Graphics::IGraphicsDevice *CreateGraphicsDevice(
            std::shared_ptr<IPhysicalDevice> device
        ) override;
        virtual const GraphicsAPICreateInfo &
        GetGraphicsAPICreateInfo() const override;

      private:
        GraphicsAPICreateInfo m_CreateInfo = {};
    };
} // namespace Nexus::Graphics

#endif