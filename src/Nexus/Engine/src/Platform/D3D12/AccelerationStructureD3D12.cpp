#include "AccelerationStructureD3D12.hpp"
#include "DeviceBufferD3D12.hpp"

namespace Nexus::Graphics
{
    AccelerationStructureD3D12::AccelerationStructureD3D12(
        const AccelerationStructureDescription &desc, GraphicsDeviceD3D12 *device
    )
        : m_Description(desc), m_Device(device)
    {
    }

    AccelerationStructureD3D12::~AccelerationStructureD3D12()
    {
    }

    const AccelerationStructureDescription &AccelerationStructureD3D12::
        GetDescription() const
    {
        return m_Description;
    }

    DeviceAddress AccelerationStructureD3D12::GetDeviceAddress(size_t offset) const
    {
        return m_Description.Buffer->GetDeviceAddress(m_Description.Offset + offset);
    }
} // namespace Nexus::Graphics