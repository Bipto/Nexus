#pragma once

#include "Platform/Vulkan/Vk.hpp"
#include "RHI/Fence.hpp"

#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class FenceVk : public IFence
    {
      public:
        FenceVk(const FenceDescription &desc, GraphicsDeviceVk *device);
        virtual ~FenceVk();

        bool IsSignalled() const final;
        const FenceDescription &GetDescription() const final;

        VkFence GetHandle() const;

      private:
        FenceDescription m_Description = {};
        VkFence m_Fence = nullptr;
        GraphicsDeviceVk *m_Device = nullptr;
    };
} // namespace Nexus::Graphics