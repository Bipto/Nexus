#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "RHI/CommandExecutor.hpp"
#include "SwapchainVk.hpp"
#include "Vk.hpp"

#include <unordered_map>

namespace Nexus::Graphics
{
    class GraphicsDeviceVk;
    class PipelineVk;

    class CommandExecutorVk : public CommandExecutor
    {
      public:
        explicit CommandExecutorVk(GraphicsDeviceVk *device);
        virtual ~CommandExecutorVk();
        void ExecuteCommands(ICommandList *commandList, IGraphicsDevice *device) final;
        void Reset() final;

      private:
        void StartRenderingToFramebuffer(FramebufferHandle framebuffer);

      public:
        void BindFramebufferImpl(FramebufferHandle framebuffer);
        void StopRendering();
        bool ValidateIsRendering();

        void BindGraphicsPipeline();
        void TryStartRendering();

      public:
        GraphicsDeviceVk *m_Device = nullptr;

        PipelineHandle m_CurrentlyBoundPipeline = {};
        const ResourceSetVk *m_CurrentlyBoundResourceSet = nullptr;
        bool m_Rendering = false;
        VkExtent2D m_RenderSize = {0, 0};

        uint32_t m_DepthAttachmentIndex = 0;
        FramebufferHandle m_CurrentRenderTarget = {};

        VkCommandBuffer m_CommandBuffer = nullptr;

        std::optional<CommandType> m_NextCommandType = {};
        bool m_LastCommand = false;

        std::array<CommandFunc, static_cast<size_t>(CommandType::Count)> m_DispatchTable = {};
    };
} // namespace Nexus::Graphics

#endif