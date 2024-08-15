#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "Nexus-Core/Graphics/TimingQuery.hpp"

namespace Nexus::Graphics
{
class TimingQueryD3D12 : public TimingQuery
{
  public:
    TimingQueryD3D12(GraphicsDeviceD3D12 *device);
    virtual ~TimingQueryD3D12();
    virtual void Resolve() override;
    virtual float GetElapsedMilliseconds() override;
    Microsoft::WRL::ComPtr<ID3D12QueryHeap> GetQueryHeap();

  private:
    void CreateQueryHeap();
    void CreateReadbackBuffer();

  private:
    GraphicsDeviceD3D12 *m_Device = nullptr;
    Microsoft::WRL::ComPtr<ID3D12QueryHeap> m_QueryHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_ReadbackBuffer = nullptr;
    float m_ElapsedTime = 0.0f;
};
} // namespace Nexus::Graphics

#endif