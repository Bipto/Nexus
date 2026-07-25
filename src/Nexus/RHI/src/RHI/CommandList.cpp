#include "RHI/CommandList.hpp"
#include "RHI/CommandExecutor.hpp"

#include <cstring>

namespace Nexus::Graphics
{
    SetVertexBufferCommandImpl::SetVertexBufferCommandImpl(const SetVertexBufferCommand &command) : m_Command(command)
    {
    }

    void SetVertexBufferCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_Command, device);
    }

    SetIndexBufferCommandImpl::SetIndexBufferCommandImpl(const IndexBufferView &view) : m_BufferView(view)
    {
    }

    void SetIndexBufferCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        SetIndexBufferCommand command{.View = m_BufferView};
        executor->ExecuteCommand(command, device);
    }

    SetPipelineCommandImpl::SetPipelineCommandImpl(PipelineHandle pipeline) : m_Pipeline(pipeline)
    {
    }

    void SetPipelineCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_Pipeline, device);
    }

    DrawCommandImpl::DrawCommandImpl(const DrawDescription &desc) : m_DrawDesc(desc)
    {
    }

    void DrawCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_DrawDesc, device);
    }

    DrawIndexedCommandImpl::DrawIndexedCommandImpl(const DrawIndexedDescription &desc) : m_DrawDesc(desc)
    {
    }

    void DrawIndexedCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_DrawDesc, device);
    }

    DrawIndirectCommandImpl::DrawIndirectCommandImpl(const DrawIndirectDescription &desc) : m_DrawDesc(desc)
    {
    }

    void DrawIndirectCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_DrawDesc, device);
    }

    DrawIndexedIndirectCommandImpl::DrawIndexedIndirectCommandImpl(const DrawIndirectIndexedDescription &desc)
        : m_DrawDesc(desc)
    {
    }

    void DrawIndexedIndirectCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_DrawDesc, device);
    }

    DrawMeshCommandImpl::DrawMeshCommandImpl(const DrawMeshDescription &desc) : m_DrawDesc(desc)
    {
    }

    void DrawMeshCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_DrawDesc, device);
    }

    DrawMeshIndirectCommandImpl::DrawMeshIndirectCommandImpl(const DrawMeshIndirectDescription &desc) : m_DrawDesc(desc)
    {
    }

    void DrawMeshIndirectCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_DrawDesc, device);
    }

    DispatchCommandImpl::DispatchCommandImpl(const DispatchDescription &desc) : m_DispatchDesc(desc)
    {
    }

    void DispatchCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_DispatchDesc, device);
    }

    DispatchIndirectCommandImpl::DispatchIndirectCommandImpl(const DispatchIndirectDescription &desc)
        : m_DispatchDesc(desc)
    {
    }

    void DispatchIndirectCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_DispatchDesc, device);
    }

    TraceRaysCommandImpl::TraceRaysCommandImpl(const TraceRaysDescription &desc) : m_TraceRaysDesc(desc)
    {
    }

    void TraceRaysCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_TraceRaysDesc, device);
    }

    SetResourceSetCommandImpl::SetResourceSetCommandImpl(const ResourceSetBindingDescription &desc)
        : m_ResourceSetBindings(desc)
    {
    }

    void SetResourceSetCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_ResourceSetBindings, device);
    }

    ClearColourTargetCommandImpl::ClearColourTargetCommandImpl(const ClearColorTargetCommand &desc)
        : m_CommandData(desc)
    {
    }

    void ClearColourTargetCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_CommandData, device);
    }

    ClearDepthStencilTargetCommandImpl::ClearDepthStencilTargetCommandImpl(const ClearDepthStencilTargetCommand &desc)
        : m_CommandData(desc)
    {
    }

    void ClearDepthStencilTargetCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_CommandData, device);
    }

    SetFramebufferCommandImpl::SetFramebufferCommandImpl(FramebufferHandle framebuffer) : m_Framebuffer(framebuffer)
    {
    }

    void SetFramebufferCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_Framebuffer, device);
    }

    SetViewportCommandImpl::SetViewportCommandImpl(const Viewport &viewport) : m_Viewport(viewport)
    {
    }

    void SetViewportCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_Viewport, device);
    }

    SetScissorCommandImpl::SetScissorCommandImpl(const Scissor &scissor) : m_Scissor(scissor)
    {
    }

    void SetScissorCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_Scissor, device);
    }

    ResolveFramebufferCommandImpl::ResolveFramebufferCommandImpl(const ResolveTextureDescription &desc)
        : m_CommandData(desc)
    {
    }

    void ResolveFramebufferCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_CommandData, device);
    }

    StartTimingQueryCommandImpl::StartTimingQueryCommandImpl(TimingQueryHandle query) : m_Query(query)
    {
    }

    void StartTimingQueryCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        StartTimingQueryCommand command{.Query = m_Query};
        executor->ExecuteCommand(command, device);
    }

    EndTimingQueryCommandImpl::EndTimingQueryCommandImpl(TimingQueryHandle query) : m_Query(query)
    {
    }

    void EndTimingQueryCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        StopTimingQueryCommand command{.Query = m_Query};
        executor->ExecuteCommand(command, device);
    }

    CopyBufferToBufferCommandImpl::CopyBufferToBufferCommandImpl(const BufferCopyDescription &desc) : m_Desc(desc)
    {
    }

    void CopyBufferToBufferCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        CopyBufferToBufferCommand command{.BufferCopy = m_Desc};
        executor->ExecuteCommand(command, device);
    }

    CopyBufferToTextureCommandImpl::CopyBufferToTextureCommandImpl(const BufferTextureCopyDescription &desc)
        : m_Desc(desc)
    {
    }

    void CopyBufferToTextureCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        CopyBufferToTextureCommand command{.BufferTextureCopy = m_Desc};
        executor->ExecuteCommand(command, device);
    }

    CopyTextureToBufferCommandImpl::CopyTextureToBufferCommandImpl(const BufferTextureCopyDescription &desc)
        : m_Desc(desc)
    {
    }

    void CopyTextureToBufferCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        CopyTextureToBufferCommand command{.TextureBufferCopy = m_Desc};
        executor->ExecuteCommand(command, device);
    }

    CopyTextureToTextureCommandImpl::CopyTextureToTextureCommandImpl(const TextureCopyDescription &desc) : m_Desc(desc)
    {
    }

    void CopyTextureToTextureCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        CopyTextureToTextureCommand command{.TextureCopy = m_Desc};
        executor->ExecuteCommand(command, device);
    }

    BeginDebugGroupCommandImpl::BeginDebugGroupCommandImpl(const std::string &name) : m_Name(name)
    {
    }

    void BeginDebugGroupCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        BeginDebugGroupCommand command{.GroupName = m_Name};
        executor->ExecuteCommand(command, device);
    }

    void EndDebugGroupCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        EndDebugGroupCommand command{};
        executor->ExecuteCommand(command, device);
    }

    InsertDebugMarkerCommandImpl::InsertDebugMarkerCommandImpl(const std::string &name) : m_Name(name)
    {
    }

    void InsertDebugMarkerCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        InsertDebugMarkerCommand command{.MarkerName = m_Name};
        executor->ExecuteCommand(command, device);
    }

    SetBlendFactorCommandImpl::SetBlendFactorCommandImpl(const BlendFactorDesc &desc) : m_CommandData(desc)
    {
    }

    void SetBlendFactorCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        SetBlendFactorCommand command{.BlendFactor = m_CommandData};
        executor->ExecuteCommand(command, device);
    }

    SetStencilReferenceCommandImpl::SetStencilReferenceCommandImpl(uint32_t stencilReference)
        : m_StencilReference(stencilReference)
    {
    }

    void SetStencilReferenceCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        SetStencilReferenceCommand command{.StencilReference = m_StencilReference};
        executor->ExecuteCommand(command, device);
    }

    BuildAccelerationStructuresCommandImpl::BuildAccelerationStructuresCommandImpl(
        const std::vector<AccelerationStructureGeometryBuildDescription> &description
    )
        : m_Description(description)
    {
    }

    void BuildAccelerationStructuresCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        BuildAccelerationStructuresCommand command{.BuildDescriptions = m_Description};
        executor->ExecuteCommand(command, device);
    }

    CopyAccelerationStructuresCommandImpl::CopyAccelerationStructuresCommandImpl(
        const AccelerationStructureCopyDescription &description
    )
        : m_Description(description)
    {
    }

    void CopyAccelerationStructuresCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_Description, device);
    }

    CopyAccelerationStructureToDeviceBufferCommandImpl::CopyAccelerationStructureToDeviceBufferCommandImpl(
        const AccelerationStructureDeviceBufferCopyDescription &description
    )
        : m_Description(description)
    {
    }

    void CopyAccelerationStructureToDeviceBufferCommandImpl::Execute(
        CommandExecutor *executor, IGraphicsDevice *device
    ) const
    {
        executor->ExecuteCommand(m_Description, device);
    }

    CopyDeviceBufferToAccelerationStructureCommandImpl::CopyDeviceBufferToAccelerationStructureCommandImpl(
        const DeviceBufferAccelerationStructureCopyDescription &description
    )
        : m_Description(description)
    {
    }

    void CopyDeviceBufferToAccelerationStructureCommandImpl::Execute(
        CommandExecutor *executor, IGraphicsDevice *device
    ) const
    {
        executor->ExecuteCommand(m_Description, device);
    }

    PushConstantsCommandImpl::PushConstantsCommandImpl(const PushConstantsDesc &description)
        : m_Description(description)
    {
    }

    void PushConstantsCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_Description, device);
    }

    SubmitBarriersCommandImpl::SubmitBarriersCommandImpl(const BarrierGroupDescription &description)
        : m_Description(description)
    {
    }

    void SubmitBarriersCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_Description, device);
    }

    EndRenderingCommandImpl::EndRenderingCommandImpl(const EndRenderingCommand &command) : m_CommandData(command)
    {
    }

    void EndRenderingCommandImpl::Execute(CommandExecutor *executor, IGraphicsDevice *device) const
    {
        executor->ExecuteCommand(m_CommandData, device);
    }

    void CommandListStorage::Clear()
    {
        Samplers.clear();
        Textures.clear();
        TextureViews.clear();
        TexelBuffers.clear();
        AccelerationStructures.clear();
        TimingQueries.clear();
        Fences.clear();
        Framebuffers.clear();
        ShaderModules.clear();
        ResourceSets.clear();
        Pipelines.clear();
        Surfaces.clear();
        CommandQueues.clear();
        DeviceBuffers.clear();

        CommandData.clear();
    }

    void CommandListStorage::Reset()
    {
        *this = CommandListStorage{};
    }

    void CommandListStorage::Print() const
    {
        size_t offset = 0;

        while (offset < CommandData.size())
        {
            const auto *header = reinterpret_cast<const CommandHeader *>(CommandData.data() + offset);

            header->Print();

            offset += header->Length;
        }
    }

    template <typename T>
    Allocation<T> Allocate(std::vector<std::byte> &commandStream, CommandType type, size_t payloadSize = 0)
    {
        static_assert(std::is_trivially_copyable_v<T>);
        static_assert(std::is_standard_layout_v<T>);

        constexpr size_t HeaderAlign = alignof(CommandHeader);
        constexpr size_t CommandAlign = alignof(T);
        constexpr size_t PayloadAlign = alignof(std::max_align_t);

        auto AlignUp = [](size_t value, size_t alignment) { return (value + alignment - 1) & ~(alignment - 1); };

        // The header is always exactly at the end
        size_t headerOffset = commandStream.size();

        size_t commandOffset = AlignUp(headerOffset + sizeof(CommandHeader), CommandAlign);

        size_t payloadOffset = AlignUp(commandOffset + sizeof(T), PayloadAlign);

        size_t endOffset = payloadOffset + payloadSize;

        commandStream.resize(endOffset);

        auto *header = reinterpret_cast<CommandHeader *>(commandStream.data() + headerOffset);

        header->Type = type;
        header->Length = endOffset - headerOffset;

        return {commandOffset, payloadSize ? payloadOffset : SIZE_MAX};
    }

    void CommandListStorage::SetVertexBuffer(VertexBufferView vertexBuffer, uint32_t slot)
    {
        auto alloc = Allocate<SetVertexBufferCommandStorage>(CommandData, CommandType::SetVertexBuffer);
        auto *command = alloc.GetCommand(CommandData);

        command->DeviceBufferIndex = DeviceBuffers.size();
        command->Offset = vertexBuffer.Offset;
        command->Size = vertexBuffer.Size;
        command->Slot = slot;

        DeviceBuffers.push_back(vertexBuffer.BufferHandle);
    }

    void CommandListStorage::SetIndexBuffer(IndexBufferView indexBuffer)
    {
        auto alloc = Allocate<SetIndexBufferCommandStorage>(CommandData, CommandType::SetIndexBuffer);
        auto *command = alloc.GetCommand(CommandData);

        command->DeviceBufferIndex = DeviceBuffers.size();
        command->Offset = indexBuffer.Offset;
        command->Size = indexBuffer.Size;
        command->BufferFormat = indexBuffer.BufferFormat;

        DeviceBuffers.push_back(indexBuffer.BufferHandle);
    }

    void CommandListStorage::SetPipeline(PipelineHandle pipeline)
    {
        auto alloc = Allocate<SetPipelineCommandStorage>(CommandData, CommandType::SetPipeline);
        auto *command = alloc.GetCommand(CommandData);
        command->PipelineIndex = Pipelines.size();

        Pipelines.push_back(pipeline);
    }

    void CommandListStorage::Draw(const DrawDescription &desc)
    {
        auto alloc = Allocate<DrawDescription>(CommandData, CommandType::Draw);
        auto *command = alloc.GetCommand(CommandData);

        command->VertexCount = desc.VertexCount;
        command->InstanceCount = desc.InstanceCount;
        command->VertexStart = desc.VertexStart;
        command->InstanceStart = desc.InstanceStart;
    }

    void CommandListStorage::DrawIndexed(const DrawIndexedDescription &desc)
    {
        auto alloc = Allocate<DrawIndexedDescription>(CommandData, CommandType::DrawIndexed);
        auto *command = alloc.GetCommand(CommandData);

        command->IndexCount = desc.IndexCount;
        command->InstanceCount = desc.InstanceCount;
        command->VertexStart = desc.VertexStart;
        command->IndexStart = desc.IndexStart;
        command->InstanceStart = desc.InstanceStart;
    }

    void CommandListStorage::DrawIndirect(const DrawIndirectDescription &desc)
    {
        auto alloc = Allocate<DrawIndirectCommandStorage>(CommandData, CommandType::DrawIndirect);
        auto *command = alloc.GetCommand(CommandData);

        command->DeviceBufferIndex = DeviceBuffers.size();
        command->Offset = desc.Offset;
        command->Stride = desc.Stride;
        command->DrawCount = desc.DrawCount;

        DeviceBuffers.push_back(desc.IndirectBuffer);
    }

    void CommandListStorage::DrawIndexedIndirect(const DrawIndirectIndexedDescription &desc)
    {
        // the same structure can be used for both indirect and indexed indirect draw commands
        auto alloc = Allocate<DrawIndirectCommandStorage>(CommandData, CommandType::DrawIndexedIndirect);
        auto *command = alloc.GetCommand(CommandData);

        command->DeviceBufferIndex = DeviceBuffers.size();
        command->Offset = desc.Offset;
        command->Stride = desc.Stride;
        command->DrawCount = desc.DrawCount;

        DeviceBuffers.push_back(desc.IndirectBuffer);
    }

    void CommandListStorage::Dispatch(const DispatchDescription &desc)
    {
        auto alloc = Allocate<DispatchDescription>(CommandData, CommandType::Dispatch);
        auto *command = alloc.GetCommand(CommandData);

        command->WorkGroupCountX = desc.WorkGroupCountX;
        command->WorkGroupCountY = desc.WorkGroupCountY;
        command->WorkGroupCountZ = desc.WorkGroupCountZ;
    }

    void CommandListStorage::DispatchIndirect(const DispatchIndirectDescription &desc)
    {
        auto alloc = Allocate<DispatchIndirectCommandStorage>(CommandData, CommandType::DispatchIndirect);
        auto *command = alloc.GetCommand(CommandData);

        command->DeviceBufferIndex = DeviceBuffers.size();
        command->Offset = desc.Offset;
        command->Stride = desc.Stride;

        DeviceBuffers.push_back(desc.IndirectBuffer);
    }

    void CommandListStorage::DrawMesh(const DrawMeshDescription &desc)
    {
        auto alloc = Allocate<DrawMeshDescription>(CommandData, CommandType::DrawMesh);
        auto *command = alloc.GetCommand(CommandData);

        command->WorkGroupCountX = desc.WorkGroupCountX;
        command->WorkGroupCountY = desc.WorkGroupCountY;
        command->WorkGroupCountZ = desc.WorkGroupCountZ;
    }

    void CommandListStorage::DrawMeshIndirect(const DrawMeshIndirectDescription &desc)
    {
        auto alloc = Allocate<DrawMeshIndirectCommandStorage>(CommandData, CommandType::DrawMeshIndirect);
        auto *command = alloc.GetCommand(CommandData);

        command->DeviceBufferIndex = DeviceBuffers.size();
        command->Offset = desc.Offset;
        command->Stride = desc.Stride;
        command->DrawCount = desc.DrawCount;

        DeviceBuffers.push_back(desc.IndirectBuffer);
    }

    void CommandListStorage::TraceRays(const TraceRaysDescription &desc)
    {
        auto alloc = Allocate<TraceRaysDescription>(CommandData, CommandType::TraceRays);
        auto *command = alloc.GetCommand(CommandData);

        command->RaygenRegion = desc.RaygenRegion;
        command->MissRegion = desc.MissRegion;
        command->HitRegion = desc.HitRegion;
        command->CallableRegion = desc.CallableRegion;
        command->Width = desc.Width;
        command->Height = desc.Height;
        command->Depth = desc.Depth;
    }

    void CommandListStorage::SetResourceSet(const ResourceSetBindingDescription &desc)
    {
        size_t payloadSize = 0;

        for (const auto &[name, offset] : desc.DynamicOffsets)
        {
            payloadSize += name.size();
            payloadSize += offset.size() * sizeof(uint32_t);
        }

        auto alloc =
            Allocate<ResourceSetBindingCommandStorage>(CommandData, CommandType::ResourceSetBinding, payloadSize);
        auto *command = alloc.GetCommand(CommandData);
        command->ResourceSetIndex = ResourceSets.size();
        command->DynamicOffsetCount = desc.DynamicOffsets.size();

        std::byte *rawPtr = alloc.GetPayload(CommandData);
        for (const auto &[name, dynamicOffset] : desc.DynamicOffsets)
        {
            memcpy(rawPtr, name.data(), name.size());
            rawPtr += name.size();
            memcpy(rawPtr, dynamicOffset.data(), dynamicOffset.size() * sizeof(uint32_t));
            rawPtr += dynamicOffset.size() * sizeof(uint32_t);
        }

        ResourceSets.push_back(desc.TargetResourceSet);
    }

    void CommandListStorage::ClearColourTarget(uint32_t index, const ClearColourValue &color, ClearRect clearRect)
    {
        auto alloc = Allocate<ClearColorTargetCommand>(CommandData, CommandType::ClearColourTarget);
        auto *command = alloc.GetCommand(CommandData);

        command->Index = index;
        command->Colour = color;
        command->Rect = clearRect;
    }

    void CommandListStorage::ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect)
    {
        auto alloc = Allocate<ClearDepthStencilTargetCommand>(CommandData, CommandType::ClearDepthTarget);
        auto *command = alloc.GetCommand(CommandData);

        command->Value = value;
        command->Rect = clearRect;
    }

    void CommandListStorage::SetFramebuffer(FramebufferHandle framebuffer)
    {
        auto alloc = Allocate<FramebufferCommandStorage>(CommandData, CommandType::SetFramebuffer);
        auto *command = alloc.GetCommand(CommandData);
        command->FramebufferIndex = Framebuffers.size();

        Framebuffers.push_back(framebuffer);
    }

    void CommandListStorage::SetViewport(const Viewport &viewport)
    {
        auto alloc = Allocate<Viewport>(CommandData, CommandType::Viewport);
        auto *command = alloc.GetCommand(CommandData);

        command->X = viewport.X;
        command->Y = viewport.Y;
        command->Width = viewport.Width;
        command->Height = viewport.Height;
        command->MinDepth = viewport.MinDepth;
        command->MaxDepth = viewport.MaxDepth;
    }

    void CommandListStorage::SetScissor(const Scissor &scissor)
    {
        auto alloc = Allocate<Scissor>(CommandData, CommandType::Scissor);
        auto *command = alloc.GetCommand(CommandData);

        command->X = scissor.X;
        command->Y = scissor.Y;
        command->Width = scissor.Width;
        command->Height = scissor.Height;
    }

    void CommandListStorage::ResolveFramebuffer(const ResolveTextureDescription &desc)
    {
        auto alloc = Allocate<ResolveTextureCommandStorage>(CommandData, CommandType::ResolveFramebuffer);
        auto *command = alloc.GetCommand(CommandData);

        command->SourceTextureIndex = Textures.size();
        command->DestinationTextureIndex = Textures.size() + 1;
        command->SourceArrayLayer = desc.SourceArrayLayer;
        command->SourceMipLevel = desc.SourceMipLevel;
        command->DestinationArrayLayer = desc.DestinationArrayLayer;
        command->DestinationMipLevel = desc.DestinationMipLevel;

        Textures.push_back(desc.Source);
        Textures.push_back(desc.Destination);
    }

    void CommandListStorage::StartTimingQuery(TimingQueryHandle query)
    {
        auto alloc = Allocate<TimingQueryCommandStorage>(CommandData, CommandType::StartTimingQuery);
        auto *command = alloc.GetCommand(CommandData);
        command->QueryIndex = TimingQueries.size();

        TimingQueries.push_back(query);
    }

    void CommandListStorage::StopTimingQuery(TimingQueryHandle query)
    {
        auto alloc = Allocate<TimingQueryCommandStorage>(CommandData, CommandType::StopTimingQuery);
        auto *command = alloc.GetCommand(CommandData);
        command->QueryIndex = TimingQueries.size();

        TimingQueries.push_back(query);
    }

    void CommandListStorage::CopyBufferToBuffer(const BufferCopyDescription &bufferCopy)
    {
        size_t payloadSize = bufferCopy.Copies.size() * sizeof(BufferCopy);

        auto alloc = Allocate<BufferCopyCommandStorage>(CommandData, CommandType::CopyBufferToBuffer, payloadSize);
        auto *command = alloc.GetCommand(CommandData);
        auto *payload = alloc.GetPayload(CommandData);

        command->SourceIndex = DeviceBuffers.size();
        command->DestinationIndex = DeviceBuffers.size() + 1;
        memcpy(payload, bufferCopy.Copies.data(), payloadSize);

        DeviceBuffers.push_back(bufferCopy.Source);
        DeviceBuffers.push_back(bufferCopy.Destination);
    }

    void CommandListStorage::CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy)
    {
        auto alloc = Allocate<BufferTextureCopyCommandStorage>(CommandData, CommandType::CopyBufferToTexture);
        auto *command = alloc.GetCommand(CommandData);

        command->BufferIndex = DeviceBuffers.size();
        command->BufferOffset = bufferTextureCopy.BufferOffset;
        command->BufferRowLength = bufferTextureCopy.BufferRowLength;
        command->BufferImageHeight = bufferTextureCopy.BufferImageHeight;
        command->TextureIndex = Textures.size();
        command->TextureOffset = bufferTextureCopy.TextureOffset;
        command->TextureExtent = bufferTextureCopy.TextureExtent;
        command->MipLevel = bufferTextureCopy.MipLevel;

        DeviceBuffers.push_back(bufferTextureCopy.BufferHandle);
        Textures.push_back(bufferTextureCopy.Texture);
    }

    void CommandListStorage::CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy)
    {
        auto alloc = Allocate<BufferTextureCopyCommandStorage>(CommandData, CommandType::CopyTextureToBuffer);
        auto *command = alloc.GetCommand(CommandData);

        command->BufferIndex = DeviceBuffers.size();
        command->BufferOffset = textureBufferCopy.BufferOffset;
        command->BufferRowLength = textureBufferCopy.BufferRowLength;
        command->BufferImageHeight = textureBufferCopy.BufferImageHeight;
        command->TextureIndex = Textures.size();
        command->TextureOffset = textureBufferCopy.TextureOffset;
        command->TextureExtent = textureBufferCopy.TextureExtent;
        command->MipLevel = textureBufferCopy.MipLevel;

        DeviceBuffers.push_back(textureBufferCopy.BufferHandle);
        Textures.push_back(textureBufferCopy.Texture);
    }

    void CommandListStorage::CopyTextureToTexture(const TextureCopyDescription &textureCopy)
    {
        auto alloc = Allocate<TextureCopyCommandStorage>(CommandData, CommandType::CopyTextureToTexture);
        auto *command = alloc.GetCommand(CommandData);

        command->SourceTextureIndex = Textures.size();
        command->DestinationTextureIndex = Textures.size() + 1;
        command->SourceOffset = textureCopy.SourceOffset;
        command->DestinationOffset = textureCopy.DestinationOffset;
        command->Extent = textureCopy.Extent;
        command->SourceMipLevel = textureCopy.SourceMipLevel;
        command->DestinationMipLevel = textureCopy.DestinationMipLevel;

        Textures.push_back(textureCopy.Source);
        Textures.push_back(textureCopy.Destination);
    }

    void CommandListStorage::BeginDebugGroup(const std::string &name)
    {
        size_t payloadSize = name.size();

        auto alloc = Allocate<size_t>(CommandData, CommandType::BeginDebugGroup, payloadSize);
        auto *command = alloc.GetCommand(CommandData);
        auto *payload = alloc.GetPayload(CommandData);

        *command = payloadSize;
        memcpy(payload, name.data(), name.size());
    }

    void CommandListStorage::EndDebugGroup()
    {
        auto alloc = Allocate<DebugGroupCommandStorage>(CommandData, CommandType::EndDebugGroup);
    }

    void CommandListStorage::InsertDebugMarker(const std::string &name)
    {
        auto alloc = Allocate<DebugLabelCommandStorage>(CommandData, CommandType::DebugLabel, name.size());
        auto *command = alloc.GetCommand(CommandData);
        auto *payload = alloc.GetPayload(CommandData);
        command->TextLength = name.size();

        memcpy(payload, name.data(), name.size());
    }

    void CommandListStorage::SetBlendFactor(const BlendFactorDesc &blendFactor)
    {
        auto alloc = Allocate<BlendFactorDesc>(CommandData, CommandType::SetBlendFactor);
        auto *command = alloc.GetCommand(CommandData);
        *command = blendFactor;
    }

    void CommandListStorage::SetStencilReference(uint32_t stencilReference)
    {
        auto alloc = Allocate<uint32_t>(CommandData, CommandType::SetStencilReference);
        auto *command = alloc.GetCommand(CommandData);
        *command = stencilReference;
    }

    void CommandListStorage::BuildAccelerationStructures(
        const std::vector<AccelerationStructureGeometryBuildDescription> &descriptions
    )
    {
        size_t payloadSize = sizeof(AccelerationStructureGeometryBuildCountStorage);

        for (const auto &build : descriptions)
        {
            payloadSize += sizeof(AccelerationStructureGeometryBuildCommandStorage);
            payloadSize += build.Geometry.size() * sizeof(AccelerationStructureGeometryDescription);
            payloadSize += build.PrimitiveCounts.size() * sizeof(uint32_t);
        }

        auto alloc = Allocate<AccelerationStructureGeometryBuildCountStorage>(
            CommandData, CommandType::BuildAccelerationStructures, payloadSize
        );
        auto *command = alloc.GetCommand(CommandData);
        auto *payload = alloc.GetPayload(CommandData);
        command->BuildCount = descriptions.size();

        std::byte *ptr = payload + sizeof(AccelerationStructureGeometryBuildCountStorage);

        for (const auto &build : descriptions)
        {
            auto *command = reinterpret_cast<AccelerationStructureGeometryBuildCommandStorage *>(ptr);

            ptr += sizeof(AccelerationStructureGeometryBuildCommandStorage);

            command->Type = build.Type;
            command->Flags = build.Flags;
            command->Mode = build.Mode;
            command->ScratchBuffer = build.ScratchBuffer;

            command->SourceIndex = AccelerationStructures.size();
            AccelerationStructures.push_back(build.Source);

            command->DestinationIndex = AccelerationStructures.size();
            AccelerationStructures.push_back(build.Destination);

            const size_t geometryBytes = build.Geometry.size() * sizeof(AccelerationStructureGeometryDescription);

            command->GeometryOffset = sizeof(AccelerationStructureGeometryBuildCommandStorage);
            command->GeometryCount = build.Geometry.size();

            if (geometryBytes > 0)
            {
                memcpy(ptr, build.Geometry.data(), geometryBytes);
                ptr += geometryBytes;
            }

            const size_t primitiveBytes = build.PrimitiveCounts.size() * sizeof(uint32_t);

            command->PrimitiveOffset = command->GeometryOffset + geometryBytes;
            command->PrimitiveCount = build.PrimitiveCounts.size();

            if (primitiveBytes > 0)
            {
                memcpy(ptr, build.PrimitiveCounts.data(), primitiveBytes);
                ptr += primitiveBytes;
            }
        }
    }

    void CommandListStorage::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
    {
        auto alloc =
            Allocate<AccelerationStructureCopyDescription>(CommandData, CommandType::CopyAccelerationStructure);
        auto *command = alloc.GetCommand(CommandData);
        *command = description;
    }

    void CommandListStorage::CopyAccelerationStructureToDeviceBuffer(
        const AccelerationStructureDeviceBufferCopyDescription &description
    )
    {
        auto alloc = Allocate<AccelerationStructureDeviceBufferCopyDescription>(
            CommandData, CommandType::CopyAccelerationStructure
        );
        auto *command = alloc.GetCommand(CommandData);
        *command = description;
    }

    void CommandListStorage::CopyDeviceBufferToAccelerationStructure(
        const DeviceBufferAccelerationStructureCopyDescription &description
    )
    {
        auto alloc = Allocate<DeviceBufferAccelerationStructureCopyDescription>(
            CommandData, CommandType::CopyAccelerationStructure
        );
        auto *command = alloc.GetCommand(CommandData);
        *command = description;
    }

    void CommandListStorage::WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset)
    {
        size_t payloadSize = name.size() + size;

        auto alloc = Allocate<PushConstantsCommandStorage>(CommandData, CommandType::PushConstants, payloadSize);
        auto *command = alloc.GetCommand(CommandData);
        auto *payload = alloc.GetPayload(CommandData);

        command->NameLength = name.size();
        command->Offset = offset;
        command->DataLength = size;

        std::byte *rawPtr = payload;
        memcpy(rawPtr, name.data(), name.size());
        rawPtr += name.size();
        memcpy(rawPtr, data, size);
    }

    void CommandListStorage::SubmitBarrierGroup(const BarrierGroupDescription &description)
    {
        size_t payloadSize = (description.MemoryBarriers.size() * sizeof(MemoryBarrierDesc)) +
                             (description.TextureBarriers.size() * sizeof(TextureBarrierCommandStorage)) +
                             (description.BufferBarriers.size() * sizeof(BufferBarrierCommandStorage));

        auto alloc = Allocate<BarrierGroupCommandStorage>(CommandData, CommandType::BarrierGroup, payloadSize);
        auto *command = alloc.GetCommand(CommandData);
        auto *payload = alloc.GetPayload(CommandData);

        command->MemoryBarrierCount = description.MemoryBarriers.size();
        command->TextureBarrierCount = description.TextureBarriers.size();
        command->BufferBarrierCount = description.BufferBarriers.size();

        // write memory barriers
        std::byte *rawPtr = payload;
        memcpy(
            rawPtr, description.MemoryBarriers.data(), description.MemoryBarriers.size() * sizeof(MemoryBarrierDesc)
        );
        rawPtr += description.MemoryBarriers.size() * sizeof(MemoryBarrierDesc);

        // write texture barriers
        for (const auto &textureBarrier : description.TextureBarriers)
        {
            TextureBarrierCommandStorage barrier = {};
            barrier.TextureIndex = Textures.size();
            barrier.Layout = textureBarrier.Layout;
            barrier.BeforeAccess = textureBarrier.BeforeAccess;
            barrier.AfterAccess = textureBarrier.AfterAccess;
            barrier.BeforeStage = textureBarrier.BeforeStage;
            barrier.AfterStage = textureBarrier.AfterStage;
            barrier.TextureSubresourceRange = textureBarrier.TextureSubresourceRange;
            memcpy(rawPtr, &barrier, sizeof(barrier));

            Textures.push_back(textureBarrier.Texture);

            rawPtr += sizeof(TextureBarrierCommandStorage);
        }

        // write buffer barriers
        for (const auto &bufferBarrier : description.BufferBarriers)
        {
            BufferBarrierCommandStorage barrier = {};
            barrier.BufferIndex = DeviceBuffers.size();
            barrier.BeforeAccess = bufferBarrier.BeforeAccess;
            barrier.AfterAccess = bufferBarrier.AfterAccess;
            barrier.BeforeStage = bufferBarrier.BeforeStage;
            barrier.AfterStage = bufferBarrier.AfterStage;
            barrier.Offset = bufferBarrier.Offset;
            barrier.Size = bufferBarrier.Size;
            memcpy(rawPtr, &barrier, sizeof(barrier));

            DeviceBuffers.push_back(bufferBarrier.Buffer);

            rawPtr += sizeof(BufferBarrierCommandStorage);
        }
    }

    ICommandList::ICommandList(const CommandListDescription &spec)
        : m_Description(spec), m_AutomaticBarrierTracking(spec.AutomaticBarrierTransitions)
    {
    }

    ICommandList::~ICommandList()
    {
        m_CommandImpls.clear();
    }

    void ICommandList::Begin()
    {
        if (m_Started)
        {
            PushError(
                "Attempting to begin a command into a CommandList that has not "
                "been closed"
            );
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.clear();
        m_CommandListStorage.Clear();
        m_Started = true;
        m_DebugGroups = 0;
        m_AutomaticBarrierTracking = m_Description.AutomaticBarrierTransitions;
        m_Barriers.Clear();
    }

    void ICommandList::End()
    {
        if (!m_Started)
        {
            PushError("Attempting to end a CommandList but the CommandList was not begun");
        }

        // just in case
        EndRendering();

        // clean up any unclosed debug groups
        for (uint32_t i = 0; i < m_DebugGroups; i++)
        {
            EndDebugGroup();
        }

        // reset the debug group counter
        m_DebugGroups = 0;

        // end recording into the CommandList
        m_Started = false;
    }

    void ICommandList::BeginAutomaticBarrierManagement()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_AutomaticBarrierTracking = true;
    }

    void ICommandList::EndAutomaticBarrierManagement()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_AutomaticBarrierTracking = false;
    }

    void ICommandList::SetVertexBuffer(VertexBufferView vertexBuffer, uint32_t slot)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        SetVertexBufferCommand command;
        command.View = vertexBuffer;
        command.Slot = slot;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetVertexBufferCommandImpl>(command));

        m_CommandListStorage.SetVertexBuffer(vertexBuffer, slot);
    }

    void ICommandList::SetIndexBuffer(IndexBufferView indexBuffer)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        SetIndexBufferCommand command;
        command.View = indexBuffer;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetIndexBufferCommandImpl>(indexBuffer));

        m_CommandListStorage.SetIndexBuffer(indexBuffer);
    }

    void ICommandList::SetPipeline(PipelineHandle pipeline)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetPipelineCommandImpl>(pipeline));

        m_CommandListStorage.SetPipeline(pipeline);
    }

    void ICommandList::Draw(const DrawDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<DrawCommandImpl>(desc));

        m_CommandListStorage.Draw(desc);
    }

    void ICommandList::DrawIndexed(const DrawIndexedDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<DrawIndexedCommandImpl>(desc));

        m_CommandListStorage.DrawIndexed(desc);
    }

    void ICommandList::DrawIndirect(const DrawIndirectDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<DrawIndirectCommandImpl>(desc));

        m_CommandListStorage.DrawIndirect(desc);
    }

    void ICommandList::DrawIndexedIndirect(const DrawIndirectIndexedDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<DrawIndexedIndirectCommandImpl>(desc));

        m_CommandListStorage.DrawIndexedIndirect(desc);
    }

    void ICommandList::Dispatch(const DispatchDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<DispatchCommandImpl>(desc));

        m_CommandListStorage.Dispatch(desc);
    }

    void ICommandList::DispatchIndirect(const DispatchIndirectDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<DispatchIndirectCommandImpl>(desc));

        m_CommandListStorage.DispatchIndirect(desc);
    }

    void ICommandList::DrawMesh(const DrawMeshDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<DrawMeshCommandImpl>(desc));

        m_CommandListStorage.DrawMesh(desc);
    }

    void ICommandList::DrawMeshIndirect(const DrawMeshIndirectDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<DrawMeshIndirectCommandImpl>(desc));

        m_CommandListStorage.DrawMeshIndirect(desc);
    }

    void ICommandList::TraceRays(const TraceRaysDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<TraceRaysCommandImpl>(desc));

        m_CommandListStorage.TraceRays(desc);
    }

    void ICommandList::SetResourceSet(const ResourceSetBindingDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        if (m_AutomaticBarrierTracking)
        {
            const auto &boundResources = desc.TargetResourceSet->GetBoundResources();

            for (const auto &[name, ciSamplers] : boundResources.CombinedImageSamplers)
            {
                for (const CombinedImageSampler &ciSampler : ciSamplers)
                {
                    if (ciSampler.ImageSampler.IsValid() && ciSampler.ImageTexture.IsValid())
                    {
                        TextureViewHandle textureView = ciSampler.ImageTexture;
                        const TextureViewDescription &viewDesc = textureView->GetDescription();

                        TextureBarrierDesc barrier = {};
                        barrier.BeforeAccess = BarrierAccess::NoAccess;
                        barrier.AfterAccess = BarrierAccess::ShaderRead;
                        barrier.BeforeStage = BarrierPipelineStage::NoStage;
                        barrier.AfterStage = BarrierPipelineStage::AllGraphics;
                        barrier.Texture = textureView->GetTexture();
                        barrier.Layout = TextureLayout::ShaderReadOnlyOptimal;
                        barrier.TextureSubresourceRange.BaseArrayLayer = viewDesc.Range.BaseArrayLayer;
                        barrier.TextureSubresourceRange.LayerCount = viewDesc.Range.LayerCount;
                        barrier.TextureSubresourceRange.BaseMipLevel = viewDesc.Range.BaseMipLevel;
                        barrier.TextureSubresourceRange.LevelCount = viewDesc.Range.LevelCount;
                        SubmitTextureBarrier(barrier);
                    }
                }
            }

            for (const auto &[name, imageViews] : boundResources.SampledImages)
            {
                for (TextureViewHandle imageView : imageViews)
                {
                    const TextureViewDescription &viewDesc = imageView->GetDescription();

                    TextureBarrierDesc barrier = {};
                    barrier.BeforeAccess = BarrierAccess::NoAccess;
                    barrier.AfterAccess = BarrierAccess::ShaderRead;
                    barrier.BeforeStage = BarrierPipelineStage::NoStage;
                    barrier.AfterStage = BarrierPipelineStage::AllGraphics;
                    barrier.Texture = imageView->GetTexture();
                    barrier.Layout = TextureLayout::ShaderReadOnlyOptimal;
                    barrier.TextureSubresourceRange.BaseArrayLayer = viewDesc.Range.BaseArrayLayer;
                    barrier.TextureSubresourceRange.LayerCount = viewDesc.Range.LayerCount;
                    barrier.TextureSubresourceRange.BaseMipLevel = viewDesc.Range.BaseMipLevel;
                    barrier.TextureSubresourceRange.LevelCount = viewDesc.Range.LevelCount;
                    SubmitTextureBarrier(barrier);
                }
            }

            for (const auto &[name, storageImages] : boundResources.StorageImages)
            {
                for (const StorageImageView &storageImage : storageImages)
                {
                    TextureBarrierDesc barrier = {};
                    barrier.BeforeAccess = BarrierAccess::NoAccess;
                    barrier.AfterAccess = BarrierAccess::ShaderRead;
                    barrier.BeforeStage = BarrierPipelineStage::NoStage;
                    barrier.AfterStage = BarrierPipelineStage::AllGraphics;
                    barrier.Texture = storageImage.Texture;
                    barrier.Layout = TextureLayout::General;
                    barrier.TextureSubresourceRange.BaseArrayLayer = storageImage.ArrayLayer;
                    barrier.TextureSubresourceRange.LayerCount = 1;
                    barrier.TextureSubresourceRange.BaseMipLevel = storageImage.MipLevel;
                    barrier.TextureSubresourceRange.LevelCount = 1;
                    SubmitTextureBarrier(barrier);
                }
            }
        }

        FlushBarriers();

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetResourceSetCommandImpl>(desc));

        m_CommandListStorage.SetResourceSet(desc);
    }

    void ICommandList::ClearColourTarget(uint32_t index, const ClearColourValue &color, ClearRect clearRect)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        ClearColorTargetCommand command;
        command.Index = index;
        command.Colour = color;
        command.Rect = clearRect;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<ClearColourTargetCommandImpl>(command));

        m_CommandListStorage.ClearColourTarget(index, color, clearRect);
    }

    void ICommandList::ClearColourTarget(uint32_t index, const ClearColourValue &color)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        ClearColorTargetCommand command;
        command.Index = index;
        command.Colour = color;
        command.Rect = std::nullopt;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<ClearColourTargetCommandImpl>(command));

        m_CommandListStorage.ClearColourTarget(index, color, {});
    }

    void ICommandList::ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        ClearDepthStencilTargetCommand command;
        command.Value = value;
        command.Rect = clearRect;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<ClearDepthStencilTargetCommandImpl>(command));

        m_CommandListStorage.ClearDepthTarget(value, clearRect);
    }

    void ICommandList::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        ClearDepthStencilTargetCommand command;
        command.Value = value;
        command.Rect = std::nullopt;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<ClearDepthStencilTargetCommandImpl>(command));

        m_CommandListStorage.ClearDepthTarget(value, {});
    }

    static void TransitionFramebufferLayouts(
        ICommandList *commandList, FramebufferHandle framebuffer, TextureLayout colourLayout, TextureLayout depthLayout
    )
    {
        // transition colour attachment layouts
        for (size_t i = 0; i < framebuffer->GetColorTextureCount(); i++)
        {
            std::optional<FramebufferColourAttachmentDescription> colourAttachmentOpt =
                framebuffer->GetColorTextureBinding(i);
            if (colourAttachmentOpt.has_value())
            {
                FramebufferColourAttachmentDescription colourAttachment = colourAttachmentOpt.value();

                TextureBarrierDesc barrierDesc = {};
                barrierDesc.Texture = colourAttachment.ColourAttachment.TargetTexture;
                barrierDesc.BeforeAccess = BarrierAccess::NoAccess;
                barrierDesc.AfterAccess = BarrierAccess::ColourAttachmentWrite;
                barrierDesc.BeforeStage = BarrierPipelineStage::NoStage;
                barrierDesc.AfterStage = BarrierPipelineStage::ColourAttachmentOutput;
                barrierDesc.Layout = colourLayout;
                barrierDesc.TextureSubresourceRange = {
                    .BaseMipLevel = colourAttachment.ColourAttachment.MipLevel,
                    .LevelCount = 1,
                    .BaseArrayLayer = colourAttachment.ColourAttachment.BaseArrayLayer,
                    .LayerCount = colourAttachment.ColourAttachment.LayerCount
                };

                commandList->SubmitTextureBarrier(barrierDesc);

                if (colourAttachment.ResolveAttachment.has_value())
                {
                    FramebufferTextureDescription resolveAttachmentDesc = colourAttachment.ResolveAttachment.value();

                    TextureBarrierDesc barrierDesc = {};
                    barrierDesc.Texture = resolveAttachmentDesc.TargetTexture;
                    barrierDesc.BeforeAccess = BarrierAccess::ColourAttachmentWrite;
                    barrierDesc.AfterAccess = BarrierAccess::NoAccess;
                    barrierDesc.BeforeStage = BarrierPipelineStage::ColourAttachmentOutput;
                    barrierDesc.AfterStage = BarrierPipelineStage::AllGraphics;
                    barrierDesc.Layout = colourLayout;
                    barrierDesc.TextureSubresourceRange = {
                        .BaseMipLevel = resolveAttachmentDesc.MipLevel,
                        .LevelCount = 1,
                        .BaseArrayLayer = resolveAttachmentDesc.BaseArrayLayer,
                        .LayerCount = resolveAttachmentDesc.LayerCount
                    };

                    commandList->SubmitTextureBarrier(barrierDesc);
                }
            }
        }

        // transition depth attachment layout if exists
        if (std::optional<FramebufferTextureDescription> depthAttachmentOpt = framebuffer->GetDepthTextureBinding())
        {
            FramebufferTextureDescription depthAttachment = depthAttachmentOpt.value();

            TextureBarrierDesc barrierDesc = {};
            barrierDesc.Texture = depthAttachment.TargetTexture;
            barrierDesc.BeforeAccess = BarrierAccess::NoAccess;
            barrierDesc.AfterAccess = BarrierAccess::DepthStencilAttachmentWrite;
            barrierDesc.BeforeStage = BarrierPipelineStage::NoStage;
            barrierDesc.AfterStage = BarrierPipelineStage::EarlyFragmentTests;
            barrierDesc.Layout = depthLayout;
            barrierDesc.TextureSubresourceRange = {
                .BaseMipLevel = depthAttachment.MipLevel,
                .LevelCount = 1,
                .BaseArrayLayer = depthAttachment.BaseArrayLayer,
                .LayerCount = depthAttachment.LayerCount
            };

            commandList->SubmitTextureBarrier(barrierDesc);
        }

        commandList->FlushBarriers();
    }

    void ICommandList::SetFramebuffer(FramebufferHandle framebuffer)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        EndRendering();

        TransitionFramebufferLayouts(
            this, framebuffer, TextureLayout::ColourAttachmentOptimal, TextureLayout::DepthStencilAttachmentOptimal
        );
        m_CurrentFramebuffer = framebuffer;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetFramebufferCommandImpl>(framebuffer));

        m_CommandListStorage.SetFramebuffer(framebuffer);
    }

    void ICommandList::SetViewport(const Viewport &viewport)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetViewportCommandImpl>(viewport));

        m_CommandListStorage.SetViewport(viewport);
    }

    void ICommandList::SetScissor(const Scissor &scissor)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetScissorCommandImpl>(scissor));

        m_CommandListStorage.SetScissor(scissor);
    }

    void ICommandList::ResolveFramebuffer(const ResolveTextureDescription &desc)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        if (m_AutomaticBarrierTracking)
        {
            TextureBarrierDesc sourceBarrierDesc = {};
            sourceBarrierDesc.Texture = desc.Source;
            sourceBarrierDesc.BeforeAccess = BarrierAccess::ColourAttachmentWrite;
            sourceBarrierDesc.AfterAccess = BarrierAccess::TransferRead;
            sourceBarrierDesc.BeforeStage = BarrierPipelineStage::ColourAttachmentOutput;
            sourceBarrierDesc.AfterStage = BarrierPipelineStage::Resolve;
            sourceBarrierDesc.Layout = TextureLayout::ResolveSrc;
            sourceBarrierDesc.TextureSubresourceRange = {
                .BaseMipLevel = desc.SourceMipLevel,
                .LevelCount = 1,
                .BaseArrayLayer = desc.SourceArrayLayer,
                .LayerCount = 1
            };
            SubmitTextureBarrier(sourceBarrierDesc);

            TextureBarrierDesc destBarrierDesc = {};
            destBarrierDesc.Texture = desc.Destination;
            destBarrierDesc.BeforeAccess = BarrierAccess::NoAccess;
            destBarrierDesc.AfterAccess = BarrierAccess::TransferWrite;
            destBarrierDesc.BeforeStage = BarrierPipelineStage::NoStage;
            destBarrierDesc.AfterStage = BarrierPipelineStage::Resolve;
            destBarrierDesc.Layout = TextureLayout::ResolveDest;
            destBarrierDesc.TextureSubresourceRange = {
                .BaseMipLevel = desc.DestinationMipLevel,
                .LevelCount = 1,
                .BaseArrayLayer = desc.DestinationArrayLayer,
                .LayerCount = 1
            };
            SubmitTextureBarrier(destBarrierDesc);
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<ResolveFramebufferCommandImpl>(desc));

        m_CommandListStorage.ResolveFramebuffer(desc);
    }

    void Nexus::Graphics::ICommandList::StartTimingQuery(TimingQueryHandle query)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        StartTimingQueryCommand command;
        command.Query = query;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<StartTimingQueryCommandImpl>(query));

        m_CommandListStorage.StartTimingQuery(query);
    }

    void Nexus::Graphics::ICommandList::StopTimingQuery(TimingQueryHandle query)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        StopTimingQueryCommand command;
        command.Query = query;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<EndTimingQueryCommandImpl>(query));

        m_CommandListStorage.StopTimingQuery(query);
    }

    void ICommandList::CopyBufferToBuffer(const BufferCopyDescription &bufferCopy)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        Graphics::CopyBufferToBufferCommand command;
        command.BufferCopy = bufferCopy;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<CopyBufferToBufferCommandImpl>(bufferCopy));

        m_CommandListStorage.CopyBufferToBuffer(bufferCopy);
    }

    void ICommandList::CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        if (m_AutomaticBarrierTracking)
        {
            Graphics::TextureBarrierDesc barrierDesc = {};
            barrierDesc.Texture = bufferTextureCopy.Texture;
            barrierDesc.BeforeAccess = BarrierAccess::NoAccess;
            barrierDesc.AfterAccess = BarrierAccess::TransferWrite;
            barrierDesc.BeforeStage = BarrierPipelineStage::Copy;
            barrierDesc.AfterStage = BarrierPipelineStage::Copy;
            barrierDesc.Layout = TextureLayout::TransferDstOptimal;
            barrierDesc.TextureSubresourceRange = {
                .BaseMipLevel = bufferTextureCopy.MipLevel,
                .LevelCount = 1,
                .BaseArrayLayer = (uint32_t)bufferTextureCopy.TextureOffset.Z,
                .LayerCount = 1
            };
            SubmitTextureBarrier(barrierDesc);
        }

        FlushBarriers();

        Graphics::CopyBufferToTextureCommand command;
        command.BufferTextureCopy = bufferTextureCopy;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<CopyBufferToTextureCommandImpl>(bufferTextureCopy));

        m_CommandListStorage.CopyBufferToTexture(bufferTextureCopy);
    }

    void ICommandList::CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        if (m_AutomaticBarrierTracking)
        {
            Graphics::TextureBarrierDesc barrierDesc = {};
            barrierDesc.Texture = textureBufferCopy.Texture;
            barrierDesc.BeforeAccess = BarrierAccess::NoAccess;
            barrierDesc.AfterAccess = BarrierAccess::TransferRead;
            barrierDesc.BeforeStage = BarrierPipelineStage::Copy;
            barrierDesc.AfterStage = BarrierPipelineStage::Copy;
            barrierDesc.Layout = TextureLayout::TransferSrcOptimal;
            barrierDesc.TextureSubresourceRange = {
                .BaseMipLevel = textureBufferCopy.MipLevel,
                .LevelCount = 1,
                .BaseArrayLayer = (uint32_t)textureBufferCopy.TextureOffset.Z,
                .LayerCount = 1
            };
            SubmitTextureBarrier(barrierDesc);
        }

        FlushBarriers();

        Graphics::CopyTextureToBufferCommand command;
        command.TextureBufferCopy = textureBufferCopy;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<CopyTextureToBufferCommandImpl>(textureBufferCopy));

        m_CommandListStorage.CopyTextureToBuffer(textureBufferCopy);
    }

    void ICommandList::CopyTextureToTexture(const TextureCopyDescription &textureCopy)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        if (m_AutomaticBarrierTracking)
        {
            Graphics::TextureBarrierDesc sourceBarrierDesc = {};
            sourceBarrierDesc.Texture = textureCopy.Source;
            sourceBarrierDesc.BeforeAccess = BarrierAccess::NoAccess;
            sourceBarrierDesc.AfterAccess = BarrierAccess::TransferWrite;
            sourceBarrierDesc.BeforeStage = BarrierPipelineStage::Copy;
            sourceBarrierDesc.AfterStage = BarrierPipelineStage::Copy;
            sourceBarrierDesc.Layout = TextureLayout::TransferSrcOptimal;
            sourceBarrierDesc.TextureSubresourceRange = {
                .BaseMipLevel = textureCopy.SourceMipLevel,
                .LevelCount = 1,
                .BaseArrayLayer = (uint32_t)textureCopy.SourceOffset.Z,
                .LayerCount = 1
            };
            SubmitTextureBarrier(sourceBarrierDesc);

            Graphics::TextureBarrierDesc destBarrierDesc = {};
            destBarrierDesc.Texture = textureCopy.Destination;
            destBarrierDesc.BeforeAccess = BarrierAccess::NoAccess;
            destBarrierDesc.AfterAccess = BarrierAccess::TransferWrite;
            destBarrierDesc.BeforeStage = BarrierPipelineStage::Copy;
            destBarrierDesc.AfterStage = BarrierPipelineStage::Copy;
            destBarrierDesc.Layout = TextureLayout::TransferDstOptimal;
            destBarrierDesc.TextureSubresourceRange = {
                .BaseMipLevel = textureCopy.DestinationMipLevel,
                .LevelCount = 1,
                .BaseArrayLayer = (uint32_t)textureCopy.DestinationOffset.Z,
                .LayerCount = 1
            };
            SubmitTextureBarrier(destBarrierDesc);
        }

        FlushBarriers();

        Graphics::CopyTextureToTextureCommand command;
        command.TextureCopy = textureCopy;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<CopyTextureToTextureCommandImpl>(textureCopy));

        m_CommandListStorage.CopyTextureToTexture(textureCopy);
    }

    void ICommandList::BeginDebugGroup(const std::string &name)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        BeginDebugGroupCommand command;
        command.GroupName = name;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<BeginDebugGroupCommandImpl>(name));

        m_CommandListStorage.BeginDebugGroup(name);

        m_DebugGroups++;
    }

    void ICommandList::EndDebugGroup()
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        EndDebugGroupCommand command;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<EndDebugGroupCommandImpl>());

        m_CommandListStorage.EndDebugGroup();

        m_DebugGroups--;
    }

    void ICommandList::InsertDebugMarker(const std::string &name)
    {
        if (!m_Started)
        {
            PushError(
                "Attempting to record a command into a CommandList without "
                "calling Begin()"
            );
            return;
        }

        InsertDebugMarkerCommand command;
        command.MarkerName = name;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<InsertDebugMarkerCommandImpl>(name));

        m_CommandListStorage.InsertDebugMarker(name);
    }

    void ICommandList::SetBlendFactor(const BlendFactorDesc &blendFactor)
    {
        SetBlendFactorCommand command;
        command.BlendFactor = blendFactor;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetBlendFactorCommandImpl>(blendFactor));

        m_CommandListStorage.SetBlendFactor(blendFactor);
    }

    void ICommandList::SetStencilReference(uint32_t stencilReference)
    {
        SetStencilReferenceCommand command;
        command.StencilReference = stencilReference;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SetStencilReferenceCommandImpl>(stencilReference));

        m_CommandListStorage.SetStencilReference(stencilReference);
    }

    void ICommandList::BuildAccelerationStructures(
        const std::vector<AccelerationStructureGeometryBuildDescription> &description
    )
    {
        BuildAccelerationStructuresCommand command;
        command.BuildDescriptions = description;

        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<BuildAccelerationStructuresCommandImpl>(description));

        m_CommandListStorage.BuildAccelerationStructures(description);
    }

    void ICommandList::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<CopyAccelerationStructuresCommandImpl>(description));

        m_CommandListStorage.CopyAccelerationStructure(description);
    }

    void ICommandList::CopyAccelerationStructureToDeviceBuffer(
        const AccelerationStructureDeviceBufferCopyDescription &description
    )
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<CopyAccelerationStructureToDeviceBufferCommandImpl>(description));

        m_CommandListStorage.CopyAccelerationStructureToDeviceBuffer(description);
    }

    void ICommandList::CopyDeviceBufferToAccelerationStructure(
        const DeviceBufferAccelerationStructureCopyDescription &description
    )
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<CopyDeviceBufferToAccelerationStructureCommandImpl>(description));

        m_CommandListStorage.CopyDeviceBufferToAccelerationStructure(description);
    }

    void ICommandList::WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        PushConstantsDesc pushConstantDesc = {};
        pushConstantDesc.Name = name;
        pushConstantDesc.Offset = offset;
        pushConstantDesc.Data.resize(size);
        memcpy(pushConstantDesc.Data.data(), data, size);
        m_CommandImpls.emplace_back(std::make_unique<PushConstantsCommandImpl>(pushConstantDesc));

        m_CommandListStorage.WritePushConstants(name, data, size, offset);
    }

    void ICommandList::SubmitMemoryBarrier(const MemoryBarrierDesc &desc)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Barriers.MemoryBarriers.emplace_back(desc);
    }

    void ICommandList::SubmitTextureBarrier(const TextureBarrierDesc &desc)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Barriers.TextureBarriers.emplace_back(desc);
    }

    void ICommandList::SubmitBufferBarrier(const BufferBarrierDesc &desc)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Barriers.BufferBarriers.emplace_back(desc);
    }

    void ICommandList::FlushBarriers()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CommandImpls.emplace_back(std::make_unique<SubmitBarriersCommandImpl>(m_Barriers));
        m_CommandListStorage.SubmitBarrierGroup(m_Barriers);
        m_Barriers.Clear();
    }

    const CommandListDescription &ICommandList::GetDescription()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Description;
    }

    const std::vector<std::unique_ptr<IGraphicsCommand>> &ICommandList::GetCommands()
    {
        return m_CommandImpls;
    }

    bool ICommandList::IsRecording() const
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        return m_Started;
    }

    void ICommandList::Print() const
    {
        m_CommandListStorage.Print();
    }

    void ICommandList::EndRendering()
    {
        if (!m_CurrentFramebuffer.IsValid())
        {
            return;
        }

        EndRenderingCommand command = {};
        command.TargetFramebuffer = m_CurrentFramebuffer;
        m_CommandImpls.emplace_back(std::make_unique<EndRenderingCommandImpl>(command));

        if (m_CurrentFramebuffer->IsOwnedBySwapchain())
        {
            TransitionFramebufferLayouts(
                this, m_CurrentFramebuffer, TextureLayout::PresentSrc, TextureLayout::DepthStencilAttachmentOptimal
            );
        }

        m_CurrentFramebuffer = {};
    }

    void ICommandList::PushError(const std::string &message)
    {
        if (m_CallbackFunction)
        {
            m_CallbackFunction(message);
        }
    }
} // namespace Nexus::Graphics
