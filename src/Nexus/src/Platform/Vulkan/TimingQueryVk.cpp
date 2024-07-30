#if defined(NX_PLATFORM_VULKAN)

#include "TimingQueryVk.hpp"

namespace Nexus::Graphics
{
    TimingQueryVk::TimingQueryVk(GraphicsDeviceVk *device)
        : m_Device(device)
    {
        VkQueryPoolCreateInfo queryPoolInfo = {};
        queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        queryPoolInfo.queryCount = 2;

        vkCreateQueryPool(m_Device->GetVkDevice(), &queryPoolInfo, nullptr, &m_QueryPool);
    }

    TimingQueryVk::~TimingQueryVk()
    {
        vkDestroyQueryPool(m_Device->GetVkDevice(), m_QueryPool, nullptr);
    }

    void TimingQueryVk::Resolve()
    {
        const auto &deviceProperties = m_Device->GetDeviceProperties();

        uint64_t timestamps[2];
        vkGetQueryPoolResults(m_Device->GetVkDevice(), m_QueryPool, 0, 2, sizeof(timestamps), timestamps, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
        m_ElapsedTime = ((timestamps[1] - timestamps[0]) * deviceProperties.limits.timestampPeriod) / 1000000.0f;
    }

    float TimingQueryVk::GetElapsedMilliseconds()
    {
        return m_ElapsedTime;
    }

    VkQueryPool TimingQueryVk::GetQueryPool()
    {
        return m_QueryPool;
    }
}

#endif