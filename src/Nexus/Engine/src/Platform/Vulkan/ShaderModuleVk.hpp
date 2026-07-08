#pragma once

#include "RHI/ShaderModule.hpp"
#include "Vk.hpp"

#if defined(NX_PLATFORM_VULKAN)

namespace Nexus::Graphics
{
    class GraphicsDeviceVk;

    class ShaderModuleVk : public IShaderModule
    {
      public:
        ShaderModuleVk(const ShaderModuleDescription &shaderModuleSpec, GraphicsDeviceVk *device);
        virtual ~ShaderModuleVk();
        VkShaderModule GetShaderModule() const;
        ShaderReflectionData Reflect() const final;

      private:
        void CreateShaderModule();

      private:
        GraphicsDeviceVk *m_GraphicsDevice = nullptr;
        VkShaderModule m_ShaderModule;
    };
} // namespace Nexus::Graphics

#endif