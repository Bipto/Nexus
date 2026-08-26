#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Platform/OpenGL/GL.hpp"
#include "RHI/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
    struct WebGLBufferData
    {
        uint32_t Handle = 0;
        bool Dirty = false;
    };

    class DeviceBufferOpenGL final : public IDeviceBuffer
    {
      public:
        DeviceBufferOpenGL(const DeviceBufferDescription &desc);
        virtual ~DeviceBufferOpenGL();
        void SetData(const void *data, uint32_t offset, uint32_t size) final;
        std::vector<char> GetData(uint32_t offset, uint32_t size) final;
        const DeviceBufferDescription &GetDescription() const final;
        DeviceAddress GetDeviceAddress(size_t offset) const final;

        [[nodiscard]] uint8_t *Map() final;
        void Unmap() final;
        void FlushRange(BufferRange range) final;

        uint32_t GetHandle() const;

        /// @brief This function will be implemented only on WebGL2 backend, due to
        /// requiring separate buffers for different usages
        void MarkDirty();

      private:
        DeviceBufferDescription m_BufferDescription = {};
        std::map<GLenum, WebGLBufferData> m_BufferHandles = {};
    };

} // namespace Nexus::Graphics

#endif