#pragma once

#if defined(WIN32)

#include "Core/Graphics/Buffer.h"
#include "DX11.h"

namespace Nexus::Graphics
{
    class DeviceBufferDX11 : public DeviceBuffer
    {
    public:
        DeviceBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data = nullptr);
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;

        ID3D11Buffer *GetNativeHandle() { return m_Buffer; }

    private:
        ID3D11Buffer *m_Buffer;
        ID3D11DeviceContext *m_Context;
    };
}

#endif