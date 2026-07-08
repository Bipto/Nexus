#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Platform/OpenGL/GL.hpp"
#include "RHI/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceOpenGL;

    class DeviceBufferOpenGL final : public IDeviceBuffer
    {
      public:
        DeviceBufferOpenGL(
            const DeviceBufferDescription &desc, GraphicsDeviceOpenGL *device
        );
        virtual ~DeviceBufferOpenGL();
        void SetData(const void *data, uint32_t offset, uint32_t size) final;
        std::vector<char> GetData(uint32_t offset, uint32_t size) final;
        const DeviceBufferDescription &GetDescription() const final;
        DeviceAddress GetDeviceAddress(size_t offset) const final;

        [[nodiscard]] uint8_t *Map() final;
        void Unmap() final;
        void FlushRange(BufferRange range) final;

        uint32_t GetHandle() const;

        void MarkDirty();

      private:
        GraphicsDeviceOpenGL *m_Device = nullptr;
        DeviceBufferDescription m_BufferDescription = {};
        uint32_t m_BufferHandle = 0;
        std::vector<uint8_t> m_BufferStorage = {};
        bool m_PersistentMapping = false;
    };

} // namespace Nexus::Graphics

#endif