#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "GraphicsDeviceVk.hpp"
#include "Nexus-Core/Graphics/TimingQuery.hpp"
#include "Vk.hpp"

namespace Nexus::Graphics {
class TimingQueryVk : public TimingQuery {
public:
  TimingQueryVk(GraphicsDeviceVk *device);
  virtual ~TimingQueryVk();
  virtual void Resolve() override;
  virtual float GetElapsedMilliseconds() override;
  VkQueryPool GetQueryPool();

private:
  GraphicsDeviceVk *m_Device = nullptr;
  VkQueryPool m_QueryPool = nullptr;
  float m_ElapsedTime = 0.0f;
};
} // namespace Nexus::Graphics

#endif