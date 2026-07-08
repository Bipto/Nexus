#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "RHI/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceD3D12;

    class DeviceBufferD3D12 final : public IDeviceBuffer
    {
      public:
        DeviceBufferD3D12(const DeviceBufferDescription &desc, GraphicsDeviceD3D12 *graphicsDevice);
        virtual ~DeviceBufferD3D12();
        void SetData(const void *data, uint32_t offset, uint32_t size) final;
        std::vector<char> GetData(uint32_t offset, uint32_t size) final;
        const DeviceBufferDescription &GetDescription() const final;
        DeviceAddress GetDeviceAddress(size_t offset) const final;

        [[nodiscard]] uint8_t *Map() final;
        void Unmap() final;
        void FlushRange(BufferRange range) final;

        Microsoft::WRL::ComPtr<ID3D12Resource2> GetHandle() const;
        size_t GetBufferSizeInBytes() const;

      private:
        DeviceBufferDescription m_BufferDescription = {};
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_BufferHandle = nullptr;
        Microsoft::WRL::ComPtr<D3D12MA::Allocation> m_Allocation = nullptr;
        GraphicsDeviceD3D12 *m_GraphicsDevice = nullptr;
        size_t m_BufferSize = 0;

        uint8_t *m_MappedHandle = nullptr;
    };
} // namespace Nexus::Graphics

#endif