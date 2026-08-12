#if defined(NX_PLATFORM_OPENGL)

#include "CommandExecutorOpenGL.hpp"

#include "GraphicsDeviceOpenGL.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "ResourceSetOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "TextureViewOpenGL.hpp"
#include "TimingQueryOpenGL.hpp"

#include "Profiling/Profiler.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
    static void BindFramebuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        auto &cmd = storage.CommandDatas.FramebufferCommands[header->CommandOffset];

        GraphicsDeviceOpenGL *deviceGL = executor->m_Device;

        if (FramebufferOpenGL *framebuffer = cmd.AsDerived<FramebufferOpenGL>())
        {
            GL::IGLContext *context = deviceGL->GetOffscreenContext();
            framebuffer->BindAsDrawBuffer(context);
            executor->m_CurrentRenderTarget = cmd;
        }
    }

    static void ClearColourTarget(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        auto &cmd = storage.CommandDatas.ClearColourTargetCommands[header->CommandOffset];

        if (!executor->ValidateForClearColour(executor->m_CurrentRenderTarget, cmd.Index))
        {
            return;
        }

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        if (cmd.Rect.has_value())
        {
            Graphics::ClearRect rect = cmd.Rect.value();

            GLint scissorBox[4];
            context->GetIntegerv(GL_SCISSOR_BOX, scissorBox);

            float scissorY =
                executor->m_CurrentRenderTarget->GetWidth() - executor->m_CurrentRenderTarget->GetHeight() - rect.Y;
            context->Scissor(rect.X, scissorY, rect.Width, rect.Height);

            float color[] = {cmd.Colour.Red, cmd.Colour.Green, cmd.Colour.Blue, cmd.Colour.Alpha};
            context->ClearBufferfv(GL_COLOR, cmd.Index, color);

            context->Scissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
        }
        else
        {
            float color[] = {cmd.Colour.Red, cmd.Colour.Green, cmd.Colour.Blue, cmd.Colour.Alpha};
            context->ClearBufferfv(GL_COLOR, cmd.Index, color);
        }
    }

    static void ClearDepthStencil(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        auto &cmd = storage.CommandDatas.ClearDepthStencilTargetCommands[header->CommandOffset];

        if (!executor->ValidateForClearDepth(executor->m_CurrentRenderTarget))
        {
            return;
        }

        GLfloat depth = cmd.Value.Depth;
        GLint stencil = cmd.Value.Stencil;

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        // enable clearing of depth buffer
        context->SetDepthMask(true);
        context->ClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
        context->SetDepthMask(false);
    }

    static void SubmitBarrierGroup(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        auto &cmd = storage.CommandDatas.BarrierGroupCommands[header->CommandOffset];

        // boolean indicating whether the resources require synchronisation
        bool requiresFinish = false;

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        if (cmd.TextureBarriers.size() > 0)

        {
            // if we have any texture barriers and texture barriers are supported, we
            // synchronise them
            if (context->SupportsTextureBarriers())
            {
                context->TextureBarrier();
            }
            // otherwise, we need to use glFinish() them
            else
            {
                requiresFinish = true;
            }
        }

        if (cmd.MemoryBarriers.size() > 0)
        {
            // if we have any memory barriers and memory barriers are supported, we
            // synchronise them
            if (context->SupportsMemoryBarriers())
            {
                context->MemoryBarrierEXT(GL_ALL_BARRIER_BITS_EXT);
            }
            // otherwise, we need to use glFinish() them
            else
            {
                requiresFinish = true;
            }
        }

        // legacy synchronise if required
        if (requiresFinish)
        {
            context->Finish();
        }

        // update texture layouts
        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (TextureBarrierDesc textureBarrier : cmd.TextureBarriers)
        {
            TextureOpenGL *textureGL = textureBarrier.Texture.AsDerived<TextureOpenGL>();

            for (uint32_t arrayLayer = textureBarrier.TextureSubresourceRange.BaseArrayLayer;
                 arrayLayer < textureBarrier.TextureSubresourceRange.BaseArrayLayer +
                                  textureBarrier.TextureSubresourceRange.LayerCount;
                 arrayLayer++)
            {
                for (uint32_t mipLevel = textureBarrier.TextureSubresourceRange.BaseMipLevel;
                     mipLevel < textureBarrier.TextureSubresourceRange.BaseMipLevel +
                                    textureBarrier.TextureSubresourceRange.LevelCount;
                     mipLevel++)
                {
                    textureGL->SetTextureLayout(arrayLayer, mipLevel, textureBarrier.Layout);
                }
            }
        }
    }

    static void SetVertexBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.SetVertexBufferCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget))
        {
            return;
        }

        executor->m_CurrentlyBoundVertexBuffers[cmd.Slot] = cmd.View;
    }

    static void SetIndexBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.SetIndexBufferCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget))
        {
            return;
        }

        executor->m_BoundIndexBuffer = cmd.View;
    }

    static void SetPipeline(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.SetPipelineCommands.at(header->CommandOffset);

        if (!cmd.IsValid())
        {
            NX_ERROR("Attempting to bind an invalid pipeline");
            return;
        }

        executor->m_CurrentlyBoundPipeline = cmd;
    }

    static void SetResourceSet(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.ResourceSetBindingCommands.at(header->CommandOffset);

        if (!cmd.TargetResourceSet.IsValid())
        {
            NX_ERROR("Attempting to update pipeline with invalid resources");
            return;
        }

        const ResourceSetOpenGL *resourceSet = cmd.TargetResourceSet.AsDerived<const ResourceSetOpenGL>();
        executor->m_BoundResourceSet = cmd;
    }

    static void Draw(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.DrawCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget))
        {
            return;
        }

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        if (executor->m_CurrentlyBoundPipeline.IsValid())
        {
            if (executor->m_CurrentlyBoundPipeline->GetType() == PipelineType::Graphics)
            {
                GraphicsPipelineOpenGL *pipeline =
                    executor->m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>();
                executor->ExecuteGraphicsCommand(
                    context, pipeline, executor->m_CurrentlyBoundVertexBuffers, executor->m_BoundIndexBuffer,
                    cmd.VertexStart, cmd.InstanceStart,
                    [&](GraphicsPipelineOpenGL *graphicsPipeline, GL::IOffscreenContext *context) {
                        GLenum topology = GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);
                        context->DrawArrays(topology, cmd.VertexStart, cmd.VertexCount, cmd.InstanceCount);
                    }
                );
            }
        }
    }

    static void DrawIndexed(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.DrawIndexedCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget))
        {
            return;
        }

        if (executor->m_CurrentlyBoundPipeline.IsValid())
        {
            GraphicsPipelineOpenGL *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>();
            GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

            if (pipeline->GetType() == PipelineType::Graphics && executor->m_BoundIndexBuffer)
            {
                IndexBufferView &indexBufferView = executor->m_BoundIndexBuffer.value();
                uint32_t indexSizeInBytes = Graphics::GetIndexFormatSizeInBytes(indexBufferView.BufferFormat);
                uint32_t offset = (cmd.IndexStart * indexSizeInBytes) + indexBufferView.Offset;
                GLenum indexFormat = GL::GetGLIndexBufferFormat(indexBufferView.BufferFormat);

                executor->ExecuteGraphicsCommand(
                    context, pipeline, executor->m_CurrentlyBoundVertexBuffers, executor->m_BoundIndexBuffer,
                    cmd.VertexStart, cmd.InstanceStart,
                    [&](GraphicsPipelineOpenGL *graphicsPipeline, GL::IOffscreenContext *context) {
                        GLenum topology = GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);
                        context->DrawElements(
                            topology, cmd.IndexCount, indexFormat, (const void *)(uint64_t)offset, cmd.InstanceCount
                        );
                    }
                );
            }
        }
    }

    static void DrawIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.DrawIndirectCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget))
        {
            return;
        }

        if (executor->m_CurrentlyBoundPipeline.IsValid())
        {
            GraphicsPipelineOpenGL *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>();

            GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

            if (pipeline->GetType() == PipelineType::Graphics)
            {
                const DeviceBufferOpenGL *indirectBuffer = cmd.IndirectBuffer.AsDerived<const DeviceBufferOpenGL>();
                if (indirectBuffer && context->IsIndirectRenderingSupported())
                {
                    executor->ExecuteGraphicsCommand(
                        context, pipeline, executor->m_CurrentlyBoundVertexBuffers, executor->m_BoundIndexBuffer, 0, 0,
                        [&](GraphicsPipelineOpenGL *graphicsPipeline, GL::IOffscreenContext *context) {
                            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());

                            GLenum topology =
                                GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);
                            context->MultiDrawArraysIndirect(
                                topology, (const void *)(uint64_t)cmd.Offset, cmd.DrawCount, cmd.Stride
                            );

                            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
                        }
                    );
                }
            }
        }
    }

    static void DrawIndexedIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.DrawIndirectIndexedCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget))
        {
            return;
        }

        if (executor->m_CurrentlyBoundPipeline.IsValid())
        {
            GraphicsPipelineOpenGL *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>();

            GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

            if (pipeline->GetType() == PipelineType::Graphics && executor->m_BoundIndexBuffer)
            {
                IndexBufferView &indexBufferView = executor->m_BoundIndexBuffer.value();
                uint32_t indexSizeInBytes = Graphics::GetIndexFormatSizeInBytes(indexBufferView.BufferFormat);
                GLenum indexFormat = GL::GetGLIndexBufferFormat(indexBufferView.BufferFormat);

#if !defined(__EMSCRIPTEN__)
                const DeviceBufferOpenGL *indirectBuffer = cmd.IndirectBuffer.AsDerived<const DeviceBufferOpenGL>();
                if (indirectBuffer && context->IsIndirectRenderingSupported())
                {
                    executor->ExecuteGraphicsCommand(
                        context, pipeline, executor->m_CurrentlyBoundVertexBuffers, executor->m_BoundIndexBuffer, 0, 0,
                        [&](GraphicsPipelineOpenGL *graphicsPipeline, GL::IOffscreenContext *context) {
                            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());

                            GLenum topology =
                                GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);

                            context->MultiDrawElementsIndirect(
                                topology, indexFormat, (const void *)(uint64_t)cmd.Offset, cmd.DrawCount, cmd.Stride
                            );

                            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
                        }
                    );

#endif
                }
            }
        }
    }

    static void Dispatch(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.DispatchCommands.at(header->CommandOffset);

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        PipelineOpenGL *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();

        if (context->IsComputeSupported())
        {
            pipeline->Bind(context);
            executor->BindResourceSet(context);
            context->DispatchCompute(cmd.WorkGroupCountX, cmd.WorkGroupCountY, cmd.WorkGroupCountZ);
            context->MemoryBarrierEXT(GL_ALL_BARRIER_BITS);
        }
    }

    static void DispatchIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.DispatchIndirectCommands.at(header->CommandOffset);

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        PipelineOpenGL *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        pipeline->Bind(context);
        executor->BindResourceSet(context);

        const DeviceBufferOpenGL *indirectBuffer = cmd.IndirectBuffer.AsDerived<const DeviceBufferOpenGL>();
        if (indirectBuffer && context->IsIndirectRenderingSupported())
        {
            context->BindBuffer(GL_DISPATCH_INDIRECT_BUFFER, indirectBuffer->GetHandle());
            context->DispatchComputeIndirect(cmd.Offset);
            context->MemoryBarrierEXT(GL_ALL_BARRIER_BITS);
            context->BindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
        }
    }

    static void DrawMesh(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.DrawMeshCommands.at(header->CommandOffset);

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        PipelineOpenGL *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();
        if (context->IsMeshTaskSupported())
        {
            pipeline->Bind(context);
            executor->BindResourceSet(context);
            context->DrawMeshTasksEXT(cmd.WorkGroupCountX, cmd.WorkGroupCountY, cmd.WorkGroupCountZ);
        }
    }

    static void DrawMeshIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.DrawMeshIndirectCommands.at(header->CommandOffset);

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        PipelineOpenGL *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();
        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        const DeviceBufferOpenGL *indirectBuffer = cmd.IndirectBuffer.AsDerived<const DeviceBufferOpenGL>();
        if (indirectBuffer && context->IsMeshTaskSupported())
        {
            pipeline->Bind(context);
            executor->BindResourceSet(context);

            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());
            context->DrawMeshTasksIndirectEXT((GLintptr)cmd.Offset, cmd.DrawCount, cmd.Stride);
            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        }
    }

    static void TraceRays(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
    }

    static void SetViewport(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        auto &cmd = storage.CommandDatas.ViewportCommands[header->CommandOffset];

        if (!executor->ValidateForSetViewport(executor->m_CurrentRenderTarget, cmd))
        {
            return;
        }

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        float left = cmd.X;
        float bottom = executor->m_CurrentRenderTarget->GetHeight() - (cmd.Y + cmd.Height);

        context->Viewport(left, bottom, cmd.Width, cmd.Height);
        context->DepthRangef(cmd.MinDepth, cmd.MaxDepth);
    }

    static void SetScissor(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        auto &cmd = storage.CommandDatas.ScissorCommands[header->CommandOffset];

        if (!executor->ValidateForSetScissor(executor->m_CurrentRenderTarget, cmd))
        {
            return;
        }

        GL::IOffscreenContext *context = executor->m_Device->GetOffscreenContext();

        float scissorY = executor->m_CurrentRenderTarget->GetHeight() - cmd.Height - cmd.Y;
        context->Scissor(cmd.X, scissorY, cmd.Width, cmd.Height);
    }

    static void PushConstants(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.PushConstantsCommands.at(header->CommandOffset);

        if (executor->m_BoundResourceSet.has_value())
        {
            ResourceSetOpenGL *resourceSet =
                executor->m_BoundResourceSet.value().TargetResourceSet.AsDerived<ResourceSetOpenGL>();
            resourceSet->SetPushConstants(cmd.Name, cmd.Data.data(), cmd.Offset, cmd.Data.size());
        }
    }

    static void CopyBufferToBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.CopyBufferToBufferCommands.at(header->CommandOffset);

        // NOTE: This will need a rework for Emscripten backend

        GraphicsDeviceOpenGL *deviceGL = (GraphicsDeviceOpenGL *)executor->m_Device;
        GL::IOffscreenContext *offscreenContext = deviceGL->GetOffscreenContext();

        // extract the OpenGL handles from the API objects
        const DeviceBufferOpenGL *src = cmd.BufferCopy.Source.AsDerived<const DeviceBufferOpenGL>();
        const DeviceBufferOpenGL *dst = cmd.BufferCopy.Destination.AsDerived<const DeviceBufferOpenGL>();

        // check that the buffer handles are valid
        if (src && dst)
        {
            for (const auto &copy : cmd.BufferCopy.Copies)
            {
                offscreenContext->CopyBufferSubData(
                    src->GetHandle(), dst->GetHandle(), copy.ReadOffset, copy.WriteOffset, copy.Size
                );
            }
        }
    }

    static void CopyBufferToTexture(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.CopyBufferToTextureCommands.at(header->CommandOffset);

        Graphics::TextureHandle textureHandle = cmd.BufferTextureCopy.Texture;

        if (textureHandle.IsValid())
        {
            TextureOpenGL *texture = textureHandle.AsDerived<TextureOpenGL>();
            GL::IGLContext *context = executor->m_Device->GetOffscreenContext();

            GL::CopyBufferToTexture(cmd, context);
            texture->MarkDirty();
        }
    }

    static void CopyTextureToBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.CopyTextureToBufferCommands.at(header->CommandOffset);

        Graphics::TextureHandle textureHandle = cmd.TextureBufferCopy.Texture;
        Graphics::DeviceBufferHandle bufferHandle = cmd.TextureBufferCopy.BufferHandle;

        if (bufferHandle.IsValid() && textureHandle.IsValid())
        {
            TextureOpenGL *textureOpenGL = textureHandle.AsDerived<TextureOpenGL>();
            GL::IGLContext *context = executor->m_Device->GetOffscreenContext();

            GL::CopyTextureToBuffer(cmd, context);
        }
    }

    static void CopyTextureToTexture(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorOpenGL *executor = reinterpret_cast<CommandExecutorOpenGL *>(data);
        const auto &cmd = storage.CommandDatas.CopyTextureToTextureCommands.at(header->CommandOffset);

        Graphics::TextureHandle dstHandle = cmd.TextureCopy.Destination;

        const TextureOpenGL *sourceTexture = cmd.TextureCopy.Source.AsDerived<const TextureOpenGL>();
        TextureOpenGL *destTexture = dstHandle.AsDerived<TextureOpenGL>();

        const TextureCopyDescription &copyDesc = cmd.TextureCopy;

        const bool copyDepth = 1;

        GL::IGLContext *context = executor->m_Device->GetOffscreenContext();

        if (context->SupportsCopyImageSubData())
        {
            context->CopyImageSubData(
                sourceTexture->GetHandle(), sourceTexture->GetTextureType(), copyDesc.SourceMipLevel,
                copyDesc.SourceOffset.X, copyDesc.SourceOffset.Y, copyDesc.SourceOffset.Z, destTexture->GetHandle(),
                destTexture->GetTextureType(), copyDesc.DestinationMipLevel, copyDesc.DestinationOffset.X,
                copyDesc.DestinationOffset.Y, copyDesc.DestinationOffset.Z, copyDesc.Extent.Width,
                copyDesc.Extent.Height, copyDepth
            );
        }
        else
        {
            GL::CopyTextureToTexture(copyDesc, context);
        }

        destTexture->MarkDirty();
    }

    CommandExecutorOpenGL::CommandExecutorOpenGL()
    {
        m_DispatchTable[ToIndex(CommandType::SetFramebuffer)] = BindFramebuffer;
        m_DispatchTable[ToIndex(CommandType::ClearColourTarget)] = ClearColourTarget;
        m_DispatchTable[ToIndex(CommandType::BarrierGroup)] = SubmitBarrierGroup;
        m_DispatchTable[ToIndex(CommandType::SetVertexBuffer)] = SetVertexBuffer;
        m_DispatchTable[ToIndex(CommandType::SetIndexBuffer)] = SetIndexBuffer;
        m_DispatchTable[ToIndex(CommandType::SetPipeline)] = SetPipeline;
        m_DispatchTable[ToIndex(CommandType::ResourceSetBinding)] = SetResourceSet;
        m_DispatchTable[ToIndex(CommandType::Draw)] = Draw;
        m_DispatchTable[ToIndex(CommandType::DrawIndexed)] = DrawIndexed;
        m_DispatchTable[ToIndex(CommandType::DrawIndirect)] = DrawIndirect;
        m_DispatchTable[ToIndex(CommandType::DrawIndexedIndirect)] = DrawIndexedIndirect;
        m_DispatchTable[ToIndex(CommandType::DrawMesh)] = DrawMesh;
        m_DispatchTable[ToIndex(CommandType::DrawMeshIndirect)] = DrawMeshIndirect;
        m_DispatchTable[ToIndex(CommandType::TraceRays)] = TraceRays;
        m_DispatchTable[ToIndex(CommandType::Viewport)] = SetViewport;
        m_DispatchTable[ToIndex(CommandType::Scissor)] = SetScissor;
        m_DispatchTable[ToIndex(CommandType::PushConstants)] = PushConstants;
        m_DispatchTable[ToIndex(CommandType::CopyBufferToBuffer)] = CopyBufferToBuffer;
        m_DispatchTable[ToIndex(CommandType::CopyBufferToTexture)] = CopyBufferToTexture;
        m_DispatchTable[ToIndex(CommandType::CopyTextureToBuffer)] = CopyTextureToBuffer;
        m_DispatchTable[ToIndex(CommandType::CopyTextureToTexture)] = CopyTextureToTexture;
    }

    CommandExecutorOpenGL::~CommandExecutorOpenGL()
    {
        Reset();
    }

    void CommandExecutorOpenGL::ExecuteCommands(ICommandList *commandList, IGraphicsDevice *device)
    {
        m_Device = dynamic_cast<GraphicsDeviceOpenGL *>(device);

        NX_PROFILE_FUNCTION();

        // execute commands
        {
            auto &storage = commandList->GetStorage();

            for (const auto &header : storage.CommandDatas.Headers)
            {
                if (auto func = m_DispatchTable[ToIndex(header.Type)])
                {
                    func(&header, storage, this);
                }
            }
        }
    }

    void CommandExecutorOpenGL::Reset()
    {
        m_CurrentlyBoundPipeline = {};
        m_CurrentlyBoundVertexBuffers = {};
        m_CurrentRenderTarget = {};
        m_BoundResourceSet = {};
    }

    void CommandExecutorOpenGL::ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
        {
            return;
        }

        m_CurrentlyBoundVertexBuffers[command.Slot] = command.View;
    }

    void CommandExecutorOpenGL::ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
        {
            return;
        }

        m_BoundIndexBuffer = command.View;
    }

    void CommandExecutorOpenGL::ExecuteCommand(PipelineHandle command, IGraphicsDevice *device)
    {
        if (!command.IsValid())
        {
            NX_ERROR("Attempting to bind an invalid pipeline");
            return;
        }

        m_CurrentlyBoundPipeline = command;
    }

    void CommandExecutorOpenGL::ExecuteGraphicsCommand(
        GL::IOffscreenContext *context, GraphicsPipelineOpenGL *pipeline,
        const std::map<uint32_t, Nexus::Graphics::VertexBufferView> &vertexBuffers,
        std::optional<Nexus::Graphics::IndexBufferView> indexBuffer, uint32_t vertexOffset, uint32_t instanceOffset,
        std::function<void(GraphicsPipelineOpenGL *pipeline, GL::IOffscreenContext *context)> drawCall
    )
    {
        pipeline->CreateVAO(context);
        pipeline->BindBuffers(vertexBuffers, indexBuffer, vertexOffset, instanceOffset, context);
        pipeline->Bind(context);
        BindResourceSet(context);

        bool valid = true;
        for (const auto &[binding, view] : vertexBuffers)
        {
            if (view.Size == 0)
                valid = false;
        }

        if (valid)
        {
            drawCall(pipeline, context);
        }

        pipeline->DestroyVAO(context);
    }

    void CommandExecutorOpenGL::ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
        {
            return;
        }

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        if (m_CurrentlyBoundPipeline.IsValid())
        {
            if (m_CurrentlyBoundPipeline->GetType() == PipelineType::Graphics)
            {
                GraphicsPipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>();
                ExecuteGraphicsCommand(
                    context, pipeline, m_CurrentlyBoundVertexBuffers, m_BoundIndexBuffer, command.VertexStart,
                    command.InstanceStart,
                    [&](GraphicsPipelineOpenGL *graphicsPipeline, GL::IOffscreenContext *context) {
                        GLenum topology = GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);
                        context->DrawArrays(topology, command.VertexStart, command.VertexCount, command.InstanceCount);
                    }
                );
            }
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
        {
            return;
        }

        if (m_CurrentlyBoundPipeline.IsValid())
        {
            GraphicsPipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>();
            GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

            if (pipeline->GetType() == PipelineType::Graphics && m_BoundIndexBuffer)
            {
                IndexBufferView &indexBufferView = m_BoundIndexBuffer.value();
                uint32_t indexSizeInBytes = Graphics::GetIndexFormatSizeInBytes(indexBufferView.BufferFormat);
                uint32_t offset = (command.IndexStart * indexSizeInBytes) + indexBufferView.Offset;
                GLenum indexFormat = GL::GetGLIndexBufferFormat(indexBufferView.BufferFormat);

                ExecuteGraphicsCommand(
                    context, pipeline, m_CurrentlyBoundVertexBuffers, m_BoundIndexBuffer, command.VertexStart,
                    command.InstanceStart,
                    [&](GraphicsPipelineOpenGL *graphicsPipeline, GL::IOffscreenContext *context) {
                        GLenum topology = GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);
                        context->DrawElements(
                            topology, command.IndexCount, indexFormat, (const void *)(uint64_t)offset,
                            command.InstanceCount
                        );
                    }
                );
            }
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
        {
            return;
        }

        if (m_CurrentlyBoundPipeline.IsValid())
        {
            GraphicsPipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>();

            GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

            if (pipeline->GetType() == PipelineType::Graphics)
            {
                const DeviceBufferOpenGL *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferOpenGL>();
                if (indirectBuffer && context->IsIndirectRenderingSupported())
                {
                    ExecuteGraphicsCommand(
                        context, pipeline, m_CurrentlyBoundVertexBuffers, m_BoundIndexBuffer, 0, 0,
                        [&](GraphicsPipelineOpenGL *graphicsPipeline, GL::IOffscreenContext *context) {
                            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());

                            GLenum topology =
                                GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);
                            context->MultiDrawArraysIndirect(
                                topology, (const void *)(uint64_t)command.Offset, command.DrawCount, command.Stride
                            );

                            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
                        }
                    );
                }
            }
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
        {
            return;
        }

        if (m_CurrentlyBoundPipeline.IsValid())
        {
            GraphicsPipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>();

            GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

            if (pipeline->GetType() == PipelineType::Graphics && m_BoundIndexBuffer)
            {
                IndexBufferView &indexBufferView = m_BoundIndexBuffer.value();
                uint32_t indexSizeInBytes = Graphics::GetIndexFormatSizeInBytes(indexBufferView.BufferFormat);
                GLenum indexFormat = GL::GetGLIndexBufferFormat(indexBufferView.BufferFormat);

#if !defined(__EMSCRIPTEN__)
                const DeviceBufferOpenGL *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferOpenGL>();
                if (indirectBuffer && context->IsIndirectRenderingSupported())
                {
                    ExecuteGraphicsCommand(
                        context, pipeline, m_CurrentlyBoundVertexBuffers, m_BoundIndexBuffer, 0, 0,
                        [&](GraphicsPipelineOpenGL *graphicsPipeline, GL::IOffscreenContext *context) {
                            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());

                            GLenum topology =
                                GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);

                            context->MultiDrawElementsIndirect(
                                topology, indexFormat, (const void *)(uint64_t)command.Offset, command.DrawCount,
                                command.Stride
                            );

                            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
                        }
                    );

#endif
                }
            }
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        PipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();

        if (context->IsComputeSupported())
        {
            pipeline->Bind(context);
            BindResourceSet(context);
            context->DispatchCompute(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
            context->MemoryBarrierEXT(GL_ALL_BARRIER_BITS);
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        PipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        pipeline->Bind(context);
        BindResourceSet(context);

        const DeviceBufferOpenGL *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferOpenGL>();
        if (indirectBuffer && context->IsIndirectRenderingSupported())
        {
            context->BindBuffer(GL_DISPATCH_INDIRECT_BUFFER, indirectBuffer->GetHandle());
            context->DispatchComputeIndirect(command.Offset);
            context->MemoryBarrierEXT(GL_ALL_BARRIER_BITS);
            context->BindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        PipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
        if (context->IsMeshTaskSupported())
        {
            pipeline->Bind(context);
            BindResourceSet(context);
            context->DrawMeshTasksEXT(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        PipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();
        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        const DeviceBufferOpenGL *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferOpenGL>();
        if (indirectBuffer && context->IsMeshTaskSupported())
        {
            pipeline->Bind(context);
            BindResourceSet(context);

            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());
            context->DrawMeshTasksIndirectEXT((GLintptr)command.Offset, command.DrawCount, command.Stride);
            context->BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device)
    {
        if (!desc.TargetResourceSet.IsValid())
        {
            NX_ERROR("Attempting to update pipeline with invalid resources");
            return;
        }

        const ResourceSetOpenGL *resourceSet = desc.TargetResourceSet.AsDerived<const ResourceSetOpenGL>();
        m_BoundResourceSet = desc;
    }

    void CommandExecutorOpenGL::ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device)
    {
        if (!ValidateForClearColour(m_CurrentRenderTarget, command.Index))
        {
            return;
        }

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        if (command.Rect.has_value())
        {
            Graphics::ClearRect rect = command.Rect.value();

            GLint scissorBox[4];
            context->GetIntegerv(GL_SCISSOR_BOX, scissorBox);

            float scissorY = m_CurrentRenderTarget->GetWidth() - m_CurrentRenderTarget->GetHeight() - rect.Y;
            context->Scissor(rect.X, scissorY, rect.Width, rect.Height);

            float color[] = {command.Colour.Red, command.Colour.Green, command.Colour.Blue, command.Colour.Alpha};
            context->ClearBufferfv(GL_COLOR, command.Index, color);

            context->Scissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
        }
        else
        {
            float color[] = {command.Colour.Red, command.Colour.Green, command.Colour.Blue, command.Colour.Alpha};
            context->ClearBufferfv(GL_COLOR, command.Index, color);
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device)
    {
        if (!ValidateForClearDepth(m_CurrentRenderTarget))
        {
            return;
        }

        GLfloat depth = command.Value.Depth;
        GLint stencil = command.Value.Stencil;

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        // enable clearing of depth buffer
        context->SetDepthMask(true);
        context->ClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
        context->SetDepthMask(false);
    }

    void CommandExecutorOpenGL::ExecuteCommand(FramebufferHandle command, IGraphicsDevice *device)
    {
        GraphicsDeviceOpenGL *deviceGL = (GraphicsDeviceOpenGL *)device;

        if (FramebufferOpenGL *framebuffer = command.AsDerived<FramebufferOpenGL>())
        {
            GL::IGLContext *context = m_Device->GetOffscreenContext();
            framebuffer->BindAsDrawBuffer(context);
            m_CurrentRenderTarget = command;
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const Viewport &command, IGraphicsDevice *device)
    {
        if (!ValidateForSetViewport(m_CurrentRenderTarget, command))
        {
            return;
        }

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        float left = command.X;
        float bottom = m_CurrentRenderTarget->GetHeight() - (command.Y + command.Height);

        context->Viewport(left, bottom, command.Width, command.Height);
        context->DepthRangef(command.MinDepth, command.MaxDepth);
    }

    void CommandExecutorOpenGL::ExecuteCommand(const Scissor &command, IGraphicsDevice *device)
    {
        if (!ValidateForSetScissor(m_CurrentRenderTarget, command))
        {
            return;
        }

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        float scissorY = m_CurrentRenderTarget->GetHeight() - command.Height - command.Y;
        context->Scissor(command.X, scissorY, command.Width, command.Height);
    }

    void CommandExecutorOpenGL::ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForResolve(command))
        {
            return;
        }

        GL::IGLContext *context = m_Device->GetOffscreenContext();

        Point2D<uint32_t> size =
            Utils::GetMipSize(command.Source->GetWidth(), command.Source->GetHeight(), command.SourceMipLevel);

        Graphics::TextureCopyDescription copyDesc = {};
        copyDesc.Source = command.Source;
        copyDesc.Destination = command.Destination;
        copyDesc.SourceOffset = {0, 0, (int32_t)command.SourceArrayLayer};
        copyDesc.DestinationOffset = {0, 0, (int32_t)command.DestinationArrayLayer};
        copyDesc.SourceMipLevel = command.SourceMipLevel;
        copyDesc.DestinationMipLevel = command.DestinationMipLevel;
        copyDesc.Extent = {size.X, size.Y};

        GL::CopyTextureToTexture(copyDesc, context);

        TextureHandle handle = command.Destination;
        TextureOpenGL *texture = handle.AsDerived<TextureOpenGL>();
        if (texture)
        {
            texture->MarkDirty();
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device)
    {
        if (!command.Query.IsValid())
        {
            NX_ERROR("Attempting to write a timestamp to an invalid query object");
            return;
        }

        TimingQueryHandle queryHandle = command.Query;
        if (TimingQueryOpenGL *query = queryHandle.AsDerived<TimingQueryOpenGL>())
        {
            GL::IGLContext *context = m_Device->GetOffscreenContext();

            if (context->AreTimestampQueriesSupported())
            {
                GLint64 timer;
                context->GetTimestamp(&timer);
                query->m_Start = static_cast<uint64_t>(timer);
            }
            else
            {
                uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::system_clock::now().time_since_epoch()
                )
                                   .count();
                query->m_Start = now;
            }
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device)
    {
        if (!command.Query.IsValid())
        {
            NX_ERROR("Attempting to write a timestamp to an invalid query object");
            return;
        }

        TimingQueryHandle queryHandle = command.Query;
        if (TimingQueryOpenGL *query = queryHandle.AsDerived<TimingQueryOpenGL>())
        {
            GL::IGLContext *context = m_Device->GetOffscreenContext();

            if (context->AreTimestampQueriesSupported())
            {
                GLint64 timer;
                context->GetTimestamp(&timer);
                query->m_End = static_cast<uint64_t>(timer);
            }
            else
            {
                uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::system_clock::now().time_since_epoch()
                )
                                   .count();
                query->m_End = now;
            }
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device)
    {
        Graphics::TextureHandle textureHandle = command.BufferTextureCopy.Texture;

        if (textureHandle.IsValid())
        {
            TextureOpenGL *texture = textureHandle.AsDerived<TextureOpenGL>();
            GL::IGLContext *context = m_Device->GetOffscreenContext();

            GL::CopyBufferToTexture(command, context);
            texture->MarkDirty();
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device)
    {
        Graphics::TextureHandle textureHandle = command.TextureBufferCopy.Texture;
        Graphics::DeviceBufferHandle bufferHandle = command.TextureBufferCopy.BufferHandle;

        if (bufferHandle.IsValid() && textureHandle.IsValid())
        {
            TextureOpenGL *textureOpenGL = textureHandle.AsDerived<TextureOpenGL>();
            GL::IGLContext *context = m_Device->GetOffscreenContext();

            GL::CopyTextureToBuffer(command, context);
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device)
    {
        Graphics::TextureHandle dstHandle = command.TextureCopy.Destination;

        const TextureOpenGL *sourceTexture = command.TextureCopy.Source.AsDerived<const TextureOpenGL>();
        TextureOpenGL *destTexture = dstHandle.AsDerived<TextureOpenGL>();

        const TextureCopyDescription &copyDesc = command.TextureCopy;

        const bool copyDepth = 1;

        GL::IGLContext *context = m_Device->GetOffscreenContext();

        if (context->SupportsCopyImageSubData())
        {
            context->CopyImageSubData(
                sourceTexture->GetHandle(), sourceTexture->GetTextureType(), copyDesc.SourceMipLevel,
                copyDesc.SourceOffset.X, copyDesc.SourceOffset.Y, copyDesc.SourceOffset.Z, destTexture->GetHandle(),
                destTexture->GetTextureType(), copyDesc.DestinationMipLevel, copyDesc.DestinationOffset.X,
                copyDesc.DestinationOffset.Y, copyDesc.DestinationOffset.Z, copyDesc.Extent.Width,
                copyDesc.Extent.Height, copyDepth
            );
        }
        else
        {
            GL::CopyTextureToTexture(copyDesc, context);
        }

        destTexture->MarkDirty();
    }

    void CommandExecutorOpenGL::ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device)
    {
        GL::IGLContext *context = m_Device->GetOffscreenContext();
        context->PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, command.GroupName.c_str());
    }

    void CommandExecutorOpenGL::ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device)
    {
        GL::IGLContext *context = m_Device->GetOffscreenContext();
        context->PopDebugGroup();
    }

    void CommandExecutorOpenGL::ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device)
    {
        GL::IGLContext *context = m_Device->GetOffscreenContext();
        context->DebugMessageInsert(
            GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1,
            command.MarkerName.c_str()
        );
    }

    void CommandExecutorOpenGL::ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device)
    {
        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
        context->BlendColor(
            command.BlendFactor.Red, command.BlendFactor.Green, command.BlendFactor.Blue, command.BlendFactor.Alpha
        );
    }

    void CommandExecutorOpenGL::ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device)
    {
        if (m_CurrentlyBoundPipeline.IsValid())
        {
            if (GraphicsPipelineOpenGL *pipelineGL = m_CurrentlyBoundPipeline.AsDerived<GraphicsPipelineOpenGL>())
            {
                GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

                pipelineGL->SetStencilReference(context, command.StencilReference);
            }
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(
        const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device
    )
    {
    }

    void CommandExecutorOpenGL::ExecuteCommand(
        const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device
    )
    {
    }

    void CommandExecutorOpenGL::ExecuteCommand(
        const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device
    )
    {
    }

    void CommandExecutorOpenGL::ExecuteCommand(
        const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device
    )
    {
    }

    void CommandExecutorOpenGL::ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device)
    {
        if (m_BoundResourceSet.has_value())
        {
            ResourceSetOpenGL *resourceSet =
                m_BoundResourceSet.value().TargetResourceSet.AsDerived<ResourceSetOpenGL>();
            resourceSet->SetPushConstants(command.Name, command.Data.data(), command.Offset, command.Data.size());
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const BarrierGroupDescription &command, IGraphicsDevice *device)
    {
        // boolean indicating whether the resources require synchronisation
        bool requiresFinish = false;

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        if (command.TextureBarriers.size() > 0)

        {
            // if we have any texture barriers and texture barriers are supported, we
            // synchronise them
            if (context->SupportsTextureBarriers())
            {
                context->TextureBarrier();
            }
            // otherwise, we need to use glFinish() them
            else
            {
                requiresFinish = true;
            }
        }

        if (command.MemoryBarriers.size() > 0)
        {
            // if we have any memory barriers and memory barriers are supported, we
            // synchronise them
            if (context->SupportsMemoryBarriers())
            {
                context->MemoryBarrierEXT(GL_ALL_BARRIER_BITS_EXT);
            }
            // otherwise, we need to use glFinish() them
            else
            {
                requiresFinish = true;
            }
        }

        // legacy synchronise if required
        if (requiresFinish)
        {
            context->Finish();
        }

        // update texture layouts
        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (TextureBarrierDesc textureBarrier : command.TextureBarriers)
        {
            TextureOpenGL *textureGL = textureBarrier.Texture.AsDerived<TextureOpenGL>();

            for (uint32_t arrayLayer = textureBarrier.TextureSubresourceRange.BaseArrayLayer;
                 arrayLayer < textureBarrier.TextureSubresourceRange.BaseArrayLayer +
                                  textureBarrier.TextureSubresourceRange.LayerCount;
                 arrayLayer++)
            {
                for (uint32_t mipLevel = textureBarrier.TextureSubresourceRange.BaseMipLevel;
                     mipLevel < textureBarrier.TextureSubresourceRange.BaseMipLevel +
                                    textureBarrier.TextureSubresourceRange.LevelCount;
                     mipLevel++)
                {
                    textureGL->SetTextureLayout(arrayLayer, mipLevel, textureBarrier.Layout);
                }
            }
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(const TraceRaysDescription &desc, IGraphicsDevice *device)
    {
    }

    void CommandExecutorOpenGL::ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device)
    {
    }

    void CommandExecutorOpenGL::BindResourceSet(GL::IOffscreenContext *context)
    {
        PipelineOpenGL *pipeline = m_CurrentlyBoundPipeline.AsDerived<PipelineOpenGL>();
        if (!pipeline)
            return;

        pipeline->Bind(context);

        if (m_BoundResourceSet.has_value())
        {
            ResourceSetBindingDescription bindingDescription = m_BoundResourceSet.value();
            ResourceSetOpenGL *resourceSet = bindingDescription.TargetResourceSet.AsDerived<ResourceSetOpenGL>();
            if (resourceSet)
            {
                resourceSet->Bind(bindingDescription, pipeline->GetShaderHandle(), context);
            }
        }
    }
} // namespace Nexus::Graphics

#endif