
#if defined(NX_PLATFORM_D3D12)

#include "CommandExecutorD3D12.hpp"

#include "DeviceBufferD3D12.hpp"
#include "FramebufferD3D12.hpp"
#include "PipelineD3D12.hpp"
#include "ResourceSetD3D12.hpp"
#include "SwapchainD3D12.hpp"
#include "TextureD3D12.hpp"
#include "TimingQueryD3D12.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

#include <WinPixEventRuntime/pix3.h>

namespace Nexus::Graphics
{
    static void BindFramebuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        auto &cmd = storage.CommandDatas.FramebufferCommands[header->CommandOffset];

        executor->SetFramebuffer(cmd, executor->m_GraphicsDevice);
    }

    static void ClearColourTarget(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        auto &cmd = storage.CommandDatas.ClearColourTargetCommands[header->CommandOffset];

        if (!executor->ValidateForClearColour(executor->m_CurrentFramebuffer, cmd.Index))
        {
            return;
        }

        float clearColor[] = {cmd.Colour.Red, cmd.Colour.Green, cmd.Colour.Blue, cmd.Colour.Alpha};

        if (cmd.Rect.has_value())
        {
            Graphics::ClearRect rect = cmd.Rect.value();

            D3D12_RECT d3d12Rect = {};
            d3d12Rect.left = rect.X;
            d3d12Rect.top = rect.Y;
            d3d12Rect.right = rect.X + rect.Width;
            d3d12Rect.bottom = rect.Y + rect.Height;

            const auto &handle = executor->m_DescriptorHandles[cmd.Index];
            executor->m_CommandList->ClearRenderTargetView(handle, clearColor, 1, &d3d12Rect);
        }
        else
        {
            const auto &handle = executor->m_DescriptorHandles[cmd.Index];
            executor->m_CommandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
        }
    }

    static void ClearDepthStencil(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        auto &cmd = storage.CommandDatas.ClearDepthStencilTargetCommands[header->CommandOffset];

        if (executor->m_DepthHandle.ptr)
        {
            D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;

            if (cmd.Rect.has_value())
            {
                Graphics::ClearRect rect = cmd.Rect.value();

                D3D12_RECT d3d12Rect = {};
                d3d12Rect.left = rect.X;
                d3d12Rect.top = rect.Y;
                d3d12Rect.right = rect.X + rect.Width;
                d3d12Rect.bottom = rect.Y + rect.Height;
                executor->m_CommandList->ClearDepthStencilView(
                    executor->m_DepthHandle, clearFlags, cmd.Value.Depth, cmd.Value.Stencil, 1, &d3d12Rect
                );
            }
            else
            {
                executor->m_CommandList->ClearDepthStencilView(
                    executor->m_DepthHandle, clearFlags, cmd.Value.Depth, cmd.Value.Stencil, 0, nullptr
                );
            }
        }
    }

    static void SubmitBarrierGroup(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        auto &cmd = storage.CommandDatas.BarrierGroupCommands[header->CommandOffset];

        const auto &d3d12Features = executor->m_GraphicsDevice->GetD3D12DeviceFeatures();

        // enhanced barriers
        if (d3d12Features.SupportsEnhancedBarriers)
        {
            std::vector<D3D12_GLOBAL_BARRIER> globalBarriers = {};
            std::vector<D3D12_TEXTURE_BARRIER> textureBarriers = {};
            std::vector<D3D12_BUFFER_BARRIER> bufferBarriers = {};

            // memory barriers
            for (const auto &memoryBarrier : cmd.MemoryBarriers)
            {
                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(memoryBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(memoryBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(memoryBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(memoryBarrier.AfterAccess);

                D3D12_GLOBAL_BARRIER &barrier = globalBarriers.emplace_back();
                barrier.SyncBefore = beforeSync;
                barrier.SyncAfter = afterSync;
                barrier.AccessBefore = beforeAccess;
                barrier.AccessAfter = afterAccess;
            }

            // texture barriers
            for (const auto &textureBarrier : cmd.TextureBarriers)
            {
                const TextureD3D12 *texture = textureBarrier.Texture.AsDerived<const TextureD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = texture->GetHandle();

                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(textureBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(textureBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(textureBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(textureBarrier.AfterAccess);

                D3D12_BARRIER_LAYOUT afterLayout = D3D12::GetBarrierLayout(textureBarrier.Layout);

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
                        D3D12_BARRIER_LAYOUT beforeLayout =
                            D3D12::GetBarrierLayout(texture->GetTextureLayout(arrayLayer, mipLevel));

                        if (beforeLayout == afterLayout)
                        {
                            continue;
                        }

                        D3D12_TEXTURE_BARRIER &barrier = textureBarriers.emplace_back();
                        barrier.SyncBefore = beforeSync;
                        barrier.SyncAfter = afterSync;
                        barrier.AccessBefore = beforeAccess;
                        barrier.AccessAfter = afterAccess;
                        barrier.LayoutBefore = beforeLayout;
                        barrier.LayoutAfter = afterLayout;
                        barrier.pResource = handle.Get();
                        barrier.Subresources.FirstArraySlice = arrayLayer;
                        barrier.Subresources.NumArraySlices = 1;
                        barrier.Subresources.IndexOrFirstMipLevel = mipLevel;
                        barrier.Subresources.NumMipLevels = 1;
                        barrier.Subresources.FirstPlane = 0;
                        barrier.Subresources.NumPlanes = 1;
                    }
                }
            }

            // buffer barriers
            for (const auto &bufferBarrier : cmd.BufferBarriers)
            {
                const DeviceBufferD3D12 *buffer = bufferBarrier.Buffer.AsDerived<DeviceBufferD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(bufferBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(bufferBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(bufferBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(bufferBarrier.AfterAccess);

                D3D12_BUFFER_BARRIER barrier = bufferBarriers.emplace_back();
                barrier.SyncBefore = beforeSync;
                barrier.SyncAfter = afterSync;
                barrier.AccessBefore = beforeAccess;
                barrier.AccessAfter = afterAccess;
                barrier.pResource = handle.Get();

                // these have to be hardcoded to these values as transitioning part
                // of a buffer is not currently supported
                barrier.Offset = 0;
                barrier.Size = UINT64_MAX;
            }

            // create the barriers to submit
            std::vector<D3D12_BARRIER_GROUP> barrierGroups = {};
            if (globalBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &globalBarrierGroup = barrierGroups.emplace_back();
                globalBarrierGroup.Type = D3D12_BARRIER_TYPE_GLOBAL;
                globalBarrierGroup.NumBarriers = static_cast<UINT32>(globalBarriers.size());
                globalBarrierGroup.pGlobalBarriers = globalBarriers.data();
            }

            if (textureBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &textureBarrierGroup = barrierGroups.emplace_back();
                textureBarrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
                textureBarrierGroup.NumBarriers = static_cast<UINT32>(textureBarriers.size());
                textureBarrierGroup.pTextureBarriers = textureBarriers.data();
            }

            if (bufferBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &bufferBarrierGroup = barrierGroups.emplace_back();
                bufferBarrierGroup.Type = D3D12_BARRIER_TYPE_BUFFER;
                bufferBarrierGroup.NumBarriers = static_cast<UINT32>(bufferBarriers.size());
                bufferBarrierGroup.pBufferBarriers = bufferBarriers.data();
            }

            if (!barrierGroups.empty())
            {
                executor->m_CommandList->Barrier(static_cast<UINT32>(barrierGroups.size()), barrierGroups.data());
            }
        }
        // resource barriers
        else
        {
            std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers = {};

            for (const auto &textureBarrier : cmd.TextureBarriers)
            {
                const TextureD3D12 *texture = textureBarrier.Texture.AsDerived<const TextureD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = texture->GetHandle();

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
                        D3D12_RESOURCE_STATES beforeState =
                            D3D12::GetTextureResourceState(texture->GetTextureLayout(arrayLayer, mipLevel));
                        D3D12_RESOURCE_STATES afterState = D3D12::GetTextureResourceState(textureBarrier.Layout);

                        if (beforeState == afterState)
                        {
                            continue;
                        }

                        uint32_t subresourceIndex =
                            Utils::CalculateSubresource(mipLevel, arrayLayer, texture->GetDescription().MipLevels);

                        D3D12_RESOURCE_BARRIER &barrier = resourceBarriers.emplace_back();
                        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                        barrier.Transition.pResource = handle.Get();
                        barrier.Transition.Subresource = subresourceIndex;
                        barrier.Transition.StateBefore = beforeState;
                        barrier.Transition.StateAfter = afterState;
                    }
                }
            }

            for (const auto &bufferBarrier : cmd.BufferBarriers)
            {
                if (bufferBarrier.BeforeAccess == BarrierAccess::ShaderWrite)
                {
                    const DeviceBufferD3D12 *buffer = bufferBarrier.Buffer.AsDerived<DeviceBufferD3D12>();
                    Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

                    D3D12_RESOURCE_BARRIER barrier = resourceBarriers.emplace_back();
                    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
                    barrier.UAV.pResource = handle.Get();
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                }
            }

            if (!resourceBarriers.empty())
            {
                executor->m_CommandList->ResourceBarrier(
                    static_cast<UINT>(resourceBarriers.size()), resourceBarriers.data()
                );
            }
        }

        // update texture layouts
        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (const TextureBarrierDesc &textureBarrier : cmd.TextureBarriers)
        {
            TextureHandle textureHandle = textureBarrier.Texture;
            TextureD3D12 *textureD3D12 = textureHandle.AsDerived<TextureD3D12>();

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
                    textureD3D12->SetTextureLayout(arrayLayer, mipLevel, textureBarrier.Layout);
                }
            }
        }
    }

    static void SetVertexBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.SetVertexBufferCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentFramebuffer))
        {
            return;
        }

        if (executor->m_CurrentlyBoundPipeline.IsValid() &&
            executor->m_CurrentlyBoundPipeline->GetType() == PipelineType::Graphics)
        {
            const GraphicsPipelineD3D12 *pipeline =
                executor->m_CurrentlyBoundPipeline.AsDerived<const GraphicsPipelineD3D12>();
            const DeviceBufferD3D12 *d3d12VertexBuffer = cmd.View.BufferHandle.AsDerived<const DeviceBufferD3D12>();
            const auto &bufferLayout = pipeline->GetPipelineDescription().Layouts.at(cmd.Slot);

            D3D12_VERTEX_BUFFER_VIEW bufferView = {};
            bufferView.BufferLocation = d3d12VertexBuffer->GetHandle()->GetGPUVirtualAddress() + cmd.View.Offset;
            bufferView.SizeInBytes = cmd.View.Size;
            bufferView.StrideInBytes = pipeline->GetPipelineDescription().Layouts.at(cmd.Slot).GetStride();

            executor->m_CommandList->IASetVertexBuffers(cmd.Slot, 1, &bufferView);
        }
    }

    static void SetIndexBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.SetIndexBufferCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentFramebuffer))
        {
            return;
        }

        const DeviceBufferD3D12 *d3d12IndexBuffer = cmd.View.BufferHandle.AsDerived<const DeviceBufferD3D12>();

        D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
        indexBufferView.BufferLocation = d3d12IndexBuffer->GetHandle()->GetGPUVirtualAddress() + cmd.View.Offset;
        indexBufferView.SizeInBytes = cmd.View.Size;
        indexBufferView.Format = D3D12::GetD3D12IndexBufferFormat(cmd.View.BufferFormat);

        executor->m_CommandList->IASetIndexBuffer(&indexBufferView);
    }

    static void SetPipeline(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        auto &cmd = storage.CommandDatas.SetPipelineCommands.at(header->CommandOffset);

        PipelineD3D12 *pipeline = cmd.AsDerived<PipelineD3D12>();
        pipeline->Bind(executor->m_CommandList);
        executor->m_CurrentlyBoundPipeline = cmd;
    }

    static void SetResourceSet(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.ResourceSetBindingCommands.at(header->CommandOffset);

        Nexus::Graphics::PipelineType pipelineType = executor->m_CurrentlyBoundPipeline->GetType();

        ResourceSetHandle handle = cmd.TargetResourceSet;
        ResourceSetD3D12 *d3d12ResourceSet = handle.AsDerived<ResourceSetD3D12>();

        if (d3d12ResourceSet)
        {
            executor->m_CurrentlyBoundResourceSet = d3d12ResourceSet;

            bool isGraphics = pipelineType != PipelineType::Compute;
            d3d12ResourceSet->Bind(isGraphics, executor->m_CommandList, cmd.DynamicOffsets);
        }
    }

    static void Draw(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.DrawCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentFramebuffer))
        {
            return;
        }

        executor->m_CommandList->DrawInstanced(cmd.VertexCount, cmd.InstanceCount, cmd.VertexStart, cmd.InstanceStart);
    }

    static void DrawIndexed(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.DrawIndexedCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentFramebuffer))
        {
            return;
        }

        executor->m_CommandList->DrawIndexedInstanced(
            cmd.IndexCount, cmd.InstanceCount, cmd.IndexStart, cmd.VertexStart, cmd.InstanceStart
        );
    }

    static void DrawIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.DrawIndirectCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentFramebuffer))
        {
            return;
        }

        NX_VALIDATE(
            cmd.IndirectBuffer->CheckUsage(Graphics::BufferUsage_Indirect),
            "Buffer passed to DrawIndirect is not an indirect buffer"
        );

        if (executor->m_CurrentlyBoundPipeline.IsValid() &&
            executor->m_CurrentlyBoundPipeline->GetType() == PipelineType::Graphics)
        {
            if (const DeviceBufferD3D12 *indirectBuffer = cmd.IndirectBuffer.AsDerived<const DeviceBufferD3D12>())
            {
                Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

                Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
                    executor->GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DRAW, cmd.Stride);

                executor->m_CommandList->ExecuteIndirect(
                    signature.Get(), cmd.DrawCount, indirectBufferHandle.Get(), cmd.Offset, nullptr, 0
                );
            }
        }
    }

    static void DrawIndexedIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.DrawIndirectIndexedCommands.at(header->CommandOffset);

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentFramebuffer))
        {
            return;
        }

        NX_VALIDATE(
            cmd.IndirectBuffer->CheckUsage(Graphics::BufferUsage_Indirect),
            "Buffer passed to DrawIndirect is not an indirect buffer"
        );

        if (executor->m_CurrentlyBoundPipeline.IsValid() &&
            executor->m_CurrentlyBoundPipeline->GetType() == PipelineType::Graphics)
        {
            if (const DeviceBufferD3D12 *indirectBuffer = cmd.IndirectBuffer.AsDerived<const DeviceBufferD3D12>())
            {
                Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

                Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
                    executor->GetOrCreateIndirectCommandSignature(
                        D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED, cmd.Stride
                    );

                executor->m_CommandList->ExecuteIndirect(
                    signature.Get(), cmd.DrawCount, indirectBufferHandle.Get(), cmd.Offset, nullptr, 0
                );
            }
        }
    }

    static void Dispatch(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.DispatchCommands.at(header->CommandOffset);

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        executor->m_CommandList->Dispatch(cmd.WorkGroupCountX, cmd.WorkGroupCountY, cmd.WorkGroupCountZ);
    }

    static void DispatchIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.DispatchIndirectCommands.at(header->CommandOffset);

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        if (const DeviceBufferD3D12 *indirectBuffer = cmd.IndirectBuffer.AsDerived<const DeviceBufferD3D12>())
        {
            Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

            Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
                executor->GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH, cmd.Stride);

            executor->m_CommandList->ExecuteIndirect(
                signature.Get(), 1, indirectBufferHandle.Get(), cmd.Offset, nullptr, 0
            );
        }
    }

    static void DrawMesh(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.DrawMeshCommands.at(header->CommandOffset);

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        executor->m_CommandList->DispatchMesh(cmd.WorkGroupCountX, cmd.WorkGroupCountY, cmd.WorkGroupCountZ);
    }

    static void DrawMeshIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.DrawMeshIndirectCommands.at(header->CommandOffset);

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        {
            if (const DeviceBufferD3D12 *indirectBuffer = cmd.IndirectBuffer.AsDerived<const DeviceBufferD3D12>())
            {
                Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

                Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
                    executor->GetOrCreateIndirectCommandSignature(
                        D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH, cmd.Stride
                    );

                executor->m_CommandList->ExecuteIndirect(
                    signature.Get(), cmd.DrawCount, indirectBufferHandle.Get(), cmd.Offset, nullptr, 0
                );
            }
        }
    }

    static void TraceRays(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
    }

    static void SetViewport(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        auto &cmd = storage.CommandDatas.ViewportCommands[header->CommandOffset];

        D3D12_VIEWPORT vp = {};
        vp.TopLeftX = cmd.X;
        vp.TopLeftY = cmd.Y;
        vp.Width = cmd.Width;
        vp.Height = cmd.Height;
        vp.MinDepth = cmd.MinDepth;
        vp.MaxDepth = cmd.MaxDepth;
        executor->m_CommandList->RSSetViewports(1, &vp);
    }

    static void SetScissor(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        auto &cmd = storage.CommandDatas.ScissorCommands[header->CommandOffset];

        RECT rect = {};
        rect.left = cmd.X;
        rect.top = cmd.Y;
        rect.right = cmd.Width + cmd.X;
        rect.bottom = cmd.Height + cmd.Y;
        executor->m_CommandList->RSSetScissorRects(1, &rect);
    }

    static void PushConstants(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.PushConstantsCommands.at(header->CommandOffset);

        if (!executor->m_CurrentlyBoundResourceSet && !executor->m_CurrentlyBoundPipeline.IsValid())
            return;

        bool isGraphics = executor->m_CurrentlyBoundPipeline->GetType() != PipelineType::Compute;

        executor->m_CurrentlyBoundResourceSet->SetPushConstants(
            cmd.Name, cmd.Data.data(), cmd.Offset, cmd.Data.size(), isGraphics, executor->m_CommandList
        );
    }

    static void CopyBufferToBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.CopyBufferToBufferCommands.at(header->CommandOffset);

        const DeviceBufferD3D12 *source = cmd.BufferCopy.Source.AsDerived<const DeviceBufferD3D12>();
        const DeviceBufferD3D12 *dest = cmd.BufferCopy.Destination.AsDerived<const DeviceBufferD3D12>();

        if (source && dest)
        {
            for (const auto &copy : cmd.BufferCopy.Copies)
            {
                executor->m_CommandList->CopyBufferRegion(
                    dest->GetHandle().Get(), copy.WriteOffset, source->GetHandle().Get(), copy.ReadOffset, copy.Size
                );
            }
        }
    }

    static void CopyBufferToTexture(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.CopyBufferToTextureCommands.at(header->CommandOffset);

        GraphicsDeviceD3D12 *deviceD3D12 = executor->m_GraphicsDevice;
        Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

        const TextureD3D12 *texture = cmd.BufferTextureCopy.Texture.AsDerived<const TextureD3D12>();

        if (!texture)
        {
            return;
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> textureHandle = texture->GetHandle();

        const bool layeredTexture = texture->IsLayeredTexture();
        uint32_t subresourceIndex = Utils::CalculateSubresource(
            cmd.BufferTextureCopy.MipLevel, layeredTexture ? cmd.BufferTextureCopy.TextureOffset.Z : 0,
            cmd.BufferTextureCopy.Texture->GetMipLevels()
        );

        D3D12_BOX textureBounds = {};
        textureBounds.left = cmd.BufferTextureCopy.TextureOffset.X;
        textureBounds.right = cmd.BufferTextureCopy.TextureOffset.X + cmd.BufferTextureCopy.TextureExtent.Width;
        textureBounds.top = cmd.BufferTextureCopy.TextureOffset.Y;
        textureBounds.bottom = cmd.BufferTextureCopy.TextureOffset.Y + cmd.BufferTextureCopy.TextureExtent.Height;

        if (texture->GetType() == TextureType::Texture3D)
        {
            textureBounds.front = cmd.BufferTextureCopy.TextureOffset.Z;
            textureBounds.back = cmd.BufferTextureCopy.TextureOffset.Z + 1;
        }
        else
        {
            textureBounds.front = 0;
            textureBounds.back = 1;
        }

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
        UINT numRows = {};
        UINT64 rowSizeInBytes = {};
        UINT64 totalBytes = {};

        D3D12_RESOURCE_DESC resourceDesc = textureHandle->GetDesc();
        nativeDevice->GetCopyableFootprints(
            &resourceDesc, subresourceIndex, 1, cmd.BufferTextureCopy.BufferOffset, &footprint, &numRows,
            &rowSizeInBytes, &totalBytes
        );

        DeviceBufferHandle stagingBuffer = executor->CreateStagingBuffer(totalBytes, true, deviceD3D12);
        DeviceBufferHandle destHandle = cmd.BufferTextureCopy.BufferHandle;
        DeviceBufferD3D12 *sourceBufferD3D12 = destHandle.AsDerived<DeviceBufferD3D12>();
        const DeviceBufferD3D12 *stagingBufferD3D12 = stagingBuffer.AsDerived<const DeviceBufferD3D12>();
        Microsoft::WRL::ComPtr<ID3D12Resource> bufferHandle = stagingBufferD3D12->GetHandle();

        uint8_t *sourceData = sourceBufferD3D12->Map();
        uint8_t *stagingData = stagingBuffer->Map();

        if (!texture || !sourceBufferD3D12 || !stagingBufferD3D12)
        {
            throw std::runtime_error("Invalid cast in ExecuteCommand");
        }

        uint32_t rowSize =
            cmd.BufferTextureCopy.TextureExtent.Width * GetPixelFormatSizeInBytes(texture->GetPixelFormat());

        for (uint32_t y = 0; y < cmd.BufferTextureCopy.TextureExtent.Height; y++)
        {
            uint8_t *dstRow = stagingData + y * footprint.Footprint.RowPitch;
            uint8_t *srcRow = sourceData + y * rowSize;

            memcpy(dstRow, srcRow, rowSize);

            // Optional: clear padding to avoid garbage
            if (rowSize < footprint.Footprint.RowPitch)
            {
                memset(dstRow + rowSize, 0, footprint.Footprint.RowPitch - rowSize);
            }
        }

        sourceBufferD3D12->Unmap();
        stagingBuffer->Unmap();

        D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
        srcLocation.pResource = bufferHandle.Get();
        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        srcLocation.PlacedFootprint = footprint;

        D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
        dstLocation.pResource = textureHandle.Get();
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstLocation.SubresourceIndex = subresourceIndex;

        const uint32_t zOffset =
            texture->GetType() == TextureType::Texture3D ? cmd.BufferTextureCopy.TextureOffset.Z : 0;

        executor->m_CommandList->CopyTextureRegion(
            &dstLocation, cmd.BufferTextureCopy.TextureOffset.X, cmd.BufferTextureCopy.TextureOffset.Y, zOffset,
            &srcLocation, &textureBounds
        );
    }

    static void CopyTextureToBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.CopyTextureToBufferCommands.at(header->CommandOffset);

        GraphicsDeviceD3D12 *deviceD3D12 = executor->m_GraphicsDevice;
        Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

        DeviceBufferHandle handle = cmd.TextureBufferCopy.BufferHandle;
        DeviceBufferD3D12 *buffer = handle.AsDerived<DeviceBufferD3D12>();
        const TextureD3D12 *texture = cmd.TextureBufferCopy.Texture.AsDerived<const TextureD3D12>();

        if (!texture)
        {
            return;
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> textureHandle = texture->GetHandle();

        bool arrayedTexture = cmd.TextureBufferCopy.Texture->GetType() == TextureType::Texture3D ||
                              cmd.TextureBufferCopy.Texture->GetType() == TextureType::TextureCube;
        uint32_t subresourceIndex = Utils::CalculateSubresource(
            cmd.TextureBufferCopy.MipLevel, arrayedTexture ? cmd.TextureBufferCopy.TextureOffset.Z : 0,
            cmd.TextureBufferCopy.Texture->GetMipLevels()
        );

        D3D12_BOX textureBounds = {};
        textureBounds.left = cmd.TextureBufferCopy.TextureOffset.X;
        textureBounds.right = cmd.TextureBufferCopy.TextureOffset.X + cmd.TextureBufferCopy.TextureExtent.Width;
        textureBounds.top = cmd.TextureBufferCopy.TextureOffset.Y;
        textureBounds.bottom = cmd.TextureBufferCopy.TextureOffset.Y + cmd.TextureBufferCopy.TextureExtent.Height;
        textureBounds.front = cmd.TextureBufferCopy.TextureOffset.Z;
        textureBounds.back = cmd.TextureBufferCopy.TextureOffset.Z + 1;

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
        UINT numRows = {};
        UINT64 rowSizeInBytes = {};
        UINT64 totalBytes = {};

        D3D12_RESOURCE_DESC resourceDesc = textureHandle->GetDesc();
        nativeDevice->GetCopyableFootprints(
            &resourceDesc, subresourceIndex, 1, cmd.TextureBufferCopy.BufferOffset, &footprint, &numRows,
            &rowSizeInBytes, &totalBytes
        );

        Microsoft::WRL::ComPtr<ID3D12Resource> bufferHandle = buffer->GetHandle();

        D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
        srcLocation.pResource = textureHandle.Get();
        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        srcLocation.SubresourceIndex = subresourceIndex;

        D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
        dstLocation.pResource = bufferHandle.Get();
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        dstLocation.PlacedFootprint = footprint;

        // copy texture data into the buffer (the 0's are for the offset into the
        // destination texture, which we do not need here)
        executor->m_CommandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, &textureBounds);

        size_t tightPitch =
            cmd.TextureBufferCopy.TextureExtent.Width * GetPixelFormatSizeInBytes(texture->GetDescription().Format);

        D3D12ReadbackBufferCopyOperation &readbackOperation = executor->m_ReadbackCopies.emplace_back();
        readbackOperation.ReadbackBuffer = buffer;
        readbackOperation.SourceRowPitch = footprint.Footprint.RowPitch;
        readbackOperation.DestinationRowPitch = tightPitch;
        readbackOperation.Height = cmd.TextureBufferCopy.TextureExtent.Height;
        readbackOperation.NumRows = numRows;
    }

    static void CopyTextureToTexture(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.CopyTextureToTextureCommands.at(header->CommandOffset);

        GraphicsDeviceD3D12 *deviceD3D12 = executor->m_GraphicsDevice;
        Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

        const TextureD3D12 *srcTexture = cmd.TextureCopy.Source.AsDerived<const TextureD3D12>();
        const TextureD3D12 *dstTexture = cmd.TextureCopy.Destination.AsDerived<const TextureD3D12>();

        Microsoft::WRL::ComPtr<ID3D12Resource2> srcHandle = srcTexture->GetHandle();
        Microsoft::WRL::ComPtr<ID3D12Resource2> dstHandle = dstTexture->GetHandle();

        // retrieve source index
        bool srcArrayedTexture =
            srcTexture->GetType() == TextureType::Texture3D || srcTexture->GetType() == TextureType::TextureCube;
        uint32_t srcSubresourceIndex = Utils::CalculateSubresource(
            cmd.TextureCopy.SourceMipLevel, srcArrayedTexture ? cmd.TextureCopy.SourceOffset.Z : 0,
            cmd.TextureCopy.Source->GetMipLevels()
        );

        // retrieve destination footprint
        bool dstArrayedTexture =
            dstTexture->GetType() == TextureType::Texture3D || dstTexture->GetType() == TextureType::TextureCube;
        uint32_t dstSubresourceIndex = Utils::CalculateSubresource(
            cmd.TextureCopy.DestinationMipLevel, dstArrayedTexture ? cmd.TextureCopy.DestinationOffset.Z : 0,
            cmd.TextureCopy.Destination->GetMipLevels()
        );

        D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
        srcLocation.pResource = srcHandle.Get();
        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        srcLocation.SubresourceIndex = srcSubresourceIndex;

        D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
        dstLocation.pResource = dstHandle.Get();
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstLocation.SubresourceIndex = dstSubresourceIndex;

        TextureLayout srcLayout =
            srcTexture->GetTextureLayout(cmd.TextureCopy.SourceOffset.Z, cmd.TextureCopy.SourceMipLevel);
        D3D12_RESOURCE_STATES srcResourceState = D3D12::GetTextureResourceState(srcLayout);

        TextureLayout dstLayout =
            dstTexture->GetTextureLayout(cmd.TextureCopy.DestinationOffset.Z, cmd.TextureCopy.DestinationMipLevel);
        D3D12_RESOURCE_STATES dstResourceState = D3D12::GetTextureResourceState(dstLayout);

        D3D12_BOX textureBounds = {};
        textureBounds.left = cmd.TextureCopy.SourceOffset.X;
        textureBounds.right = cmd.TextureCopy.SourceOffset.X + cmd.TextureCopy.Extent.Width;
        textureBounds.top = cmd.TextureCopy.SourceOffset.Y;
        textureBounds.bottom = cmd.TextureCopy.SourceOffset.Y + cmd.TextureCopy.Extent.Height;
        textureBounds.front = cmd.TextureCopy.SourceOffset.Z;
        textureBounds.back = cmd.TextureCopy.SourceOffset.Z + 1;

        executor->m_CommandList->CopyTextureRegion(
            &dstLocation, cmd.TextureCopy.DestinationOffset.X, cmd.TextureCopy.DestinationOffset.Y,
            cmd.TextureCopy.DestinationOffset.Z, &srcLocation, &textureBounds
        );
    }

    static void Resolve(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);
        const auto &cmd = storage.CommandDatas.ResolveCommands.at(header->CommandOffset);
        executor->ResolveTextureImpl(cmd);
    }

    static void StartTimingQuery(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.StartTimingQueryCommands.at(header->CommandOffset);

        if (const TimingQueryD3D12 *queryD3D12 = cmd.Query.AsDerived<const TimingQueryD3D12>())
        {
            Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap = queryD3D12->GetQueryHeap();
            executor->m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
        }
    }

    static void StopTimingQuery(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.StopTimingQueryCommands.at(header->CommandOffset);

        if (const TimingQueryD3D12 *queryD3D12 = cmd.Query.AsDerived<const TimingQueryD3D12>())
        {
            Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap = queryD3D12->GetQueryHeap();
            executor->m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
        }
    }

    static void BeginDebugGroup(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.BeginDebugGroupCommands.at(header->CommandOffset);

        if (executor->m_PIXBeginEvent && executor->m_PIXEndEvent)
        {
            uint32_t colour = Utils::PackColour(cmd.Colour);
            executor->m_PIXBeginEvent(executor->m_CommandList.Get(), colour, cmd.GroupName.c_str());
        }
    }

    static void EndDebugGroup(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.EndDebugGroupCommands.at(header->CommandOffset);

        if (executor->m_PIXBeginEvent && executor->m_PIXEndEvent)
        {
            executor->m_PIXEndEvent(executor->m_CommandList.Get());
        }
    }

    static void DebugLabel(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.InsertDebugMarkerCommands.at(header->CommandOffset);

        if (executor->m_PIXSetMarker)
        {
            uint32_t colour = Utils::PackColour(cmd.Colour);
            executor->m_PIXSetMarker(executor->m_CommandList.Get(), colour, cmd.MarkerName.c_str());
        }
    }

    static void SetBlendFactor(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.SetBlendFactorCommands.at(header->CommandOffset);

        float blendFactor[4] = {
            cmd.BlendFactor.Red, cmd.BlendFactor.Green, cmd.BlendFactor.Blue, cmd.BlendFactor.Alpha
        };

        executor->m_CommandList->OMSetBlendFactor(blendFactor);
    }

    static void SetStencilReference(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.SetStencilReferenceCommands.at(header->CommandOffset);

        executor->m_CommandList->OMSetStencilRef(cmd.StencilReference);
    }

    static void BuildAccelerationStructures(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.BuildAccelerationStructuresCommands.at(header->CommandOffset);

        for (const auto &accelerationStructureBuildDesc : cmd.BuildDescriptions)
        {
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
            std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometry = {};

            D3D12::GetD3D12AccelerationStructureInputs(accelerationStructureBuildDesc, inputs, geometry);

            D3D12_GPU_VIRTUAL_ADDRESS srcAddress = 0;
            D3D12_GPU_VIRTUAL_ADDRESS destAddress = 0;
            D3D12_GPU_VIRTUAL_ADDRESS scratchAddress = accelerationStructureBuildDesc.ScratchBuffer;

            if (accelerationStructureBuildDesc.Source.IsValid())
            {
                srcAddress = accelerationStructureBuildDesc.Source->GetDeviceAddress(0);
            }

            if (accelerationStructureBuildDesc.Destination.IsValid())
            {
                destAddress = accelerationStructureBuildDesc.Destination->GetDeviceAddress(0);
            }

            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
            buildDesc.SourceAccelerationStructureData = srcAddress;
            buildDesc.DestAccelerationStructureData = destAddress;
            buildDesc.ScratchAccelerationStructureData = scratchAddress;
            buildDesc.Inputs = inputs;
            executor->m_CommandList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
        }
    }

    static void CopyAccelerationStructures(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.CopyAccelerationStructuresCommands.at(header->CommandOffset);
    }

    static void CopyAccelerationStructureToDeviceBuffer(
        const CommandHeader *header, CommandListStorage &storage, void *data
    )
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.CopyAccelerationStructureDeviceBufferCommands.at(header->CommandOffset);
    }

    static void CopyDeviceBufferToAccelerationStructure(
        const CommandHeader *header, CommandListStorage &storage, void *data
    )
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.CopyDeviceBufferAccelerationStructureCommands.at(header->CommandOffset);
    }

    static void EndRendering(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorD3D12 *executor = reinterpret_cast<CommandExecutorD3D12 *>(data);

        const auto &cmd = storage.CommandDatas.EndRenderingCommands.at(header->CommandOffset);

        FramebufferHandle framebuffer = cmd.TargetFramebuffer;

        if (framebuffer->GetSampleCount() > 1)
        {
            for (size_t textureIndex = 0; textureIndex < framebuffer->GetColorTextureCount(); textureIndex++)
            {
                std::optional<FramebufferColourAttachmentDescription> colourAttachmentDescOpt =
                    framebuffer->GetColorTextureBinding(textureIndex);
                if (colourAttachmentDescOpt.has_value())
                {
                    FramebufferColourAttachmentDescription colourAttachmentDesc = colourAttachmentDescOpt.value();
                    if (colourAttachmentDesc.ResolveAttachment.has_value())
                    {
                        for (uint32_t layerIndex = 0; layerIndex < colourAttachmentDesc.ColourAttachment.LayerCount;
                             layerIndex++)
                        {
                            FramebufferTextureDescription resolveAttachmentDesc =
                                colourAttachmentDesc.ResolveAttachment.value();

                            uint32_t sourceArrayIndex =
                                colourAttachmentDesc.ColourAttachment.BaseArrayLayer + layerIndex;
                            uint32_t destArrayIndex = resolveAttachmentDesc.BaseArrayLayer + layerIndex;

                            TextureLayout sourceLayout =
                                colourAttachmentDesc.ColourAttachment.TargetTexture->GetTextureLayout(
                                    sourceArrayIndex, colourAttachmentDesc.ColourAttachment.MipLevel
                                );
                            TextureLayout destLayout = resolveAttachmentDesc.TargetTexture->GetTextureLayout(
                                destArrayIndex, resolveAttachmentDesc.MipLevel
                            );

                            BarrierGroupDescription barrierGroup = {};

                            // to resolve compatible layouts
                            {
                                TextureBarrierDesc sourceBarrier = {};
                                sourceBarrier.Texture = colourAttachmentDesc.ColourAttachment.TargetTexture;
                                sourceBarrier.BeforeAccess = BarrierAccess::ColourAttachmentWrite;
                                sourceBarrier.AfterAccess = BarrierAccess::ColourAttachmentRead;
                                sourceBarrier.BeforeStage = BarrierPipelineStage::ColourAttachmentOutput;
                                sourceBarrier.AfterStage = BarrierPipelineStage::Resolve;
                                sourceBarrier.Layout = TextureLayout::ResolveSrc;
                                sourceBarrier.TextureSubresourceRange = {
                                    .BaseMipLevel = colourAttachmentDesc.ColourAttachment.MipLevel,
                                    .LevelCount = 1,
                                    .BaseArrayLayer = sourceArrayIndex,
                                    .LayerCount = 1
                                };
                                barrierGroup.TextureBarriers.emplace_back(sourceBarrier);

                                TextureBarrierDesc destBarrier = {};
                                destBarrier.Texture = resolveAttachmentDesc.TargetTexture;
                                destBarrier.BeforeAccess = BarrierAccess::ColourAttachmentWrite;
                                destBarrier.AfterAccess = BarrierAccess::ColourAttachmentRead;
                                destBarrier.BeforeStage = BarrierPipelineStage::ColourAttachmentOutput;
                                destBarrier.AfterStage = BarrierPipelineStage::Resolve;
                                destBarrier.Layout = TextureLayout::ResolveDest;
                                destBarrier.TextureSubresourceRange = {
                                    .BaseMipLevel = resolveAttachmentDesc.MipLevel,
                                    .LevelCount = 1,
                                    .BaseArrayLayer = destArrayIndex,
                                    .LayerCount = 1
                                };
                                barrierGroup.TextureBarriers.emplace_back(destBarrier);

                                executor->SubmitBarrierGroupImpl(barrierGroup);
                            }

                            // execute resolve
                            {
                                ResolveTextureDescription resolveDesc = {};
                                resolveDesc.Source = colourAttachmentDesc.ColourAttachment.TargetTexture;
                                resolveDesc.SourceMipLevel = colourAttachmentDesc.ColourAttachment.MipLevel;
                                resolveDesc.SourceArrayLayer = sourceArrayIndex;
                                resolveDesc.Destination = resolveAttachmentDesc.TargetTexture;
                                resolveDesc.DestinationMipLevel = resolveAttachmentDesc.MipLevel;
                                resolveDesc.DestinationArrayLayer = destArrayIndex;

                                executor->ResolveTextureImpl(resolveDesc);
                            }

                            // to resolve compatible layouts
                            {
                                BarrierGroupDescription barrierGroup = {};

                                TextureBarrierDesc sourceBarrier = {};
                                sourceBarrier.Texture = colourAttachmentDesc.ColourAttachment.TargetTexture;
                                sourceBarrier.BeforeAccess = BarrierAccess::TransferRead;
                                sourceBarrier.AfterAccess = BarrierAccess::NoAccess;
                                sourceBarrier.BeforeStage = BarrierPipelineStage::Resolve;
                                sourceBarrier.AfterStage = BarrierPipelineStage::NoStage;
                                sourceBarrier.Layout = sourceLayout;
                                sourceBarrier.TextureSubresourceRange = {
                                    .BaseMipLevel = colourAttachmentDesc.ColourAttachment.MipLevel,
                                    .LevelCount = 1,
                                    .BaseArrayLayer = sourceArrayIndex,
                                    .LayerCount = 1
                                };
                                barrierGroup.TextureBarriers.emplace_back(sourceBarrier);

                                TextureBarrierDesc destBarrier = {};
                                destBarrier.Texture = resolveAttachmentDesc.TargetTexture;
                                destBarrier.BeforeAccess = BarrierAccess::TransferRead;
                                destBarrier.AfterAccess = BarrierAccess::NoAccess;
                                destBarrier.BeforeStage = BarrierPipelineStage::Resolve;
                                destBarrier.AfterStage = BarrierPipelineStage::NoStage;
                                destBarrier.Layout = destLayout;
                                destBarrier.TextureSubresourceRange = {
                                    .BaseMipLevel = resolveAttachmentDesc.MipLevel,
                                    .LevelCount = 1,
                                    .BaseArrayLayer = destArrayIndex,
                                    .LayerCount = 1
                                };
                                barrierGroup.TextureBarriers.emplace_back(destBarrier);
                            }
                        }
                    }
                }
            }
        }
    }

    CommandExecutorD3D12::CommandExecutorD3D12(Microsoft::WRL::ComPtr<ID3D12Device9> device) : m_Device(device)
    {
        m_PixModule = LoadLibrary("WinPixEventRuntime.dll");
        if (m_PixModule)
        {
            m_PIXBeginEvent = (PIXBeginEventFn)GetProcAddress(m_PixModule, "PIXBeginEventOnCommandList");
            m_PIXEndEvent = (PIXEndEventFn)GetProcAddress(m_PixModule, "PIXEndEventOnCommandList");
            m_PIXSetMarker = (PIXSetMarkerFn)GetProcAddress(m_PixModule, "PIXSetMarkerOnCommandList");
        }
        else
        {
            NX_WARNING(
                "Failed to load PIX, some debugging functionality may not work "
                "correctly"
            );
        }

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
        m_DispatchTable[ToIndex(CommandType::ResolveFramebuffer)] = Resolve;
        m_DispatchTable[ToIndex(CommandType::StartTimingQuery)] = StartTimingQuery;
        m_DispatchTable[ToIndex(CommandType::StopTimingQuery)] = StopTimingQuery;
        m_DispatchTable[ToIndex(CommandType::BeginDebugGroup)] = BeginDebugGroup;
        m_DispatchTable[ToIndex(CommandType::EndDebugGroup)] = EndDebugGroup;
        m_DispatchTable[ToIndex(CommandType::DebugLabel)] = DebugLabel;
        m_DispatchTable[ToIndex(CommandType::SetBlendFactor)] = SetBlendFactor;
        m_DispatchTable[ToIndex(CommandType::SetStencilReference)] = SetStencilReference;
        m_DispatchTable[ToIndex(CommandType::BuildAccelerationStructures)] = BuildAccelerationStructures;
        m_DispatchTable[ToIndex(CommandType::CopyAccelerationStructure)] = CopyAccelerationStructures;
        m_DispatchTable[ToIndex(CommandType::CopyAccelerationStructureToDeviceBuffer)] =
            CopyAccelerationStructureToDeviceBuffer;
        m_DispatchTable[ToIndex(CommandType::CopyDeviceBufferToAccelerationStructure)] =
            CopyDeviceBufferToAccelerationStructure;
        m_DispatchTable[ToIndex(CommandType::EndRendering)] = EndRendering;
    }

    CommandExecutorD3D12::~CommandExecutorD3D12()
    {
        FreeLibrary(m_PixModule);
        m_PixModule = NULL;
    }

    void CommandExecutorD3D12::ExecuteCommands(ICommandList *commandList, IGraphicsDevice *device)
    {
        m_GraphicsDevice = reinterpret_cast<GraphicsDeviceD3D12 *>(device);

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

        m_CurrentFramebuffer = {};
    }

    void CommandExecutorD3D12::Reset()
    {
        m_DescriptorHandles = {};
        m_DepthHandle = {};

        m_CurrentlyBoundResourceSet = nullptr;

        m_CurrentFramebuffer = {};
        m_CurrentlyBoundPipeline = {};
    }

    void CommandExecutorD3D12::SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList)
    {
        m_CommandList = commandList;
    }

    void CommandExecutorD3D12::SetCommandQueue(CommandQueueD3D12 *commandQueue)
    {
        m_CommandQueue = commandQueue;
    }

    void CommandExecutorD3D12::FlushReadbacks(IGraphicsDevice *device)
    {
        if (m_ReadbackCopies.size() == 0)
        {
            return;
        }

        std::vector<D3D12ReadbackBufferCopyOperation> readbackCopies = m_ReadbackCopies;
        m_ReadbackCopies.clear();

        device->WaitForIdle();

        for (const D3D12ReadbackBufferCopyOperation &readbackCopy : readbackCopies)
        {
            const uint32_t copyPerRow = std::min(readbackCopy.SourceRowPitch, readbackCopy.DestinationRowPitch);

            uint8_t *srcData = readbackCopy.ReadbackBuffer->Map();

            std::vector<uint8_t> pixels(readbackCopy.NumRows * readbackCopy.DestinationRowPitch, 0);

            for (uint32_t y = 0; y < readbackCopy.NumRows; y++)
            {
                const uint8_t *srcRow = srcData + y * readbackCopy.SourceRowPitch;
                uint8_t *dstRow = pixels.data() + y * readbackCopy.DestinationRowPitch;
                memcpy(dstRow, srcRow, readbackCopy.DestinationRowPitch);
            }

            memcpy(srcData, pixels.data(), pixels.size());

            readbackCopy.ReadbackBuffer->Unmap();
        }

        device->WaitForIdle();
    }

    void CommandExecutorD3D12::ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
        {
            return;
        }

        if (m_CurrentlyBoundPipeline.IsValid() && m_CurrentlyBoundPipeline->GetType() == PipelineType::Graphics)
        {
            const GraphicsPipelineD3D12 *pipeline = m_CurrentlyBoundPipeline.AsDerived<const GraphicsPipelineD3D12>();
            const DeviceBufferD3D12 *d3d12VertexBuffer = command.View.BufferHandle.AsDerived<const DeviceBufferD3D12>();
            const auto &bufferLayout = pipeline->GetPipelineDescription().Layouts.at(command.Slot);

            D3D12_VERTEX_BUFFER_VIEW bufferView = {};
            bufferView.BufferLocation = d3d12VertexBuffer->GetHandle()->GetGPUVirtualAddress() + command.View.Offset;
            bufferView.SizeInBytes = command.View.Size;
            bufferView.StrideInBytes = pipeline->GetPipelineDescription().Layouts.at(command.Slot).GetStride();

            m_CommandList->IASetVertexBuffers(command.Slot, 1, &bufferView);
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
        {
            return;
        }

        const DeviceBufferD3D12 *d3d12IndexBuffer = command.View.BufferHandle.AsDerived<const DeviceBufferD3D12>();

        D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
        indexBufferView.BufferLocation = d3d12IndexBuffer->GetHandle()->GetGPUVirtualAddress() + command.View.Offset;
        indexBufferView.SizeInBytes = command.View.Size;
        indexBufferView.Format = D3D12::GetD3D12IndexBufferFormat(command.View.BufferFormat);

        m_CommandList->IASetIndexBuffer(&indexBufferView);
    }

    void CommandExecutorD3D12::ExecuteCommand(PipelineHandle command, IGraphicsDevice *device)
    {
        PipelineD3D12 *pipeline = command.AsDerived<PipelineD3D12>();
        pipeline->Bind(m_CommandList);
        m_CurrentlyBoundPipeline = command;
    }

    void CommandExecutorD3D12::ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
        {
            return;
        }

        m_CommandList->DrawInstanced(
            command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart
        );
    }

    void CommandExecutorD3D12::ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
        {
            return;
        }

        m_CommandList->DrawIndexedInstanced(
            command.IndexCount, command.InstanceCount, command.IndexStart, command.VertexStart, command.InstanceStart
        );
    }

    void CommandExecutorD3D12::ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
        {
            return;
        }

        NX_VALIDATE(
            command.IndirectBuffer->CheckUsage(Graphics::BufferUsage_Indirect),
            "Buffer passed to DrawIndirect is not an indirect buffer"
        );

        if (m_CurrentlyBoundPipeline.IsValid() && m_CurrentlyBoundPipeline->GetType() == PipelineType::Graphics)
        {
            if (const DeviceBufferD3D12 *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferD3D12>())
            {
                Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

                Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
                    GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DRAW, command.Stride);

                m_CommandList->ExecuteIndirect(
                    signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0
                );
            }
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
        {
            return;
        }

        NX_VALIDATE(
            command.IndirectBuffer->CheckUsage(Graphics::BufferUsage_Indirect),
            "Buffer passed to DrawIndirect is not an indirect buffer"
        );

        if (m_CurrentlyBoundPipeline.IsValid() && m_CurrentlyBoundPipeline->GetType() == PipelineType::Graphics)
        {
            if (const DeviceBufferD3D12 *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferD3D12>())
            {
                Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

                Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
                    GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED, command.Stride);

                m_CommandList->ExecuteIndirect(
                    signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0
                );
            }
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        m_CommandList->Dispatch(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
    }

    void CommandExecutorD3D12::ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        if (const DeviceBufferD3D12 *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferD3D12>())
        {
            Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

            Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
                GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH, command.Stride);

            m_CommandList->ExecuteIndirect(signature.Get(), 1, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        m_CommandList->DispatchMesh(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
    }

    void CommandExecutorD3D12::ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device)
    {
        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        {
            if (const DeviceBufferD3D12 *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferD3D12>())
            {
                Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

                Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
                    GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH, command.Stride);

                m_CommandList->ExecuteIndirect(
                    signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0
                );
            }
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device)
    {
        Nexus::Graphics::PipelineType pipelineType = m_CurrentlyBoundPipeline->GetType();

        ResourceSetHandle handle = desc.TargetResourceSet;
        ResourceSetD3D12 *d3d12ResourceSet = handle.AsDerived<ResourceSetD3D12>();
        GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

        if (d3d12ResourceSet)
        {
            m_CurrentlyBoundResourceSet = d3d12ResourceSet;

            bool isGraphics = pipelineType != PipelineType::Compute;
            d3d12ResourceSet->Bind(isGraphics, m_CommandList, desc.DynamicOffsets);
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device)
    {
        if (!ValidateForClearColour(m_CurrentFramebuffer, command.Index))
        {
            return;
        }

        float clearColor[] = {command.Colour.Red, command.Colour.Green, command.Colour.Blue, command.Colour.Alpha};

        if (command.Rect.has_value())
        {
            Graphics::ClearRect rect = command.Rect.value();

            D3D12_RECT d3d12Rect = {};
            d3d12Rect.left = rect.X;
            d3d12Rect.top = rect.Y;
            d3d12Rect.right = rect.X + rect.Width;
            d3d12Rect.bottom = rect.Y + rect.Height;

            const auto &handle = m_DescriptorHandles[command.Index];
            m_CommandList->ClearRenderTargetView(handle, clearColor, 1, &d3d12Rect);
        }
        else
        {
            const auto &handle = m_DescriptorHandles[command.Index];
            m_CommandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device)
    {
        if (m_DepthHandle.ptr)
        {
            D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;

            if (command.Rect.has_value())
            {
                Graphics::ClearRect rect = command.Rect.value();

                D3D12_RECT d3d12Rect = {};
                d3d12Rect.left = rect.X;
                d3d12Rect.top = rect.Y;
                d3d12Rect.right = rect.X + rect.Width;
                d3d12Rect.bottom = rect.Y + rect.Height;
                m_CommandList->ClearDepthStencilView(
                    m_DepthHandle, clearFlags, command.Value.Depth, command.Value.Stencil, 1, &d3d12Rect
                );
            }
            else
            {
                m_CommandList->ClearDepthStencilView(
                    m_DepthHandle, clearFlags, command.Value.Depth, command.Value.Stencil, 0, nullptr
                );
            }
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(FramebufferHandle command, IGraphicsDevice *device)
    {
        SetFramebuffer(command, device);
    }

    void CommandExecutorD3D12::ExecuteCommand(const Viewport &command, IGraphicsDevice *device)
    {
        D3D12_VIEWPORT vp = {};
        vp.TopLeftX = command.X;
        vp.TopLeftY = command.Y;
        vp.Width = command.Width;
        vp.Height = command.Height;
        vp.MinDepth = command.MinDepth;
        vp.MaxDepth = command.MaxDepth;
        m_CommandList->RSSetViewports(1, &vp);
    }

    void CommandExecutorD3D12::ExecuteCommand(const Scissor &command, IGraphicsDevice *device)
    {
        RECT rect = {};
        rect.left = command.X;
        rect.top = command.Y;
        rect.right = command.Width + command.X;
        rect.bottom = command.Height + command.Y;
        m_CommandList->RSSetScissorRects(1, &rect);
    }

    void CommandExecutorD3D12::ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device)
    {
        if (const TextureD3D12 *const source = command.Source.AsDerived<const TextureD3D12>())
        {
            uint32_t sourceSubresource =
                Utils::CalculateSubresource(command.SourceMipLevel, command.SourceArrayLayer, source->GetMipLevels());
            Microsoft::WRL::ComPtr<ID3D12Resource2> sourceHandle = source->GetHandle();

            if (const TextureD3D12 *dest = command.Destination.AsDerived<const TextureD3D12>())
            {
                uint32_t destinationSubresource = Utils::CalculateSubresource(
                    command.DestinationMipLevel, command.DestinationArrayLayer, dest->GetMipLevels()
                );
                Microsoft::WRL::ComPtr<ID3D12Resource2> destHandle = dest->GetHandle();

                PixelFormat destFormat = dest->GetPixelFormat();

                m_CommandList->ResolveSubresource(
                    destHandle.Get(), destinationSubresource, sourceHandle.Get(), sourceSubresource,
                    D3D12::GetD3D12PixelFormat(destFormat)
                );
            }
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device)
    {
        if (const TimingQueryD3D12 *queryD3D12 = command.Query.AsDerived<const TimingQueryD3D12>())
        {
            Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap = queryD3D12->GetQueryHeap();
            m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device)
    {
        if (const TimingQueryD3D12 *queryD3D12 = command.Query.AsDerived<const TimingQueryD3D12>())
        {
            Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap = queryD3D12->GetQueryHeap();
            m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device)
    {
        const DeviceBufferD3D12 *source = command.BufferCopy.Source.AsDerived<const DeviceBufferD3D12>();
        const DeviceBufferD3D12 *dest = command.BufferCopy.Destination.AsDerived<const DeviceBufferD3D12>();

        if (source && dest)
        {
            for (const auto &copy : command.BufferCopy.Copies)
            {
                m_CommandList->CopyBufferRegion(
                    dest->GetHandle().Get(), copy.WriteOffset, source->GetHandle().Get(), copy.ReadOffset, copy.Size
                );
            }
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device)
    {
        GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;
        Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

        const TextureD3D12 *texture = command.BufferTextureCopy.Texture.AsDerived<const TextureD3D12>();

        if (!texture)
        {
            return;
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> textureHandle = texture->GetHandle();

        const bool layeredTexture = texture->IsLayeredTexture();
        uint32_t subresourceIndex = Utils::CalculateSubresource(
            command.BufferTextureCopy.MipLevel, layeredTexture ? command.BufferTextureCopy.TextureOffset.Z : 0,
            command.BufferTextureCopy.Texture->GetMipLevels()
        );

        D3D12_BOX textureBounds = {};
        textureBounds.left = command.BufferTextureCopy.TextureOffset.X;
        textureBounds.right = command.BufferTextureCopy.TextureOffset.X + command.BufferTextureCopy.TextureExtent.Width;
        textureBounds.top = command.BufferTextureCopy.TextureOffset.Y;
        textureBounds.bottom =
            command.BufferTextureCopy.TextureOffset.Y + command.BufferTextureCopy.TextureExtent.Height;

        if (texture->GetType() == TextureType::Texture3D)
        {
            textureBounds.front = command.BufferTextureCopy.TextureOffset.Z;
            textureBounds.back = command.BufferTextureCopy.TextureOffset.Z + 1;
        }
        else
        {
            textureBounds.front = 0;
            textureBounds.back = 1;
        }

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
        UINT numRows = {};
        UINT64 rowSizeInBytes = {};
        UINT64 totalBytes = {};

        D3D12_RESOURCE_DESC resourceDesc = textureHandle->GetDesc();
        nativeDevice->GetCopyableFootprints(
            &resourceDesc, subresourceIndex, 1, command.BufferTextureCopy.BufferOffset, &footprint, &numRows,
            &rowSizeInBytes, &totalBytes
        );

        DeviceBufferHandle stagingBuffer = CreateStagingBuffer(totalBytes, true, device);
        DeviceBufferHandle destHandle = command.BufferTextureCopy.BufferHandle;
        DeviceBufferD3D12 *sourceBufferD3D12 = destHandle.AsDerived<DeviceBufferD3D12>();
        const DeviceBufferD3D12 *stagingBufferD3D12 = stagingBuffer.AsDerived<const DeviceBufferD3D12>();
        Microsoft::WRL::ComPtr<ID3D12Resource> bufferHandle = stagingBufferD3D12->GetHandle();

        uint8_t *sourceData = sourceBufferD3D12->Map();
        uint8_t *stagingData = stagingBuffer->Map();

        if (!texture || !sourceBufferD3D12 || !stagingBufferD3D12)
        {
            throw std::runtime_error("Invalid cast in ExecuteCommand");
        }

        uint32_t rowSize =
            command.BufferTextureCopy.TextureExtent.Width * GetPixelFormatSizeInBytes(texture->GetPixelFormat());

        for (uint32_t y = 0; y < command.BufferTextureCopy.TextureExtent.Height; y++)
        {
            uint8_t *dstRow = stagingData + y * footprint.Footprint.RowPitch;
            uint8_t *srcRow = sourceData + y * rowSize;

            memcpy(dstRow, srcRow, rowSize);

            // Optional: clear padding to avoid garbage
            if (rowSize < footprint.Footprint.RowPitch)
            {
                memset(dstRow + rowSize, 0, footprint.Footprint.RowPitch - rowSize);
            }
        }

        sourceBufferD3D12->Unmap();
        stagingBuffer->Unmap();

        D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
        srcLocation.pResource = bufferHandle.Get();
        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        srcLocation.PlacedFootprint = footprint;

        D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
        dstLocation.pResource = textureHandle.Get();
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstLocation.SubresourceIndex = subresourceIndex;

        const uint32_t zOffset =
            texture->GetType() == TextureType::Texture3D ? command.BufferTextureCopy.TextureOffset.Z : 0;

        m_CommandList->CopyTextureRegion(
            &dstLocation, command.BufferTextureCopy.TextureOffset.X, command.BufferTextureCopy.TextureOffset.Y, zOffset,
            &srcLocation, &textureBounds
        );
    }

    void CommandExecutorD3D12::ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device)
    {
        GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;
        Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

        DeviceBufferHandle handle = command.TextureBufferCopy.BufferHandle;
        DeviceBufferD3D12 *buffer = handle.AsDerived<DeviceBufferD3D12>();
        const TextureD3D12 *texture = command.TextureBufferCopy.Texture.AsDerived<const TextureD3D12>();

        if (!texture)
        {
            return;
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> textureHandle = texture->GetHandle();

        bool arrayedTexture = command.TextureBufferCopy.Texture->GetType() == TextureType::Texture3D ||
                              command.TextureBufferCopy.Texture->GetType() == TextureType::TextureCube;
        uint32_t subresourceIndex = Utils::CalculateSubresource(
            command.TextureBufferCopy.MipLevel, arrayedTexture ? command.TextureBufferCopy.TextureOffset.Z : 0,
            command.TextureBufferCopy.Texture->GetMipLevels()
        );

        D3D12_BOX textureBounds = {};
        textureBounds.left = command.TextureBufferCopy.TextureOffset.X;
        textureBounds.right = command.TextureBufferCopy.TextureOffset.X + command.TextureBufferCopy.TextureExtent.Width;
        textureBounds.top = command.TextureBufferCopy.TextureOffset.Y;
        textureBounds.bottom =
            command.TextureBufferCopy.TextureOffset.Y + command.TextureBufferCopy.TextureExtent.Height;
        textureBounds.front = command.TextureBufferCopy.TextureOffset.Z;
        textureBounds.back = command.TextureBufferCopy.TextureOffset.Z + 1;

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
        UINT numRows = {};
        UINT64 rowSizeInBytes = {};
        UINT64 totalBytes = {};

        D3D12_RESOURCE_DESC resourceDesc = textureHandle->GetDesc();
        nativeDevice->GetCopyableFootprints(
            &resourceDesc, subresourceIndex, 1, command.TextureBufferCopy.BufferOffset, &footprint, &numRows,
            &rowSizeInBytes, &totalBytes
        );

        Microsoft::WRL::ComPtr<ID3D12Resource> bufferHandle = buffer->GetHandle();

        D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
        srcLocation.pResource = textureHandle.Get();
        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        srcLocation.SubresourceIndex = subresourceIndex;

        D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
        dstLocation.pResource = bufferHandle.Get();
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        dstLocation.PlacedFootprint = footprint;

        // copy texture data into the buffer (the 0's are for the offset into the
        // destination texture, which we do not need here)
        m_CommandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, &textureBounds);

        size_t tightPitch =
            command.TextureBufferCopy.TextureExtent.Width * GetPixelFormatSizeInBytes(texture->GetDescription().Format);

        D3D12ReadbackBufferCopyOperation &readbackOperation = m_ReadbackCopies.emplace_back();
        readbackOperation.ReadbackBuffer = buffer;
        readbackOperation.SourceRowPitch = footprint.Footprint.RowPitch;
        readbackOperation.DestinationRowPitch = tightPitch;
        readbackOperation.Height = command.TextureBufferCopy.TextureExtent.Height;
        readbackOperation.NumRows = numRows;
    }

    void CommandExecutorD3D12::ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device)
    {
        GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;
        Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

        const TextureD3D12 *srcTexture = command.TextureCopy.Source.AsDerived<const TextureD3D12>();
        const TextureD3D12 *dstTexture = command.TextureCopy.Destination.AsDerived<const TextureD3D12>();

        Microsoft::WRL::ComPtr<ID3D12Resource2> srcHandle = srcTexture->GetHandle();
        Microsoft::WRL::ComPtr<ID3D12Resource2> dstHandle = dstTexture->GetHandle();

        // retrieve source index
        bool srcArrayedTexture =
            srcTexture->GetType() == TextureType::Texture3D || srcTexture->GetType() == TextureType::TextureCube;
        uint32_t srcSubresourceIndex = Utils::CalculateSubresource(
            command.TextureCopy.SourceMipLevel, srcArrayedTexture ? command.TextureCopy.SourceOffset.Z : 0,
            command.TextureCopy.Source->GetMipLevels()
        );

        // retrieve destination footprint
        bool dstArrayedTexture =
            dstTexture->GetType() == TextureType::Texture3D || dstTexture->GetType() == TextureType::TextureCube;
        uint32_t dstSubresourceIndex = Utils::CalculateSubresource(
            command.TextureCopy.DestinationMipLevel, dstArrayedTexture ? command.TextureCopy.DestinationOffset.Z : 0,
            command.TextureCopy.Destination->GetMipLevels()
        );

        D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
        srcLocation.pResource = srcHandle.Get();
        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        srcLocation.SubresourceIndex = srcSubresourceIndex;

        D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
        dstLocation.pResource = dstHandle.Get();
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstLocation.SubresourceIndex = dstSubresourceIndex;

        TextureLayout srcLayout =
            srcTexture->GetTextureLayout(command.TextureCopy.SourceOffset.Z, command.TextureCopy.SourceMipLevel);
        D3D12_RESOURCE_STATES srcResourceState = D3D12::GetTextureResourceState(srcLayout);

        TextureLayout dstLayout = dstTexture->GetTextureLayout(
            command.TextureCopy.DestinationOffset.Z, command.TextureCopy.DestinationMipLevel
        );
        D3D12_RESOURCE_STATES dstResourceState = D3D12::GetTextureResourceState(dstLayout);

        D3D12_BOX textureBounds = {};
        textureBounds.left = command.TextureCopy.SourceOffset.X;
        textureBounds.right = command.TextureCopy.SourceOffset.X + command.TextureCopy.Extent.Width;
        textureBounds.top = command.TextureCopy.SourceOffset.Y;
        textureBounds.bottom = command.TextureCopy.SourceOffset.Y + command.TextureCopy.Extent.Height;
        textureBounds.front = command.TextureCopy.SourceOffset.Z;
        textureBounds.back = command.TextureCopy.SourceOffset.Z + 1;

        m_CommandList->CopyTextureRegion(
            &dstLocation, command.TextureCopy.DestinationOffset.X, command.TextureCopy.DestinationOffset.Y,
            command.TextureCopy.DestinationOffset.Z, &srcLocation, &textureBounds
        );
    }

    void CommandExecutorD3D12::ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device)
    {
        if (m_PIXBeginEvent && m_PIXEndEvent)
        {
            uint32_t colour = Utils::PackColour(command.Colour);
            m_PIXBeginEvent(m_CommandList.Get(), colour, command.GroupName.c_str());
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device)
    {
        if (m_PIXBeginEvent && m_PIXEndEvent)
        {
            m_PIXEndEvent(m_CommandList.Get());
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device)
    {
        if (m_PIXSetMarker)
        {
            uint32_t colour = Utils::PackColour(command.Colour);
            m_PIXSetMarker(m_CommandList.Get(), colour, command.MarkerName.c_str());
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device)
    {
        float blendFactor[4] = {
            command.BlendFactor.Red, command.BlendFactor.Green, command.BlendFactor.Blue, command.BlendFactor.Alpha
        };

        m_CommandList->OMSetBlendFactor(blendFactor);
    }

    void CommandExecutorD3D12::ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device)
    {
        m_CommandList->OMSetStencilRef(command.StencilReference);
    }

    void CommandExecutorD3D12::ExecuteCommand(
        const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device
    )
    {
        for (const auto &accelerationStructureBuildDesc : command.BuildDescriptions)
        {
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
            std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometry = {};

            D3D12::GetD3D12AccelerationStructureInputs(accelerationStructureBuildDesc, inputs, geometry);

            D3D12_GPU_VIRTUAL_ADDRESS srcAddress = 0;
            D3D12_GPU_VIRTUAL_ADDRESS destAddress = 0;
            D3D12_GPU_VIRTUAL_ADDRESS scratchAddress = accelerationStructureBuildDesc.ScratchBuffer;

            if (accelerationStructureBuildDesc.Source.IsValid())
            {
                srcAddress = accelerationStructureBuildDesc.Source->GetDeviceAddress(0);
            }

            if (accelerationStructureBuildDesc.Destination.IsValid())
            {
                destAddress = accelerationStructureBuildDesc.Destination->GetDeviceAddress(0);
            }

            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
            buildDesc.SourceAccelerationStructureData = srcAddress;
            buildDesc.DestAccelerationStructureData = destAddress;
            buildDesc.ScratchAccelerationStructureData = scratchAddress;
            buildDesc.Inputs = inputs;
            m_CommandList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(
        const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device
    )
    {
    }

    void CommandExecutorD3D12::ExecuteCommand(
        const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device
    )
    {
    }

    void CommandExecutorD3D12::ExecuteCommand(
        const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device
    )
    {
    }

    void CommandExecutorD3D12::ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device)
    {
        if (!m_CurrentlyBoundResourceSet && !m_CurrentlyBoundPipeline.IsValid())
            return;

        bool isGraphics = m_CurrentlyBoundPipeline->GetType() != PipelineType::Compute;

        m_CurrentlyBoundResourceSet->SetPushConstants(
            command.Name, command.Data.data(), command.Offset, command.Data.size(), isGraphics, m_CommandList
        );
    }

    void CommandExecutorD3D12::ExecuteCommand(const BarrierGroupDescription &command, IGraphicsDevice *device)
    {
        GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;
        const auto &d3d12Features = deviceD3D12->GetD3D12DeviceFeatures();

        // enhanced barriers
        if (d3d12Features.SupportsEnhancedBarriers)
        {
            std::vector<D3D12_GLOBAL_BARRIER> globalBarriers = {};
            std::vector<D3D12_TEXTURE_BARRIER> textureBarriers = {};
            std::vector<D3D12_BUFFER_BARRIER> bufferBarriers = {};

            // memory barriers
            for (const auto &memoryBarrier : command.MemoryBarriers)
            {
                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(memoryBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(memoryBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(memoryBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(memoryBarrier.AfterAccess);

                D3D12_GLOBAL_BARRIER &barrier = globalBarriers.emplace_back();
                barrier.SyncBefore = beforeSync;
                barrier.SyncAfter = afterSync;
                barrier.AccessBefore = beforeAccess;
                barrier.AccessAfter = afterAccess;
            }

            // texture barriers
            for (const auto &textureBarrier : command.TextureBarriers)
            {
                const TextureD3D12 *texture = textureBarrier.Texture.AsDerived<const TextureD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = texture->GetHandle();

                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(textureBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(textureBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(textureBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(textureBarrier.AfterAccess);

                D3D12_BARRIER_LAYOUT afterLayout = D3D12::GetBarrierLayout(textureBarrier.Layout);

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
                        D3D12_BARRIER_LAYOUT beforeLayout =
                            D3D12::GetBarrierLayout(texture->GetTextureLayout(arrayLayer, mipLevel));

                        if (beforeLayout == afterLayout)
                        {
                            continue;
                        }

                        D3D12_TEXTURE_BARRIER &barrier = textureBarriers.emplace_back();
                        barrier.SyncBefore = beforeSync;
                        barrier.SyncAfter = afterSync;
                        barrier.AccessBefore = beforeAccess;
                        barrier.AccessAfter = afterAccess;
                        barrier.LayoutBefore = beforeLayout;
                        barrier.LayoutAfter = afterLayout;
                        barrier.pResource = handle.Get();
                        barrier.Subresources.FirstArraySlice = arrayLayer;
                        barrier.Subresources.NumArraySlices = 1;
                        barrier.Subresources.IndexOrFirstMipLevel = mipLevel;
                        barrier.Subresources.NumMipLevels = 1;
                        barrier.Subresources.FirstPlane = 0;
                        barrier.Subresources.NumPlanes = 1;
                    }
                }
            }

            // buffer barriers
            for (const auto &bufferBarrier : command.BufferBarriers)
            {
                const DeviceBufferD3D12 *buffer = bufferBarrier.Buffer.AsDerived<DeviceBufferD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(bufferBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(bufferBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(bufferBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(bufferBarrier.AfterAccess);

                D3D12_BUFFER_BARRIER barrier = bufferBarriers.emplace_back();
                barrier.SyncBefore = beforeSync;
                barrier.SyncAfter = afterSync;
                barrier.AccessBefore = beforeAccess;
                barrier.AccessAfter = afterAccess;
                barrier.pResource = handle.Get();

                // these have to be hardcoded to these values as transitioning part
                // of a buffer is not currently supported
                barrier.Offset = 0;
                barrier.Size = UINT64_MAX;
            }

            // create the barriers to submit
            std::vector<D3D12_BARRIER_GROUP> barrierGroups = {};
            if (globalBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &globalBarrierGroup = barrierGroups.emplace_back();
                globalBarrierGroup.Type = D3D12_BARRIER_TYPE_GLOBAL;
                globalBarrierGroup.NumBarriers = static_cast<UINT32>(globalBarriers.size());
                globalBarrierGroup.pGlobalBarriers = globalBarriers.data();
            }

            if (textureBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &textureBarrierGroup = barrierGroups.emplace_back();
                textureBarrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
                textureBarrierGroup.NumBarriers = static_cast<UINT32>(textureBarriers.size());
                textureBarrierGroup.pTextureBarriers = textureBarriers.data();
            }

            if (bufferBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &bufferBarrierGroup = barrierGroups.emplace_back();
                bufferBarrierGroup.Type = D3D12_BARRIER_TYPE_BUFFER;
                bufferBarrierGroup.NumBarriers = static_cast<UINT32>(bufferBarriers.size());
                bufferBarrierGroup.pBufferBarriers = bufferBarriers.data();
            }

            m_CommandList->Barrier(static_cast<UINT32>(barrierGroups.size()), barrierGroups.data());
        }
        // resource barriers
        else
        {
            std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers = {};

            for (const auto &textureBarrier : command.TextureBarriers)
            {
                const TextureD3D12 *texture = textureBarrier.Texture.AsDerived<const TextureD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = texture->GetHandle();

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
                        D3D12_RESOURCE_STATES beforeState =
                            D3D12::GetTextureResourceState(texture->GetTextureLayout(arrayLayer, mipLevel));
                        D3D12_RESOURCE_STATES afterState = D3D12::GetTextureResourceState(textureBarrier.Layout);

                        if (beforeState == afterState)
                        {
                            continue;
                        }

                        uint32_t subresourceIndex =
                            Utils::CalculateSubresource(mipLevel, arrayLayer, texture->GetDescription().MipLevels);

                        D3D12_RESOURCE_BARRIER &barrier = resourceBarriers.emplace_back();
                        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                        barrier.Transition.pResource = handle.Get();
                        barrier.Transition.Subresource = subresourceIndex;
                        barrier.Transition.StateBefore = beforeState;
                        barrier.Transition.StateAfter = afterState;
                    }
                }
            }

            for (const auto &bufferBarrier : command.BufferBarriers)
            {
                if (bufferBarrier.BeforeAccess == BarrierAccess::ShaderWrite)
                {
                    const DeviceBufferD3D12 *buffer = bufferBarrier.Buffer.AsDerived<DeviceBufferD3D12>();
                    Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

                    D3D12_RESOURCE_BARRIER barrier = resourceBarriers.emplace_back();
                    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
                    barrier.UAV.pResource = handle.Get();
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                }
            }

            m_CommandList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), resourceBarriers.data());
        }

        // update texture layouts
        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (const TextureBarrierDesc &textureBarrier : command.TextureBarriers)
        {
            TextureHandle textureHandle = textureBarrier.Texture;
            TextureD3D12 *textureD3D12 = textureHandle.AsDerived<TextureD3D12>();

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
                    textureD3D12->SetTextureLayout(arrayLayer, mipLevel, textureBarrier.Layout);
                }
            }
        }
    }

    void CommandExecutorD3D12::ExecuteCommand(const TraceRaysDescription &desc, IGraphicsDevice *device)
    {
    }

    void CommandExecutorD3D12::ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device)
    {
        FramebufferHandle framebuffer = command.TargetFramebuffer;

        if (framebuffer->GetSampleCount() > 1)
        {
            for (size_t textureIndex = 0; textureIndex < framebuffer->GetColorTextureCount(); textureIndex++)
            {
                std::optional<FramebufferColourAttachmentDescription> colourAttachmentDescOpt =
                    framebuffer->GetColorTextureBinding(textureIndex);
                if (colourAttachmentDescOpt.has_value())
                {
                    FramebufferColourAttachmentDescription colourAttachmentDesc = colourAttachmentDescOpt.value();
                    if (colourAttachmentDesc.ResolveAttachment.has_value())
                    {
                        for (uint32_t layerIndex = 0; layerIndex < colourAttachmentDesc.ColourAttachment.LayerCount;
                             layerIndex++)
                        {
                            FramebufferTextureDescription resolveAttachmentDesc =
                                colourAttachmentDesc.ResolveAttachment.value();

                            uint32_t sourceArrayIndex =
                                colourAttachmentDesc.ColourAttachment.BaseArrayLayer + layerIndex;
                            uint32_t destArrayIndex = resolveAttachmentDesc.BaseArrayLayer + layerIndex;

                            TextureLayout sourceLayout =
                                colourAttachmentDesc.ColourAttachment.TargetTexture->GetTextureLayout(
                                    sourceArrayIndex, colourAttachmentDesc.ColourAttachment.MipLevel
                                );
                            TextureLayout destLayout = resolveAttachmentDesc.TargetTexture->GetTextureLayout(
                                destArrayIndex, resolveAttachmentDesc.MipLevel
                            );

                            BarrierGroupDescription barrierGroup = {};

                            // to resolve compatible layouts
                            {
                                TextureBarrierDesc sourceBarrier = {};
                                sourceBarrier.Texture = colourAttachmentDesc.ColourAttachment.TargetTexture;
                                sourceBarrier.BeforeAccess = BarrierAccess::ColourAttachmentWrite;
                                sourceBarrier.AfterAccess = BarrierAccess::ColourAttachmentRead;
                                sourceBarrier.BeforeStage = BarrierPipelineStage::ColourAttachmentOutput;
                                sourceBarrier.AfterStage = BarrierPipelineStage::Resolve;
                                sourceBarrier.Layout = TextureLayout::ResolveSrc;
                                sourceBarrier.TextureSubresourceRange = {
                                    .BaseMipLevel = colourAttachmentDesc.ColourAttachment.MipLevel,
                                    .LevelCount = 1,
                                    .BaseArrayLayer = sourceArrayIndex,
                                    .LayerCount = 1
                                };
                                barrierGroup.TextureBarriers.emplace_back(sourceBarrier);

                                TextureBarrierDesc destBarrier = {};
                                destBarrier.Texture = resolveAttachmentDesc.TargetTexture;
                                destBarrier.BeforeAccess = BarrierAccess::ColourAttachmentWrite;
                                destBarrier.AfterAccess = BarrierAccess::ColourAttachmentRead;
                                destBarrier.BeforeStage = BarrierPipelineStage::ColourAttachmentOutput;
                                destBarrier.AfterStage = BarrierPipelineStage::Resolve;
                                destBarrier.Layout = TextureLayout::ResolveDest;
                                destBarrier.TextureSubresourceRange = {
                                    .BaseMipLevel = resolveAttachmentDesc.MipLevel,
                                    .LevelCount = 1,
                                    .BaseArrayLayer = destArrayIndex,
                                    .LayerCount = 1
                                };
                                barrierGroup.TextureBarriers.emplace_back(destBarrier);

                                ExecuteCommand(barrierGroup, device);
                            }

                            // execute resolve
                            {
                                ResolveTextureDescription resolveDesc = {};
                                resolveDesc.Source = colourAttachmentDesc.ColourAttachment.TargetTexture;
                                resolveDesc.SourceMipLevel = colourAttachmentDesc.ColourAttachment.MipLevel;
                                resolveDesc.SourceArrayLayer = sourceArrayIndex;
                                resolveDesc.Destination = resolveAttachmentDesc.TargetTexture;
                                resolveDesc.DestinationMipLevel = resolveAttachmentDesc.MipLevel;
                                resolveDesc.DestinationArrayLayer = destArrayIndex;

                                ExecuteCommand(resolveDesc, device);
                            }

                            // to resolve compatible layouts
                            {
                                BarrierGroupDescription barrierGroup = {};

                                TextureBarrierDesc sourceBarrier = {};
                                sourceBarrier.Texture = colourAttachmentDesc.ColourAttachment.TargetTexture;
                                sourceBarrier.BeforeAccess = BarrierAccess::TransferRead;
                                sourceBarrier.AfterAccess = BarrierAccess::NoAccess;
                                sourceBarrier.BeforeStage = BarrierPipelineStage::Resolve;
                                sourceBarrier.AfterStage = BarrierPipelineStage::NoStage;
                                sourceBarrier.Layout = sourceLayout;
                                sourceBarrier.TextureSubresourceRange = {
                                    .BaseMipLevel = colourAttachmentDesc.ColourAttachment.MipLevel,
                                    .LevelCount = 1,
                                    .BaseArrayLayer = sourceArrayIndex,
                                    .LayerCount = 1
                                };
                                barrierGroup.TextureBarriers.emplace_back(sourceBarrier);

                                TextureBarrierDesc destBarrier = {};
                                destBarrier.Texture = resolveAttachmentDesc.TargetTexture;
                                destBarrier.BeforeAccess = BarrierAccess::TransferRead;
                                destBarrier.AfterAccess = BarrierAccess::NoAccess;
                                destBarrier.BeforeStage = BarrierPipelineStage::Resolve;
                                destBarrier.AfterStage = BarrierPipelineStage::NoStage;
                                destBarrier.Layout = destLayout;
                                destBarrier.TextureSubresourceRange = {
                                    .BaseMipLevel = resolveAttachmentDesc.MipLevel,
                                    .LevelCount = 1,
                                    .BaseArrayLayer = destArrayIndex,
                                    .LayerCount = 1
                                };
                                barrierGroup.TextureBarriers.emplace_back(destBarrier);
                            }
                        }
                    }
                }
            }
        }
    }

    void CommandExecutorD3D12::SetFramebuffer(FramebufferHandle framebuffer, IGraphicsDevice *device)
    {
        ResetPreviousRenderTargets(device);
        GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

        if (framebuffer.IsValid())
        {
            FramebufferD3D12 *framebufferD3D12 = framebuffer.AsDerived<FramebufferD3D12>();

            m_DescriptorHandles = framebufferD3D12->GetColourAttachmentCPUHandles();
            m_DepthHandle = framebufferD3D12->GetDepthAttachmentCPUHandle();

            if (!framebufferD3D12->HasDepthTexture())
            {
                m_DepthHandle = {};
            }

            if (framebufferD3D12->HasDepthTexture())
            {
                m_CommandList->OMSetRenderTargets(
                    m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, &m_DepthHandle
                );
            }
            else
            {
                m_CommandList->OMSetRenderTargets(
                    m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, nullptr
                );
            }

            m_CurrentFramebuffer = framebuffer;
        }
    }

    void CommandExecutorD3D12::ResetPreviousRenderTargets(IGraphicsDevice *device)
    {
        m_CurrentFramebuffer = {};
        m_DepthHandle = {};
    }

    void CommandExecutorD3D12::CreateDrawIndirectSignatureCommand()
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

        D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
        commandSignatureDesc.pArgumentDescs = &argumentDesc;
        commandSignatureDesc.NumArgumentDescs = 1;
        commandSignatureDesc.ByteStride = sizeof(D3D12_DRAW_ARGUMENTS);

        m_Device->CreateCommandSignature(
            &commandSignatureDesc, nullptr, IID_PPV_ARGS(m_DrawIndirectCommandSignature.GetAddressOf())
        );
    }

    void CommandExecutorD3D12::CreateDrawIndexedIndirectSignatureCommand()
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

        D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
        commandSignatureDesc.pArgumentDescs = &argumentDesc;
        commandSignatureDesc.NumArgumentDescs = 1;
        commandSignatureDesc.ByteStride = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);

        m_Device->CreateCommandSignature(
            &commandSignatureDesc, nullptr, IID_PPV_ARGS(m_DrawIndexedIndirectCommandSignature.GetAddressOf())
        );
    }

    void CommandExecutorD3D12::CreateDispatchIndirectSignatureCommand()
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

        D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
        commandSignatureDesc.pArgumentDescs = &argumentDesc;
        commandSignatureDesc.NumArgumentDescs = 1;
        commandSignatureDesc.ByteStride = sizeof(D3D12_DISPATCH_ARGUMENTS);

        m_Device->CreateCommandSignature(
            &commandSignatureDesc, nullptr, IID_PPV_ARGS(m_DispatchIndirectCommandSignature.GetAddressOf())
        );
    }

    void CommandExecutorD3D12::SubmitBarrierGroupImpl(const BarrierGroupDescription &barrierGroupDesc)
    {
        const auto &d3d12Features = m_GraphicsDevice->GetD3D12DeviceFeatures();

        // enhanced barriers
        if (d3d12Features.SupportsEnhancedBarriers)
        {
            std::vector<D3D12_GLOBAL_BARRIER> globalBarriers = {};
            std::vector<D3D12_TEXTURE_BARRIER> textureBarriers = {};
            std::vector<D3D12_BUFFER_BARRIER> bufferBarriers = {};

            // memory barriers
            for (const auto &memoryBarrier : barrierGroupDesc.MemoryBarriers)
            {
                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(memoryBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(memoryBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(memoryBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(memoryBarrier.AfterAccess);

                D3D12_GLOBAL_BARRIER &barrier = globalBarriers.emplace_back();
                barrier.SyncBefore = beforeSync;
                barrier.SyncAfter = afterSync;
                barrier.AccessBefore = beforeAccess;
                barrier.AccessAfter = afterAccess;
            }

            // texture barriers
            for (const auto &textureBarrier : barrierGroupDesc.TextureBarriers)
            {
                const TextureD3D12 *texture = textureBarrier.Texture.AsDerived<const TextureD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = texture->GetHandle();

                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(textureBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(textureBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(textureBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(textureBarrier.AfterAccess);

                D3D12_BARRIER_LAYOUT afterLayout = D3D12::GetBarrierLayout(textureBarrier.Layout);

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
                        D3D12_BARRIER_LAYOUT beforeLayout =
                            D3D12::GetBarrierLayout(texture->GetTextureLayout(arrayLayer, mipLevel));

                        if (beforeLayout == afterLayout)
                        {
                            continue;
                        }

                        D3D12_TEXTURE_BARRIER &barrier = textureBarriers.emplace_back();
                        barrier.SyncBefore = beforeSync;
                        barrier.SyncAfter = afterSync;
                        barrier.AccessBefore = beforeAccess;
                        barrier.AccessAfter = afterAccess;
                        barrier.LayoutBefore = beforeLayout;
                        barrier.LayoutAfter = afterLayout;
                        barrier.pResource = handle.Get();
                        barrier.Subresources.FirstArraySlice = arrayLayer;
                        barrier.Subresources.NumArraySlices = 1;
                        barrier.Subresources.IndexOrFirstMipLevel = mipLevel;
                        barrier.Subresources.NumMipLevels = 1;
                        barrier.Subresources.FirstPlane = 0;
                        barrier.Subresources.NumPlanes = 1;
                    }
                }
            }

            // buffer barriers
            for (const auto &bufferBarrier : barrierGroupDesc.BufferBarriers)
            {
                const DeviceBufferD3D12 *buffer = bufferBarrier.Buffer.AsDerived<DeviceBufferD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

                D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(bufferBarrier.BeforeStage);
                D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(bufferBarrier.AfterStage);
                D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(bufferBarrier.BeforeAccess);
                D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(bufferBarrier.AfterAccess);

                D3D12_BUFFER_BARRIER barrier = bufferBarriers.emplace_back();
                barrier.SyncBefore = beforeSync;
                barrier.SyncAfter = afterSync;
                barrier.AccessBefore = beforeAccess;
                barrier.AccessAfter = afterAccess;
                barrier.pResource = handle.Get();

                // these have to be hardcoded to these values as transitioning part
                // of a buffer is not currently supported
                barrier.Offset = 0;
                barrier.Size = UINT64_MAX;
            }

            // create the barriers to submit
            std::vector<D3D12_BARRIER_GROUP> barrierGroups = {};
            if (globalBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &globalBarrierGroup = barrierGroups.emplace_back();
                globalBarrierGroup.Type = D3D12_BARRIER_TYPE_GLOBAL;
                globalBarrierGroup.NumBarriers = static_cast<UINT32>(globalBarriers.size());
                globalBarrierGroup.pGlobalBarriers = globalBarriers.data();
            }

            if (textureBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &textureBarrierGroup = barrierGroups.emplace_back();
                textureBarrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
                textureBarrierGroup.NumBarriers = static_cast<UINT32>(textureBarriers.size());
                textureBarrierGroup.pTextureBarriers = textureBarriers.data();
            }

            if (bufferBarriers.size() > 0)
            {
                D3D12_BARRIER_GROUP &bufferBarrierGroup = barrierGroups.emplace_back();
                bufferBarrierGroup.Type = D3D12_BARRIER_TYPE_BUFFER;
                bufferBarrierGroup.NumBarriers = static_cast<UINT32>(bufferBarriers.size());
                bufferBarrierGroup.pBufferBarriers = bufferBarriers.data();
            }

            m_CommandList->Barrier(static_cast<UINT32>(barrierGroups.size()), barrierGroups.data());
        }
        // resource barriers
        else
        {
            std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers = {};

            for (const auto &textureBarrier : barrierGroupDesc.TextureBarriers)
            {
                const TextureD3D12 *texture = textureBarrier.Texture.AsDerived<const TextureD3D12>();
                Microsoft::WRL::ComPtr<ID3D12Resource2> handle = texture->GetHandle();

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
                        D3D12_RESOURCE_STATES beforeState =
                            D3D12::GetTextureResourceState(texture->GetTextureLayout(arrayLayer, mipLevel));
                        D3D12_RESOURCE_STATES afterState = D3D12::GetTextureResourceState(textureBarrier.Layout);

                        if (beforeState == afterState)
                        {
                            continue;
                        }

                        uint32_t subresourceIndex =
                            Utils::CalculateSubresource(mipLevel, arrayLayer, texture->GetDescription().MipLevels);

                        D3D12_RESOURCE_BARRIER &barrier = resourceBarriers.emplace_back();
                        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                        barrier.Transition.pResource = handle.Get();
                        barrier.Transition.Subresource = subresourceIndex;
                        barrier.Transition.StateBefore = beforeState;
                        barrier.Transition.StateAfter = afterState;
                    }
                }
            }

            for (const auto &bufferBarrier : barrierGroupDesc.BufferBarriers)
            {
                if (bufferBarrier.BeforeAccess == BarrierAccess::ShaderWrite)
                {
                    const DeviceBufferD3D12 *buffer = bufferBarrier.Buffer.AsDerived<DeviceBufferD3D12>();
                    Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

                    D3D12_RESOURCE_BARRIER barrier = resourceBarriers.emplace_back();
                    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
                    barrier.UAV.pResource = handle.Get();
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                }
            }

            m_CommandList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), resourceBarriers.data());
        }

        // update texture layouts
        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (const TextureBarrierDesc &textureBarrier : barrierGroupDesc.TextureBarriers)
        {
            TextureHandle textureHandle = textureBarrier.Texture;
            TextureD3D12 *textureD3D12 = textureHandle.AsDerived<TextureD3D12>();

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
                    textureD3D12->SetTextureLayout(arrayLayer, mipLevel, textureBarrier.Layout);
                }
            }
        }
    }

    void CommandExecutorD3D12::ResolveTextureImpl(const ResolveTextureDescription &resolveDesc)
    {
        if (const TextureD3D12 *const source = resolveDesc.Source.AsDerived<const TextureD3D12>())
        {
            uint32_t sourceSubresource = Utils::CalculateSubresource(
                resolveDesc.SourceMipLevel, resolveDesc.SourceArrayLayer, source->GetMipLevels()
            );
            Microsoft::WRL::ComPtr<ID3D12Resource2> sourceHandle = source->GetHandle();

            if (const TextureD3D12 *dest = resolveDesc.Destination.AsDerived<const TextureD3D12>())
            {
                uint32_t destinationSubresource = Utils::CalculateSubresource(
                    resolveDesc.DestinationMipLevel, resolveDesc.DestinationArrayLayer, dest->GetMipLevels()
                );
                Microsoft::WRL::ComPtr<ID3D12Resource2> destHandle = dest->GetHandle();

                PixelFormat destFormat = dest->GetPixelFormat();

                m_CommandList->ResolveSubresource(
                    destHandle.Get(), destinationSubresource, sourceHandle.Get(), sourceSubresource,
                    D3D12::GetD3D12PixelFormat(destFormat)
                );
            }
        }
    }

    Microsoft::WRL::ComPtr<ID3D12CommandSignature> CommandExecutorD3D12::GetOrCreateIndirectCommandSignature(
        D3D12_INDIRECT_ARGUMENT_TYPE type, size_t stride
    )
    {
        // element found in map
        if (m_IndirectCommandSignatures[type].find(stride) != m_IndirectCommandSignatures[type].end())
        {
            return m_IndirectCommandSignatures[type][stride];
        }
        else
        {
            D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
            argumentDesc.Type = type;

            D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
            commandSignatureDesc.pArgumentDescs = &argumentDesc;
            commandSignatureDesc.NumArgumentDescs = 1;
            commandSignatureDesc.ByteStride = stride;

            Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature;
            m_Device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(signature.GetAddressOf()));
            m_IndirectCommandSignatures[type][stride] = signature;
            return signature;
        }
    }

    void CommandExecutorD3D12::InsertResourceBarrier(const TextureBarrierDesc &command)
    {
        TextureHandle textureHandle = command.Texture;
        TextureD3D12 *texture = textureHandle.AsDerived<TextureD3D12>();
        Microsoft::WRL::ComPtr<ID3D12Resource2> handle = texture->GetHandle();
        std::vector<D3D12_RESOURCE_BARRIER> barriers = {};

        for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
             arrayLayer < command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
             arrayLayer++)
        {
            for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
                 mipLevel < command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
                 mipLevel++)
            {
                D3D12_RESOURCE_STATES beforeState =
                    D3D12::GetTextureResourceState(texture->GetTextureLayout(arrayLayer, mipLevel));
                D3D12_RESOURCE_STATES afterState = D3D12::GetTextureResourceState(command.Layout);

                if (beforeState == afterState)
                {
                    continue;
                }

                uint32_t subresourceIndex =
                    Utils::CalculateSubresource(mipLevel, arrayLayer, texture->GetDescription().MipLevels);

                D3D12_RESOURCE_BARRIER &barrier = barriers.emplace_back();
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource = handle.Get();
                barrier.Transition.Subresource = subresourceIndex;
                barrier.Transition.StateBefore = beforeState;
                barrier.Transition.StateAfter = afterState;
            }
        }

        if (barriers.size() > 0)
        {
            m_CommandList->ResourceBarrier(barriers.size(), barriers.data());
        }

        for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
             arrayLayer < command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
             arrayLayer++)
        {
            for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
                 mipLevel < command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
                 mipLevel++)
            {
                texture->SetTextureLayout(arrayLayer, mipLevel, command.Layout);
            }
        }
    }

    void CommandExecutorD3D12::InsertTextureBarrier(const TextureBarrierDesc &command)
    {
        TextureHandle textureHandle = command.Texture;
        TextureD3D12 *texture = textureHandle.AsDerived<TextureD3D12>();
        Microsoft::WRL::ComPtr<ID3D12Resource2> handle = texture->GetHandle();

        bool transitionEachSubresourceSeparately = false;
        TextureLayout testLayout = texture->GetTextureLayout(0, 0);

        if (command.TextureSubresourceRange.LayerCount == texture->GetDescription().DepthOrArrayLayers &&
            command.TextureSubresourceRange.LevelCount == texture->GetDescription().MipLevels)
        {
            for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
                 arrayLayer <
                 command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
                 arrayLayer++)
            {
                for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
                     mipLevel <
                     command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
                     mipLevel++)
                {
                    TextureLayout subresourceLayout = texture->GetTextureLayout(arrayLayer, mipLevel);
                    if (subresourceLayout != testLayout)
                    {
                        transitionEachSubresourceSeparately = true;
                        break;
                    }
                }
            }
        }
        else
        {
            transitionEachSubresourceSeparately = true;
        }

        std::vector<D3D12_TEXTURE_BARRIER> barriers = {};

        if (!transitionEachSubresourceSeparately)
        {
            D3D12_BARRIER_LAYOUT beforeLayout = D3D12::GetBarrierLayout(testLayout);
            D3D12_BARRIER_LAYOUT afterLayout = D3D12::GetBarrierLayout(command.Layout);

            // we do not need to try to insert a barrier if the layout is already
            // correct
            if (beforeLayout == afterLayout)
            {
                return;
            }

            D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(command.BeforeStage);
            D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(command.AfterStage);
            D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(command.BeforeAccess);
            D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(command.AfterAccess);

            // if this is the first time using the resource, we need to hardcode the
            // before access value
            if (beforeLayout == D3D12_BARRIER_LAYOUT_UNDEFINED)
            {
                beforeAccess = D3D12_BARRIER_ACCESS_NO_ACCESS;
            }

            D3D12_TEXTURE_BARRIER &barrier = barriers.emplace_back();
            barrier.SyncBefore = beforeSync;
            barrier.SyncAfter = afterSync;
            barrier.AccessBefore = beforeAccess;
            barrier.AccessAfter = afterAccess;
            barrier.LayoutBefore = beforeLayout;
            barrier.LayoutAfter = afterLayout;
            barrier.pResource = handle.Get();
            barrier.Subresources.FirstArraySlice = command.TextureSubresourceRange.BaseArrayLayer;
            barrier.Subresources.NumArraySlices = command.TextureSubresourceRange.LayerCount;
            barrier.Subresources.IndexOrFirstMipLevel = command.TextureSubresourceRange.BaseMipLevel;
            barrier.Subresources.NumMipLevels = command.TextureSubresourceRange.LevelCount;
            barrier.Subresources.FirstPlane = 0;
            barrier.Subresources.NumPlanes = 1;
        }
        else
        {
            for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
                 arrayLayer <
                 command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
                 arrayLayer++)
            {
                for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
                     mipLevel <
                     command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
                     mipLevel++)
                {
                    D3D12_BARRIER_LAYOUT beforeLayout = D3D12::GetBarrierLayout(testLayout);
                    D3D12_BARRIER_LAYOUT afterLayout = D3D12::GetBarrierLayout(command.Layout);

                    // we do not need to try to insert a barrier if the layout is
                    // already correct
                    if (beforeLayout == afterLayout)
                    {
                        continue;
                    }

                    D3D12_BARRIER_SYNC beforeSync = D3D12::GetBarrierSync(command.BeforeStage);
                    D3D12_BARRIER_SYNC afterSync = D3D12::GetBarrierSync(command.AfterStage);
                    D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(command.BeforeAccess);
                    D3D12_BARRIER_ACCESS afterAccess = D3D12::GetBarrierAccess(command.AfterAccess);

                    D3D12_TEXTURE_BARRIER &barrier = barriers.emplace_back();
                    barrier.SyncBefore = beforeSync;
                    barrier.SyncAfter = afterSync;
                    barrier.AccessBefore = beforeAccess;
                    barrier.AccessAfter = afterAccess;
                    barrier.LayoutBefore = beforeLayout;
                    barrier.LayoutAfter = afterLayout;
                    barrier.pResource = handle.Get();
                    barrier.Subresources.FirstArraySlice = arrayLayer;
                    barrier.Subresources.NumArraySlices = 1;
                    barrier.Subresources.IndexOrFirstMipLevel = mipLevel;
                    barrier.Subresources.NumMipLevels = 1;
                    barrier.Subresources.FirstPlane = 0;
                    barrier.Subresources.NumPlanes = 1;
                }
            }
        }

        D3D12_BARRIER_GROUP barrierGroup = {};
        barrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
        barrierGroup.NumBarriers = barriers.size();
        barrierGroup.pTextureBarriers = barriers.data();

        m_CommandList->Barrier(1, &barrierGroup);

        for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
             arrayLayer < command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
             arrayLayer++)
        {
            for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
                 mipLevel < command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
                 mipLevel++)
            {
                texture->SetTextureLayout(arrayLayer, mipLevel, command.Layout);
            }
        }
    }

    Nexus::Graphics::DeviceBufferHandle CommandExecutorD3D12::CreateStagingBuffer(
        size_t size, bool upload, IGraphicsDevice *device
    )
    {
        DeviceBufferHandle &buffer = m_UploadBuffers.emplace_back();

        DeviceBufferDescription description = {};
        upload ? description.Access = BufferMemoryAccess::Upload : description.Access = BufferMemoryAccess::Readback;
        description.SizeInBytes = size;
        description.StrideInBytes = size;
        description.DebugName = "Staging Buffer";
        buffer = device->CreateDeviceBuffer(description);

        return buffer;
    }
} // namespace Nexus::Graphics

#endif