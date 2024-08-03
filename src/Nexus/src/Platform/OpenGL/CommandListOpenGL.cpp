#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/nxpch.hpp"

#include "CommandListOpenGL.hpp"
#include "PipelineOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "GraphicsDeviceOpenGL.hpp"
#include "ResourceSetOpenGL.hpp"
#include "TimingQueryOpenGL.hpp"

#include "GL.hpp"

namespace Nexus::Graphics
{
    CommandListOpenGL::CommandListOpenGL(GraphicsDevice *device)
        : m_Device(device)
    {
    }

    CommandListOpenGL::~CommandListOpenGL()
    {
        m_CommandRecorder.Clear();
    }

    void CommandListOpenGL::Begin()
    {
        m_CommandRecorder.Clear();
    }

    void CommandListOpenGL::End()
    {
    }

    void CommandListOpenGL::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot)
    {
        SetVertexBufferCommand command;
        command.VertexBufferRef = vertexBuffer;
        command.Slot = slot;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        m_CommandRecorder.PushCommand(indexBuffer);
    }

    void CommandListOpenGL::SetPipeline(Ref<Pipeline> pipeline)
    {
        m_CommandRecorder.PushCommand(pipeline);
    }

    void CommandListOpenGL::Draw(uint32_t start, uint32_t count)
    {
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
    {
        DrawIndexedCommand command;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.Count = count;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
    {
        DrawInstancedCommand command;
        command.VertexCount = vertexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.InstanceStart = instanceStart;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart)
    {
        DrawInstancedIndexedCommand command;
        command.IndexCount = indexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.InstanceStart = instanceStart;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::SetResourceSet(Ref<ResourceSet> resources)
    {
        UpdateResourcesCommand command;
        command.Resources = resources;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        ClearColorTargetCommand command;
        command.Index = index;
        command.Color = color;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        ClearDepthStencilTargetCommand command;
        command.Value = value;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::SetRenderTarget(RenderTarget target)
    {
        SetRenderTargetCommand command;
        command.Target = target;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::SetViewport(const Viewport &viewport)
    {
        SetViewportCommand command;
        command.Viewport = viewport;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::SetScissor(const Scissor &scissor)
    {
        SetScissorCommand command;
        command.Scissor = scissor;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target)
    {
        ResolveSamplesToSwapchainCommand command;
        command.Source = source;
        command.SourceIndex = sourceIndex;
        command.Target = target;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::StartTimingQuery(Ref<TimingQuery> query)
    {
        StartTimingQueryCommand command;
        command.Query = query;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListOpenGL::StopTimingQuery(Ref<TimingQuery> query)
    {
        StopTimingQueryCommand command;
        command.Query = query;
        m_CommandRecorder.PushCommand(command);
    }

    CommandRecorder &CommandListOpenGL::GetCommandRecorder()
    {
        return m_CommandRecorder;
    }

    GraphicsDevice *CommandListOpenGL::GetGraphicsDevice()
    {
        return m_Device;
    }
}

#endif