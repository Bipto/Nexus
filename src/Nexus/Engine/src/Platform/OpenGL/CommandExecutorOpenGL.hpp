#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "DeviceBufferOpenGL.hpp"
#include "PipelineOpenGL.hpp"
#include "RHI/CommandExecutor.hpp"
#include "RHI/CommandList.hpp"
#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
    class CommandExecutorOpenGL final : public CommandExecutor
    {
      public:
        CommandExecutorOpenGL();
        virtual ~CommandExecutorOpenGL();
        void ExecuteCommands(ICommandList *commandList, IGraphicsDevice *device);
        void Reset();

        void BindResourceSet(GL::IOffscreenContext *context);
        void ExecuteGraphicsCommand(
            GL::IOffscreenContext *context, GraphicsPipelineOpenGL *pipeline,
            const std::map<uint32_t, Nexus::Graphics::VertexBufferView> &vertexBuffers,
            std::optional<Nexus::Graphics::IndexBufferView> indexBuffer, uint32_t vertexOffset, uint32_t instanceOffset,
            std::function<void(GraphicsPipelineOpenGL *pipeline, GL::IOffscreenContext *context)> drawCall);

      public:
        GraphicsDeviceOpenGL *m_Device = nullptr;
        PipelineHandle m_CurrentlyBoundPipeline = {};
        FramebufferHandle m_CurrentRenderTarget = {};
        std::map<uint32_t, VertexBufferView> m_CurrentlyBoundVertexBuffers = {};
        std::optional<IndexBufferView> m_BoundIndexBuffer = {};
        std::optional<ResourceSetBindingDescription> m_BoundResourceSet = {};

        std::array<CommandFunc, static_cast<size_t>(CommandType::Count)> m_DispatchTable = {};
    };
} // namespace Nexus::Graphics

#endif