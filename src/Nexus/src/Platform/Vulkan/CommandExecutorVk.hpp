#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Nexus-Core/Graphics/CommandExecutor.hpp"

#include "Vk.hpp"

namespace Nexus::Graphics
{
class GraphicsDeviceVk;

class CommandExecutorVk : public CommandExecutor
{
  public:
    explicit CommandExecutorVk(GraphicsDeviceVk *device);
    virtual ~CommandExecutorVk();
    virtual void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device) override;
    virtual void Reset() override;

    void SetCommandBuffer(VkCommandBuffer commandBuffer);

    const VkCommandBuffer &GetCurrentCommandBuffer();
    const VkFence &GetCurrentFence();
    const VkSemaphore &GetCurrentSemaphore();

  private:
    virtual void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(WeakRef<IndexBuffer> command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(DrawElementCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(DrawInstancedCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device) override;
    virtual void ExecuteCommand(const TransitionImageLayoutCommand &command, GraphicsDevice *device) override;

    void TransitionVulkanImageLayout(VkImage image, uint32_t level, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlagBits aspectMask);

  private:
    GraphicsDeviceVk *m_Device = nullptr;

    Ref<Pipeline> m_CurrentlyBoundPipeline = nullptr;
    bool m_RenderPassStarted = false;
    VkExtent2D m_RenderSize = {0, 0};

    uint32_t m_DepthAttachmentIndex = 0;
    RenderTarget m_CurrentRenderTarget;

    VkCommandBuffer m_CommandBuffer = nullptr;
};
} // namespace Nexus::Graphics

#endif