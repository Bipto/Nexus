#pragma once

#include "GL.hpp"
#include "Core/Graphics/Buffer.hpp"

#include "Platform/OpenGL/ShaderOpenGL.hpp"

namespace Nexus::Graphics
{
    class DeviceBufferOpenGL : public DeviceBuffer
    {
    public:
        DeviceBufferOpenGL(const BufferDescription &description, const void *data = nullptr);
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        void Bind();
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
    };
}