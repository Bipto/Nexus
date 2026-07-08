#pragma once

#include "GraphicsDeviceD3D12.hpp"
#include "RHI/AccelerationStructure.hpp"

namespace Nexus::Graphics
{
    class AccelerationStructureD3D12 : public IAccelerationStructure
    {
      public:
        AccelerationStructureD3D12(const AccelerationStructureDescription &desc, GraphicsDeviceD3D12 *device);
        virtual ~AccelerationStructureD3D12();

        const AccelerationStructureDescription &GetDescription() const final;
        DeviceAddress GetDeviceAddress(size_t offset) const final;

      private:
        AccelerationStructureDescription m_Description = {};
        GraphicsDeviceD3D12 *m_Device = nullptr;
    };
} // namespace Nexus::Graphics