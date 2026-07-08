#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/nxpch.hpp"
#include "Platform/OpenGL/DeviceBufferOpenGL.hpp"
#include "Platform/OpenGL/SamplerOpenGL.hpp"
#include "Platform/OpenGL/TextureOpenGL.hpp"
#include "RHI/ResourceSet.hpp"

namespace Nexus::Graphics
{
    class ResourceSetOpenGL final : public IResourceSet
    {
      public:
        ResourceSetOpenGL(PipelineHandle pipeline, GraphicsDeviceOpenGL *device);
        void Flush() final;

        void Bind(
            const ResourceSetBindingDescription &bindingDesc, uint32_t programHandle,
            GL::IOffscreenContext *context
        );
        void SetPushConstants(
            const std::string &name, const void *data, size_t offset, size_t size
        );

      private:
        std::map<std::string, std::vector<int32_t>> m_BindingLocations = {};
        std::map<std::string, Ref<DeviceBufferOpenGL>> m_EmulatedPushConstants = {};
        std::map<std::string, Ref<DeviceBufferOpenGL>>
            m_EmulatedInlineUniformBlocks = {};
        std::map<int32_t, const SamplerOpenGL *> m_ImmutableSamplers = {};
    };
} // namespace Nexus::Graphics

#endif