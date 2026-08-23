
#if defined(NX_PLATFORM_VULKAN)

#include "CommandExecutorVk.hpp"

#include "CommandListVk.hpp"
#include "DeviceBufferVk.hpp"
#include "FramebufferVk.hpp"
#include "PipelineVk.hpp"
#include "ResourceSetVk.hpp"
#include "TimingQueryVk.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

#include "Profiling/Profiler.hpp"

namespace Nexus::Graphics
{
    static void BindFramebuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);
        const auto &framebuffer = storage.CommandDatas.FramebufferCommands.at(header->CommandOffset);
        executor->BindFramebufferImpl(framebuffer);
    }

    static void ClearColourTarget(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.ClearColourTargetCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (!executor->ValidateForClearColour(executor->m_CurrentRenderTarget, cmd.Index) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        VkClearAttachment clearAttachment{};
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        clearAttachment.clearValue.color = {cmd.Colour.Red, cmd.Colour.Green, cmd.Colour.Blue, cmd.Colour.Alpha};
        clearAttachment.colorAttachment = cmd.Index;

        VkClearRect clearRect;
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;

        if (cmd.Rect.has_value())
        {
            Graphics::ClearRect rect = cmd.Rect.value();
            clearRect.rect.offset = {rect.X, rect.Y};
            clearRect.rect.extent = {rect.Width, rect.Height};
        }
        else
        {
            clearRect.rect.offset = {0, 0};
            clearRect.rect.extent = {executor->m_RenderSize};
        }

        if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
        {
            return;
        }

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdClearAttachments(executor->m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    static void ClearDepthStencil(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);
        const auto &cmd = storage.CommandDatas.ClearDepthStencilTargetCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (!executor->ValidateForClearDepth(executor->m_CurrentRenderTarget) || !executor->ValidateIsRendering())
        {
            return;
        }

        VkClearAttachment clearAttachment{};
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        clearAttachment.clearValue.depthStencil.depth = cmd.Value.Depth;
        clearAttachment.clearValue.depthStencil.stencil = cmd.Value.Stencil;
        clearAttachment.colorAttachment = executor->m_DepthAttachmentIndex;

        VkClearRect clearRect = {};
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;

        if (cmd.Rect.has_value())
        {
            Graphics::ClearRect rect = cmd.Rect.value();
            clearRect.rect.offset = {rect.X, rect.Y};
            clearRect.rect.extent = {rect.Width, rect.Height};
        }
        else
        {
            clearRect.rect.offset = {0, 0};
            clearRect.rect.extent = {executor->m_RenderSize};
        }

        if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
        {
            return;
        }

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdClearAttachments(executor->m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    static void SubmitBarrierGroup(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);
        const auto &cmd = storage.CommandDatas.BarrierGroupCommands.at(header->CommandOffset);

        if (executor->m_Rendering)
        {
            executor->StopRendering();
        }

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        // for now VK_DEPENDENCY_BY_REGION_BIT is hardcoded, however this may need to
        // be exposed in future
        VkDependencyFlagBits dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        std::unordered_map<VkImage, std::deque<Vk::SubresourceRangeLayout>> ranges;

        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (const TextureBarrierDesc &textureBarrier : cmd.TextureBarriers)
        {
            const TextureVk *textureVk = textureBarrier.Texture.AsDerived<const TextureVk>();

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
                    Vk::SubresourceRangeLayout &range = ranges[textureVk->GetImage()].emplace_back();
                    range.range.aspectMask = Vk::GetAspectFlags(textureVk->IsDepth());
                    range.range.baseArrayLayer = arrayLayer;
                    range.range.layerCount = 1;
                    range.range.baseMipLevel = mipLevel;
                    range.range.levelCount = 1;
                    range.layout =
                        Vk::GetImageLayout(executor->m_Device, textureVk->GetTextureLayout(arrayLayer, mipLevel));
                }
            }
        }

        uint32_t srcQueue = VK_QUEUE_FAMILY_IGNORED;
        uint32_t dstQueue = VK_QUEUE_FAMILY_IGNORED;

        // submit a PipelineBarrier2
        if (context.CmdPipelineBarrier2)
        {
            std::vector<VkMemoryBarrier2> memoryBarriersVk = {};
            std::vector<VkImageMemoryBarrier2> textureBarriersVk = {};
            std::vector<VkBufferMemoryBarrier2> bufferBarriersVk = {};

            memoryBarriersVk.reserve(cmd.MemoryBarriers.size());
            textureBarriersVk.reserve(cmd.TextureBarriers.size());
            bufferBarriersVk.reserve(cmd.BufferBarriers.size());

            for (const MemoryBarrierDesc &memoryBarrier : cmd.MemoryBarriers)
            {
                Vk::CreateMemoryBarrier2(executor->m_Device, memoryBarrier, memoryBarriersVk);
            }

            for (const TextureBarrierDesc &textureBarrier : cmd.TextureBarriers)
            {
                VkImageLayout layout = Vk::GetImageLayout(executor->m_Device, textureBarrier.Layout);
                const TextureVk *textureVk = textureBarrier.Texture.AsDerived<const TextureVk>();
                Vk::CreateTextureBarrier2(
                    executor->m_Device, textureVk->GetImage(), textureBarrier.BeforeAccess, textureBarrier.AfterAccess,
                    textureBarrier.BeforeStage, textureBarrier.AfterStage, srcQueue, dstQueue, layout,
                    textureBarriersVk, ranges
                );
            }

            for (const BufferBarrierDesc &bufferBarrier : cmd.BufferBarriers)
            {
                Vk::CreateBufferBarrier2(executor->m_Device, bufferBarrier, bufferBarriersVk, srcQueue, dstQueue);
            }

            VkDependencyInfo dependencyInfo = {};
            dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            dependencyInfo.pNext = nullptr;
            dependencyInfo.dependencyFlags = dependencyFlags;
            dependencyInfo.memoryBarrierCount = static_cast<uint32_t>(memoryBarriersVk.size());
            dependencyInfo.pMemoryBarriers = memoryBarriersVk.data();
            dependencyInfo.bufferMemoryBarrierCount = static_cast<uint32_t>(bufferBarriersVk.size());
            dependencyInfo.pBufferMemoryBarriers = bufferBarriersVk.data();
            dependencyInfo.imageMemoryBarrierCount = static_cast<uint32_t>(textureBarriersVk.size());
            dependencyInfo.pImageMemoryBarriers = textureBarriersVk.data();

            context.CmdPipelineBarrier2(executor->m_CommandBuffer, std::addressof(dependencyInfo));
        }
        // submit a legacy pipeline barrier
        else
        {
            std::vector<VkMemoryBarrier> memoryBarriersVk = {};
            std::vector<VkImageMemoryBarrier> textureBarriersVk = {};
            std::vector<VkBufferMemoryBarrier> bufferBarriersVk = {};

            memoryBarriersVk.reserve(cmd.MemoryBarriers.size());
            textureBarriersVk.reserve(cmd.TextureBarriers.size());
            bufferBarriersVk.reserve(cmd.BufferBarriers.size());

            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_NONE;
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_NONE;

            for (const MemoryBarrierDesc &memoryBarrier : cmd.MemoryBarriers)
            {
                Vk::CreateMemoryBarrier(executor->m_Device, memoryBarrier, memoryBarriersVk);
            }

            for (const TextureBarrierDesc &textureBarrier : cmd.TextureBarriers)
            {
                VkImageLayout layout = Vk::GetImageLayout(executor->m_Device, textureBarrier.Layout);
                const TextureVk *textureVk = textureBarrier.Texture.AsDerived<const TextureVk>();
                Vk::CreateTextureBarrier(
                    executor->m_Device, textureVk->GetImage(), textureBarrier.BeforeAccess, textureBarrier.AfterAccess,
                    textureBarrier.BeforeStage, textureBarrier.AfterStage, srcQueue, dstQueue, layout,
                    textureBarriersVk, ranges
                );

                srcStageMask |= Vk::GetPipelineStageFlags(executor->m_Device, textureBarrier.BeforeStage);
                dstStageMask |= Vk::GetPipelineStageFlags(executor->m_Device, textureBarrier.AfterStage);
            }

            for (const BufferBarrierDesc &bufferBarrier : cmd.BufferBarriers)
            {
                Vk::CreateBufferBarrier(executor->m_Device, bufferBarrier, bufferBarriersVk, srcQueue, dstQueue);
                srcStageMask |= Vk::GetPipelineStageFlags(executor->m_Device, bufferBarrier.BeforeStage);
                dstStageMask |= Vk::GetPipelineStageFlags(executor->m_Device, bufferBarrier.AfterStage);
            }

            context.CmdPipelineBarrier(
                executor->m_CommandBuffer, srcStageMask, dstStageMask, dependencyFlags,
                static_cast<uint32_t>(memoryBarriersVk.size()), memoryBarriersVk.data(),
                static_cast<uint32_t>(bufferBarriersVk.size()), bufferBarriersVk.data(),
                static_cast<uint32_t>(textureBarriersVk.size()), textureBarriersVk.data()
            );
        }

        // update texture layouts
        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (const TextureBarrierDesc &textureBarrier : cmd.TextureBarriers)
        {
            TextureHandle handle = textureBarrier.Texture;
            TextureVk *textureVk = handle.AsDerived<TextureVk>();

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
                    textureVk->SetTextureLayout(arrayLayer, mipLevel, textureBarrier.Layout);
                }
            }
        }
    }

    static void SetVertexBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.SetVertexBufferCommands.at(header->CommandOffset);

        const auto &bufferHandle = cmd.View.BufferHandle;

        executor->TryStartRendering();

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        const DeviceBufferVk *vertexBufferVk = bufferHandle.AsDerived<const DeviceBufferVk>();
        VkBuffer vertexBuffers[] = {vertexBufferVk->GetVkBuffer()};
        VkDeviceSize offsets[] = {cmd.View.Offset};
        VkDeviceSize sizes[] = {cmd.View.Size};

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        if (context.CmdBindVertexBuffers2)
        {
            context.CmdBindVertexBuffers2(
                executor->m_CommandBuffer, cmd.Slot, 1, vertexBuffers, offsets, sizes, nullptr
            );
        }
        else if (context.CmdBindVertexBuffers2EXT)
        {
            context.CmdBindVertexBuffers2EXT(
                executor->m_CommandBuffer, cmd.Slot, 1, vertexBuffers, offsets, sizes, nullptr
            );
        }
        else
        {
            context.CmdBindVertexBuffers(executor->m_CommandBuffer, cmd.Slot, 1, vertexBuffers, offsets);
        }
    }

    static void SetIndexBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.SetIndexBufferCommands.at(header->CommandOffset);
        const auto &bufferHandle = cmd.View.BufferHandle;

        executor->TryStartRendering();

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        const DeviceBufferVk *indexBufferVk = bufferHandle.AsDerived<const DeviceBufferVk>();
        VkBuffer indexBufferHandle = indexBufferVk->GetVkBuffer();
        VkIndexType indexType = Vk::GetVulkanIndexBufferFormat(cmd.View.BufferFormat);
        VkDeviceSize offset = cmd.View.Offset;
        VkDeviceSize size = cmd.View.Size;

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        if (context.CmdBindIndexBuffer2)
        {
            context.CmdBindIndexBuffer2(executor->m_CommandBuffer, indexBufferHandle, offset, size, indexType);
        }
        else if (context.CmdBindIndexBuffer2KHR)
        {
            context.CmdBindIndexBuffer2KHR(executor->m_CommandBuffer, indexBufferHandle, offset, size, indexType);
        }
        else
        {
            context.CmdBindIndexBuffer(executor->m_CommandBuffer, indexBufferHandle, offset, indexType);
        }
    }

    static void SetPipeline(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        auto &pipelineHandle = storage.CommandDatas.SetPipelineCommands.at(header->CommandOffset);
        executor->TryStartRendering();

        if (PipelineVk *pipeline = pipelineHandle.AsDerived<PipelineVk>())
        {
            executor->m_CurrentlyBoundPipeline = pipelineHandle;

            if (pipelineHandle->GetType() != PipelineType::Graphics &&
                pipelineHandle->GetType() != PipelineType::Meshlet)
            {
                pipeline->Bind(executor->m_CommandBuffer, VK_NULL_HANDLE);
            }
            else
            {
                // we immediately bind the graphics/meshlet pipeline if dynamic
                // rendering is available, otherwise we need to know which
                // VkRenderPass to use with it
                const VulkanDeviceFeatures &deviceFeatures = executor->m_Device->GetDeviceFeatures();
                if (deviceFeatures.DynamicRenderingAvailable)
                {
                    pipeline->Bind(executor->m_CommandBuffer, VK_NULL_HANDLE);
                }
            }
        }
    }

    static void SetResourceSet(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);
        auto &cmd = storage.CommandDatas.ResourceSetBindingCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (PipelineVk *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<PipelineVk>())
        {
            pipeline->SetResourceSet(executor->m_CommandBuffer, cmd);

            const ResourceSetVk *resourceSet = cmd.TargetResourceSet.AsDerived<const ResourceSetVk>();
            executor->m_CurrentlyBoundResourceSet = resourceSet;
        }
    }

    static void Draw(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.DrawCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        executor->BindGraphicsPipeline();

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdDraw(
            executor->m_CommandBuffer, cmd.VertexCount, cmd.InstanceCount, cmd.VertexStart, cmd.InstanceStart
        );
    }

    static void DrawIndexed(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.DrawIndexedCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        executor->BindGraphicsPipeline();

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdDrawIndexed(
            executor->m_CommandBuffer, cmd.IndexCount, cmd.InstanceCount, cmd.IndexStart, cmd.VertexStart,
            cmd.InstanceStart
        );
    }

    static void DrawIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.DrawIndirectCommands.at(header->CommandOffset);

        auto &deviceBuffer = cmd.IndirectBuffer;

        executor->TryStartRendering();

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        if (const DeviceBufferVk *indirectBuffer = deviceBuffer.AsDerived<const DeviceBufferVk>())
        {
            executor->BindGraphicsPipeline();

            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
            context.CmdDrawIndirect(
                executor->m_CommandBuffer, indirectBuffer->GetVkBuffer(), cmd.Offset, cmd.DrawCount, cmd.Stride
            );
        }
    }

    static void DrawIndexedIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.DrawIndirectIndexedCommands.at(header->CommandOffset);

        auto &deviceBuffer = cmd.IndirectBuffer;

        executor->TryStartRendering();

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        if (const DeviceBufferVk *indirectBuffer = deviceBuffer.AsDerived<const DeviceBufferVk>())
        {
            executor->BindGraphicsPipeline();

            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
            context.CmdDrawIndexedIndirect(
                executor->m_CommandBuffer, indirectBuffer->GetVkBuffer(), cmd.Offset, cmd.DrawCount, cmd.Stride
            );
        }
    }

    static void Dispatch(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.DispatchCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdDispatch(executor->m_CommandBuffer, cmd.WorkGroupCountX, cmd.WorkGroupCountY, cmd.WorkGroupCountZ);
    }

    static void DispatchIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.DispatchIndirectCommands.at(header->CommandOffset);

        auto &deviceBuffer = cmd.IndirectBuffer;

        executor->TryStartRendering();

        if (!executor->ValidateForComputeCall(executor->m_CurrentlyBoundPipeline))
        {
            return;
        }

        if (const DeviceBufferVk *indirectBuffer = deviceBuffer.AsDerived<const DeviceBufferVk>())
        {
            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
            context.CmdDispatchIndirect(executor->m_CommandBuffer, indirectBuffer->GetVkBuffer(), cmd.Offset);
        }
    }

    static void DrawMesh(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.DrawMeshCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        executor->BindGraphicsPipeline();

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        if (context.CmdDrawMeshTasksEXT)
        {
            context.CmdDrawMeshTasksEXT(
                executor->m_CommandBuffer, cmd.WorkGroupCountX, cmd.WorkGroupCountY, cmd.WorkGroupCountZ
            );
        }
    }

    static void DrawMeshIndirect(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.DrawMeshIndirectCommands.at(header->CommandOffset);

        auto &deviceBuffer = cmd.IndirectBuffer;

        executor->TryStartRendering();

        if (!executor->ValidateForGraphicsCall(executor->m_CurrentlyBoundPipeline, executor->m_CurrentRenderTarget) ||
            !executor->ValidateIsRendering())
        {
            return;
        }

        if (const DeviceBufferVk *indirectBuffer = deviceBuffer.AsDerived<const DeviceBufferVk>())
        {
            executor->BindGraphicsPipeline();

            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

            if (context.CmdDrawMeshTasksIndirectEXT)
            {
                context.CmdDrawMeshTasksIndirectEXT(
                    executor->m_CommandBuffer, indirectBuffer->GetVkBuffer(), cmd.Offset, cmd.DrawCount, cmd.Stride
                );
            }
        }
    }

    static void TraceRays(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.TraceRaysCommands.at(header->CommandOffset);

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        if (context.CmdTraceRaysKHR)
        {
            VkStridedDeviceAddressRegionKHR raygenRegion = {
                .deviceAddress = cmd.RaygenRegion.Address,
                .stride = cmd.RaygenRegion.Size,
                .size = cmd.RaygenRegion.Size
            };

            VkStridedDeviceAddressRegionKHR missRegion = {
                .deviceAddress = cmd.MissRegion.Address, .stride = cmd.MissRegion.Stride, .size = cmd.MissRegion.Size
            };

            VkStridedDeviceAddressRegionKHR hitRegion = {
                .deviceAddress = cmd.HitRegion.Address, .stride = cmd.HitRegion.Stride, .size = cmd.HitRegion.Size
            };

            VkStridedDeviceAddressRegionKHR callableRegion = {
                .deviceAddress = cmd.CallableRegion.Address,
                .stride = cmd.CallableRegion.Stride,
                .size = cmd.CallableRegion.Size
            };

            context.CmdTraceRaysKHR(
                executor->m_CommandBuffer, &raygenRegion, &missRegion, &hitRegion, &callableRegion, cmd.Width,
                cmd.Height, cmd.Depth
            );
        }
    }

    static void SetViewport(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);
        const auto &cmd = storage.CommandDatas.ViewportCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (!executor->ValidateForSetViewport(executor->m_CurrentRenderTarget, cmd))
        {
            return;
        }

        if (cmd.Width == 0 || cmd.Height == 0)
            return;

        VkViewport vp;
        vp.x = cmd.X;
        vp.y = cmd.Height + cmd.Y;
        vp.width = cmd.Width;
        vp.height = -cmd.Height;
        vp.minDepth = cmd.MinDepth;
        vp.maxDepth = cmd.MaxDepth;

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdSetViewport(executor->m_CommandBuffer, 0, 1, &vp);
    }

    static void SetScissor(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.ScissorCommands.at(header->CommandOffset);

        executor->TryStartRendering();

        if (!executor->ValidateForSetScissor(executor->m_CurrentRenderTarget, cmd))
        {
            return;
        }

        VkRect2D rect;
        rect.offset = {(int32_t)cmd.X, (int32_t)cmd.Y};
        rect.extent = {(uint32_t)cmd.Width, (uint32_t)cmd.Height};

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdSetScissor(executor->m_CommandBuffer, 0, 1, &rect);
    }

    static void PushConstants(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.PushConstantsCommands.at(header->CommandOffset);

        if (!executor->m_CurrentlyBoundResourceSet)
            return;

        std::optional<VkShaderStageFlags> stageFlags =
            executor->m_CurrentlyBoundResourceSet->GetPushConstantsStageFlags(cmd.Name);

        if (!stageFlags.has_value())
            return;

        if (PipelineVk *pipeline = executor->m_CurrentlyBoundPipeline.AsDerived<PipelineVk>())
        {
            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

            if (context.CmdPushConstants2)
            {
                VkPushConstantsInfo pushConstantsInfo = {};
                pushConstantsInfo.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
                pushConstantsInfo.pNext = nullptr;
                pushConstantsInfo.layout = pipeline->GetPipelineLayout();
                pushConstantsInfo.stageFlags = stageFlags.value();
                pushConstantsInfo.offset = cmd.Offset;
                pushConstantsInfo.size = cmd.Data.size();
                pushConstantsInfo.pValues = cmd.Data.data();

                context.CmdPushConstants2(executor->m_CommandBuffer, &pushConstantsInfo);
            }
            else if (context.CmdPushConstants2KHR)
            {
                VkPushConstantsInfoKHR pushConstantsInfo = {};
                pushConstantsInfo.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO_KHR;
                pushConstantsInfo.pNext = nullptr;
                pushConstantsInfo.layout = pipeline->GetPipelineLayout();
                pushConstantsInfo.stageFlags = stageFlags.value();
                pushConstantsInfo.offset = cmd.Offset;
                pushConstantsInfo.size = cmd.Data.size();
                pushConstantsInfo.pValues = cmd.Data.data();

                context.CmdPushConstants2KHR(executor->m_CommandBuffer, &pushConstantsInfo);
            }
            else
            {
                context.CmdPushConstants(
                    executor->m_CommandBuffer, pipeline->GetPipelineLayout(), stageFlags.value(), cmd.Offset,
                    cmd.Data.size(), cmd.Data.data()
                );
            }
        }
    }

    static void CopyBufferToBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.CopyBufferToBufferCommands.at(header->CommandOffset);

        const DeviceBufferVk *src = cmd.BufferCopy.Source.AsDerived<const DeviceBufferVk>();
        const DeviceBufferVk *dst = cmd.BufferCopy.Destination.AsDerived<const DeviceBufferVk>();

        if (!src || !dst)
        {
            return;
        }

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        if (context.CmdCopyBuffer2KHR)
        {
            std::vector<VkBufferCopy2KHR> bufferCopies;

            for (const auto &copy : cmd.BufferCopy.Copies)
            {
                VkBufferCopy2KHR &bufferCopy = bufferCopies.emplace_back();
                bufferCopy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR;
                bufferCopy.pNext = nullptr;
                bufferCopy.srcOffset = copy.ReadOffset;
                bufferCopy.dstOffset = copy.WriteOffset;
                bufferCopy.size = copy.Size;
            }

            VkCopyBufferInfo2KHR copyInfo = {};
            copyInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2_KHR;
            copyInfo.pNext = nullptr;
            copyInfo.srcBuffer = src->GetVkBuffer();
            copyInfo.dstBuffer = dst->GetVkBuffer();
            copyInfo.regionCount = bufferCopies.size();
            copyInfo.pRegions = bufferCopies.data();

            context.CmdCopyBuffer2KHR(executor->m_CommandBuffer, &copyInfo);
        }
        else
        {
            std::vector<VkBufferCopy> bufferCopies;

            for (const auto &copy : cmd.BufferCopy.Copies)
            {
                VkBufferCopy &bufferCopy = bufferCopies.emplace_back();
                bufferCopy.srcOffset = copy.ReadOffset;
                bufferCopy.dstOffset = copy.WriteOffset;
                bufferCopy.size = copy.Size;
            }

            context.CmdCopyBuffer(
                executor->m_CommandBuffer, src->GetVkBuffer(), dst->GetVkBuffer(), bufferCopies.size(),
                bufferCopies.data()
            );
        }
    }

    static void CopyBufferToTexture(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.CopyBufferToTextureCommands.at(header->CommandOffset);
        const DeviceBufferVk *buffer = cmd.BufferTextureCopy.BufferHandle.AsDerived<const DeviceBufferVk>();
        const TextureVk *texture = cmd.BufferTextureCopy.Texture.AsDerived<const TextureVk>();
        VkImageAspectFlagBits aspectFlags = Vk::GetAspectFlags(texture->IsDepth());

        if (!buffer)
        {
            return;
        }

        const uint32_t copyDepth = 1;

        std::map<uint32_t, VkImageLayout> previousLayouts;

        // perform copy
        {
            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

            VkImageSubresourceLayers imageSubresource = {};
            imageSubresource.aspectMask = aspectFlags;
            imageSubresource.mipLevel = cmd.BufferTextureCopy.MipLevel;
            imageSubresource.baseArrayLayer = 0;
            imageSubresource.layerCount = 1;

            if (texture->GetType() != TextureType::Texture3D)
            {
                imageSubresource.baseArrayLayer = cmd.BufferTextureCopy.TextureOffset.Z;
                imageSubresource.layerCount = copyDepth;
            }

            VkOffset3D imageOffset = {};
            imageOffset.x = cmd.BufferTextureCopy.TextureOffset.X;
            imageOffset.y = cmd.BufferTextureCopy.TextureOffset.Y;
            imageOffset.z = 0;

            if (texture->GetType() == TextureType::Texture3D)
            {
                imageOffset.z = cmd.BufferTextureCopy.TextureOffset.Z;
            }

            VkExtent3D imageExtent = {};
            imageExtent.width = cmd.BufferTextureCopy.TextureExtent.Width;
            imageExtent.height = cmd.BufferTextureCopy.TextureExtent.Height;
            imageExtent.depth = copyDepth;

            if (context.CmdCopyBufferToImage2KHR)
            {
                VkBufferImageCopy2KHR copyRegion = {};
                copyRegion.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR;
                copyRegion.pNext = nullptr;
                copyRegion.bufferOffset = cmd.BufferTextureCopy.BufferOffset;
                copyRegion.bufferRowLength = cmd.BufferTextureCopy.BufferRowLength;
                copyRegion.bufferImageHeight = cmd.BufferTextureCopy.BufferImageHeight;
                copyRegion.imageSubresource = imageSubresource;
                copyRegion.imageOffset = imageOffset;
                copyRegion.imageExtent = imageExtent;

                VkCopyBufferToImageInfo2KHR copyInfo = {};
                copyInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2_KHR;
                copyInfo.pNext = nullptr;
                copyInfo.srcBuffer = buffer->GetVkBuffer();
                copyInfo.dstImage = texture->GetImage();
                copyInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                copyInfo.pRegions = &copyRegion;
                copyInfo.regionCount = 1;

                context.CmdCopyBufferToImage2KHR(executor->m_CommandBuffer, &copyInfo);
            }
            else
            {
                VkBufferImageCopy copyRegion = {};
                copyRegion.bufferOffset = cmd.BufferTextureCopy.BufferOffset;
                copyRegion.bufferRowLength = cmd.BufferTextureCopy.BufferRowLength;
                copyRegion.bufferImageHeight = cmd.BufferTextureCopy.BufferImageHeight;
                copyRegion.imageSubresource = imageSubresource;
                copyRegion.imageOffset = imageOffset;
                copyRegion.imageExtent = imageExtent;

                context.CmdCopyBufferToImage(
                    executor->m_CommandBuffer, buffer->GetVkBuffer(), texture->GetImage(),
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion
                );
            }
        }
    }

    static void CopyTextureToBuffer(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.CopyTextureToBufferCommands.at(header->CommandOffset);
        const DeviceBufferVk *buffer = cmd.TextureBufferCopy.BufferHandle.AsDerived<const DeviceBufferVk>();

        if (!buffer)
        {
            return;
        }

        const TextureVk *texture = cmd.TextureBufferCopy.Texture.AsDerived<const TextureVk>();
        VkImageAspectFlagBits aspectFlags = Vk::GetAspectFlags(texture->IsDepth());

        std::map<uint32_t, VkImageLayout> previousLayouts;

        const bool copyDepth = 1;

        // perform copy
        {
            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

            VkImageSubresourceLayers imageSubresource = {};
            imageSubresource.aspectMask = aspectFlags;
            imageSubresource.mipLevel = cmd.TextureBufferCopy.MipLevel;
            imageSubresource.baseArrayLayer = 0;
            imageSubresource.layerCount = 1;

            if (texture->GetType() != TextureType::Texture3D)
            {
                imageSubresource.baseArrayLayer = cmd.TextureBufferCopy.TextureOffset.Z;
                imageSubresource.layerCount = copyDepth;
            }

            VkOffset3D imageOffset = {};
            imageOffset.x = cmd.TextureBufferCopy.TextureOffset.X;
            imageOffset.y = cmd.TextureBufferCopy.TextureOffset.Y;
            imageOffset.z = 0;

            if (texture->GetType() == TextureType::Texture3D)
            {
                imageOffset.z = cmd.TextureBufferCopy.TextureOffset.Z;
            }

            VkExtent3D imageExtent = {};
            imageExtent.width = cmd.TextureBufferCopy.TextureExtent.Width;
            imageExtent.height = cmd.TextureBufferCopy.TextureExtent.Height;
            imageExtent.depth = copyDepth;

            if (context.CmdCopyImageToBuffer2KHR)
            {
                VkBufferImageCopy2KHR copyRegion = {};
                copyRegion.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR;
                copyRegion.pNext = nullptr;
                copyRegion.bufferOffset = cmd.TextureBufferCopy.BufferOffset;
                copyRegion.bufferRowLength = cmd.TextureBufferCopy.BufferRowLength;
                copyRegion.bufferImageHeight = cmd.TextureBufferCopy.BufferImageHeight;
                copyRegion.imageSubresource = imageSubresource;
                copyRegion.imageOffset = imageOffset;
                copyRegion.imageExtent = imageExtent;

                VkCopyImageToBufferInfo2KHR copyInfo = {};
                copyInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2_KHR;
                copyInfo.pNext = nullptr;
                copyInfo.srcImage = texture->GetImage();
                copyInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                copyInfo.dstBuffer = buffer->GetVkBuffer();
                copyInfo.regionCount = 1;
                copyInfo.pRegions = &copyRegion;

                context.CmdCopyImageToBuffer2KHR(executor->m_CommandBuffer, &copyInfo);
            }
            else
            {
                VkBufferImageCopy copyRegion = {};
                copyRegion.bufferOffset = cmd.TextureBufferCopy.BufferOffset;
                copyRegion.bufferRowLength = cmd.TextureBufferCopy.BufferRowLength;
                copyRegion.bufferImageHeight = cmd.TextureBufferCopy.BufferImageHeight;
                copyRegion.imageSubresource = imageSubresource;
                copyRegion.imageOffset = imageOffset;
                copyRegion.imageExtent = imageExtent;

                context.CmdCopyImageToBuffer(
                    executor->m_CommandBuffer, texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    buffer->GetVkBuffer(), 1, &copyRegion
                );
            }
        }
    }

    static void CopyTextureToTexture(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.CopyTextureToTextureCommands.at(header->CommandOffset);
        const TextureVk *srcTexture = cmd.TextureCopy.Source.AsDerived<const TextureVk>();
        const TextureVk *dstTexture = cmd.TextureCopy.Destination.AsDerived<const TextureVk>();

        VkImageAspectFlagBits srcAspect = Vk::GetAspectFlags(srcTexture->IsDepth());
        VkImageAspectFlagBits dstAspect = Vk::GetAspectFlags(dstTexture->IsDepth());

        std::map<uint32_t, VkImageLayout> srcLayouts;
        std::map<uint32_t, VkImageLayout> dstLayouts;

        const bool copyDepth = 1;

        // copy image
        {
            VkImageSubresourceLayers srcSubresource = {};
            srcSubresource.aspectMask = srcAspect;
            srcSubresource.mipLevel = cmd.TextureCopy.SourceMipLevel;
            srcSubresource.baseArrayLayer = 0;
            srcSubresource.layerCount = 1;

            if (srcTexture->GetType() != TextureType::Texture3D)
            {
                srcSubresource.baseArrayLayer = cmd.TextureCopy.SourceOffset.Z;
                srcSubresource.layerCount = copyDepth;
            }

            VkOffset3D srcOffset = {};
            srcOffset.x = cmd.TextureCopy.SourceOffset.X;
            srcOffset.y = cmd.TextureCopy.SourceOffset.Y;
            srcOffset.z = 0;

            if (srcTexture->GetType() != TextureType::Texture2D)
            {
                srcOffset.z = cmd.TextureCopy.SourceOffset.Z;
            }

            VkImageSubresourceLayers dstSubresource = {};
            dstSubresource.aspectMask = dstAspect;
            dstSubresource.mipLevel = cmd.TextureCopy.DestinationMipLevel;
            dstSubresource.baseArrayLayer = 0;
            dstSubresource.layerCount = 1;

            // we can only set these parameters for array textures, i.e. not 3D
            // textures
            if (dstTexture->GetType() != TextureType::Texture3D)
            {
                dstSubresource.baseArrayLayer = cmd.TextureCopy.DestinationOffset.Z;
                dstSubresource.layerCount = copyDepth;
            }

            VkOffset3D dstOffset = {};
            dstOffset.x = cmd.TextureCopy.DestinationOffset.X;
            dstOffset.y = cmd.TextureCopy.DestinationOffset.Y;
            dstOffset.z = 0;

            if (dstTexture->GetType() != TextureType::Texture2D)
            {
                dstOffset.z = cmd.TextureCopy.DestinationOffset.Z;
            }

            VkExtent3D copyExtent = {};
            copyExtent.width = cmd.TextureCopy.Extent.Width;
            copyExtent.height = cmd.TextureCopy.Extent.Height;
            copyExtent.depth = copyDepth;

            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

            if (context.CmdCopyImage2KHR)
            {
                VkImageCopy2KHR copyRegion = {};
                copyRegion.sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2_KHR;
                copyRegion.pNext = nullptr;
                copyRegion.srcSubresource = srcSubresource;
                copyRegion.srcOffset = srcOffset;
                copyRegion.dstSubresource = dstSubresource;
                copyRegion.dstOffset = dstOffset;
                copyRegion.extent = copyExtent;

                VkCopyImageInfo2KHR copyInfo = {};
                copyInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2_KHR;
                copyInfo.pNext = nullptr;
                copyInfo.srcImage = srcTexture->GetImage();
                copyInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                copyInfo.dstImage = dstTexture->GetImage();
                copyInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                copyInfo.regionCount = 1;
                copyInfo.pRegions = &copyRegion;

                context.CmdCopyImage2KHR(executor->m_CommandBuffer, &copyInfo);
            }
            else
            {
                VkImageCopy copyRegion = {};

                // src
                copyRegion.srcSubresource = srcSubresource;
                copyRegion.srcOffset = srcOffset;

                // dst
                copyRegion.dstSubresource = dstSubresource;
                copyRegion.dstOffset = dstOffset;

                // copy extents
                copyRegion.extent = copyExtent;

                context.CmdCopyImage(
                    executor->m_CommandBuffer, srcTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    dstTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion
                );
            }
        }
    }

    static void Resolve(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.ResolveCommands.at(header->CommandOffset);

        if (!executor->ValidateForResolve(cmd))
        {
            return;
        }

        executor->StopRendering();

        auto source = cmd.Source.AsDerived<const TextureVk>();
        auto destination = cmd.Destination.AsDerived<const TextureVk>();

        Point2D<uint32_t> size = Utils::GetMipSize(cmd.Source->GetWidth(), cmd.Source->GetHeight(), cmd.SourceMipLevel);

        VkImageSubresourceLayers srcSubresource = {};
        srcSubresource.aspectMask =
            source->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        srcSubresource.mipLevel = cmd.SourceMipLevel;
        srcSubresource.baseArrayLayer = 0;
        srcSubresource.layerCount = 1;

        if (source->GetType() != TextureType::Texture3D)
        {
            srcSubresource.baseArrayLayer = cmd.SourceArrayLayer;
        }

        VkOffset3D srcOffset = {};
        srcOffset.x = 0;
        srcOffset.y = 0;
        srcOffset.z = 0;

        if (source->GetType() == TextureType::Texture3D)
        {
            srcOffset.z = cmd.SourceArrayLayer;
        }

        VkImageSubresourceLayers dstSubresource = {};
        dstSubresource.aspectMask = destination->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
                                                           : VK_IMAGE_ASPECT_COLOR_BIT;
        dstSubresource.mipLevel = cmd.DestinationMipLevel;
        dstSubresource.baseArrayLayer = 0;
        dstSubresource.layerCount = 1;

        if (destination->GetType() != TextureType::Texture3D)
        {
            dstSubresource.baseArrayLayer = cmd.DestinationArrayLayer;
        }

        VkOffset3D dstOffset = {};
        dstOffset.x = 0;
        dstOffset.y = 0;
        dstOffset.z = 0;

        if (destination->GetType() == TextureType::Texture3D)
        {
            dstOffset.z = cmd.DestinationArrayLayer;
        }

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        if (context.CmdResolveImage2KHR)
        {
            VkImageResolve2KHR resolve = {};
            resolve.sType = VK_STRUCTURE_TYPE_IMAGE_RESOLVE_2_KHR;
            resolve.pNext = nullptr;
            resolve.srcOffset = srcOffset;
            resolve.dstOffset = dstOffset;
            resolve.extent = {size.X, size.Y, 1};
            resolve.srcSubresource = srcSubresource;
            resolve.dstSubresource = dstSubresource;

            VkResolveImageInfo2KHR resolveInfo = {};
            resolveInfo.sType = VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2_KHR;
            resolveInfo.pNext = nullptr;
            resolveInfo.srcImage = source->GetImage();
            resolveInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            resolveInfo.dstImage = destination->GetImage();
            resolveInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            resolveInfo.regionCount = 1;
            resolveInfo.pRegions = &resolve;

            context.CmdResolveImage2KHR(executor->m_CommandBuffer, &resolveInfo);
        }
        else
        {
            VkImageResolve resolve = {};
            resolve.srcOffset = srcOffset;
            resolve.dstOffset = dstOffset;
            resolve.extent = {size.X, size.Y, 1};
            resolve.srcSubresource = srcSubresource;
            resolve.dstSubresource = dstSubresource;

            context.CmdResolveImage(
                executor->m_CommandBuffer, source->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                destination->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &resolve
            );
        }

        executor->BindFramebufferImpl(executor->m_CurrentRenderTarget);
    }

    static void StartTimingQuery(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.StartTimingQueryCommands.at(header->CommandOffset);

        TimingQueryHandle queryHandle = cmd.Query;
        if (TimingQueryVk *queryVk = queryHandle.AsDerived<TimingQueryVk>())
        {
            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
            context.CmdResetQueryPool(executor->m_CommandBuffer, queryVk->GetQueryPool(), 0, 2);
            context.CmdWriteTimestamp(
                executor->m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0
            );
        }
    }

    static void StopTimingQuery(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.StopTimingQueryCommands.at(header->CommandOffset);

        TimingQueryHandle queryHandle = cmd.Query;
        if (TimingQueryVk *queryVk = queryHandle.AsDerived<TimingQueryVk>())
        {
            const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
            context.CmdWriteTimestamp(
                executor->m_CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryVk->GetQueryPool(), 1
            );
        }
    }

    static void BeginDebugGroup(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.BeginDebugGroupCommands.at(header->CommandOffset);

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        if (context.CmdBeginDebugUtilsLabelEXT)
        {
            VkDebugUtilsLabelEXT labelEXT = {};
            labelEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            labelEXT.pNext = nullptr;
            labelEXT.pLabelName = cmd.GroupName.c_str();
            labelEXT.color[0] = cmd.Colour.r;
            labelEXT.color[1] = cmd.Colour.g;
            labelEXT.color[2] = cmd.Colour.b;
            labelEXT.color[3] = cmd.Colour.a;
            context.CmdBeginDebugUtilsLabelEXT(executor->m_CommandBuffer, &labelEXT);
        }
        else if (context.CmdDebugMarkerBeginEXT)
        {
            VkDebugMarkerMarkerInfoEXT markerInfo = {};
            markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
            markerInfo.pNext = nullptr;
            markerInfo.pMarkerName = cmd.GroupName.c_str();
            markerInfo.color[0] = cmd.Colour.r;
            markerInfo.color[1] = cmd.Colour.g;
            markerInfo.color[2] = cmd.Colour.b;
            markerInfo.color[3] = cmd.Colour.a;
            context.CmdDebugMarkerBeginEXT(executor->m_CommandBuffer, &markerInfo);
        }
    }

    static void EndDebugGroup(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.EndDebugGroupCommands.at(header->CommandOffset);

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        // if this is the last command in the buffer, then we must explicitly stop
        // rendering to ensure that the implict render pass management occurs in the
        // correct order
        if (executor->m_LastCommand)
        {
            executor->StopRendering();
        }
        // otherwise, if the next command is to set a new render target, we need to
        // stop rendering to ensure that they show in the correct order in debuggers
        else
        {
            if (executor->m_NextCommandType.has_value() &&
                executor->m_NextCommandType.value() == CommandType::SetFramebuffer)
            {
                executor->StopRendering();
            }
        }

        if (context.CmdEndDebugUtilsLabelEXT)
        {
            context.CmdEndDebugUtilsLabelEXT(executor->m_CommandBuffer);
        }
        else if (context.CmdDebugMarkerEndEXT)
        {
            context.CmdDebugMarkerEndEXT(executor->m_CommandBuffer);
        }
    }

    static void DebugLabel(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.InsertDebugMarkerCommands.at(header->CommandOffset);

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();

        if (context.CmdInsertDebugUtilsLabelEXT)
        {
            VkDebugUtilsLabelEXT labelEXT = {};
            labelEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            labelEXT.pNext = nullptr;
            labelEXT.pLabelName = cmd.MarkerName.c_str();
            labelEXT.color[0] = cmd.Colour.r;
            labelEXT.color[1] = cmd.Colour.g;
            labelEXT.color[2] = cmd.Colour.b;
            labelEXT.color[3] = cmd.Colour.a;
            context.CmdInsertDebugUtilsLabelEXT(executor->m_CommandBuffer, &labelEXT);
        }
        else if (context.CmdDebugMarkerInsertEXT)
        {
            VkDebugMarkerMarkerInfoEXT markerInfo = {};
            markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
            markerInfo.pNext = nullptr;
            markerInfo.pMarkerName = cmd.MarkerName.c_str();
            markerInfo.color[0] = cmd.Colour.r;
            markerInfo.color[1] = cmd.Colour.g;
            markerInfo.color[2] = cmd.Colour.b;
            markerInfo.color[3] = cmd.Colour.a;
            context.CmdDebugMarkerInsertEXT(executor->m_CommandBuffer, &markerInfo);
        }
    }

    static void SetBlendFactor(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.SetBlendFactorCommands.at(header->CommandOffset);

        float blendConstants[4] = {
            cmd.BlendFactor.Red, cmd.BlendFactor.Green, cmd.BlendFactor.Blue, cmd.BlendFactor.Alpha
        };

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdSetBlendConstants(executor->m_CommandBuffer, blendConstants);
    }

    static void SetStencilReference(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.SetStencilReferenceCommands.at(header->CommandOffset);

        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        context.CmdSetStencilReference(executor->m_CommandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, cmd.StencilReference);
    }

    static void BuildAccelerationStructures(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.BuildAccelerationStructuresCommands.at(header->CommandOffset);

        // return early if the function is not available to use
        const GladVulkanContext &context = executor->m_Device->GetVulkanContext();
        if (!context.CmdBuildAccelerationStructuresKHR)
        {
            return;
        }

        // create storage for the data
        std::vector<std::vector<VkAccelerationStructureGeometryKHR>> accelerationStructureGeometries = {};
        std::vector<VkAccelerationStructureBuildGeometryInfoKHR> buildGeometries = {};
        std::vector<std::vector<VkAccelerationStructureBuildRangeInfoKHR>> buildRanges = {};

        // loop through all requested builds
        for (const AccelerationStructureGeometryBuildDescription &buildGeometryInfo : cmd.BuildDescriptions)
        {
            // validate that required members have been filled in correctly
            NX_VALIDATE(
                buildGeometryInfo.Destination.IsValid(), "Acceleration structure build must have a destination"
            );
            NX_VALIDATE(buildGeometryInfo.ScratchBuffer, "Acceleration structure build must have a scratch buffer");

            if (buildGeometryInfo.Mode == AccelerationStructureBuildMode::Update)
            {
                NX_VALIDATE(buildGeometryInfo.Source.IsValid(), "Acceleration structure update must have a source");
            }

            // create a new vector to hold the information for the individual build
            std::vector<uint32_t> primitiveCounts;
            std::vector<VkAccelerationStructureGeometryKHR> &accelerationStructureGeometry =
                accelerationStructureGeometries.emplace_back();
            accelerationStructureGeometry =
                Vk::GetVulkanAccelerationStructureGeometries(buildGeometryInfo, primitiveCounts);

            // create the new build description
            buildGeometries.push_back(Vk::GetGeometryBuildInfo(buildGeometryInfo, accelerationStructureGeometry));

            // create a new vector to hold the build range
            std::vector<VkAccelerationStructureBuildRangeInfoKHR> &geometryBuildRange = buildRanges.emplace_back();

            // iterate through each build range and convert them to Vulkan types
            for (uint32_t primitiveCount : primitiveCounts)
            {
                geometryBuildRange.push_back(Vk::GetAccelerationStructureBuildRange(primitiveCount));
            }
        }

        std::vector<const VkAccelerationStructureBuildRangeInfoKHR *> buildRangePtrs;
        buildRangePtrs.reserve(buildRanges.size());
        for (const auto &range : buildRanges)
        {
            buildRangePtrs.push_back(range.data());
        }

        // execute the acceleration structure build
        context.CmdBuildAccelerationStructuresKHR(
            executor->m_CommandBuffer, buildGeometries.size(), buildGeometries.data(), buildRangePtrs.data()
        );
    }

    static void CopyAccelerationStructures(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.CopyAccelerationStructuresCommands.at(header->CommandOffset);
    }

    static void CopyAccelerationStructureToDeviceBuffer(
        const CommandHeader *header, CommandListStorage &storage, void *data
    )
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.CopyAccelerationStructureDeviceBufferCommands.at(header->CommandOffset);
    }

    static void CopyDeviceBufferToAccelerationStructure(
        const CommandHeader *header, CommandListStorage &storage, void *data
    )
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.CopyDeviceBufferAccelerationStructureCommands.at(header->CommandOffset);
    }

    static void EndRendering(const CommandHeader *header, CommandListStorage &storage, void *data)
    {
        CommandExecutorVk *executor = reinterpret_cast<CommandExecutorVk *>(data);

        const auto &cmd = storage.CommandDatas.EndRenderingCommands.at(header->CommandOffset);
    }

    static void BeginRenderPass(
        GraphicsDeviceVk *device, const VkRenderPassBeginInfo &beginInfo, VkSubpassContents subpassContents,
        VkCommandBuffer commandBuffer
    )
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = device->GetVulkanContext();

        if (context.CmdBeginRenderPass2KHR)
        {
            VkSubpassBeginInfo subpassInfo = {};
            subpassInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
            subpassInfo.pNext = nullptr;
            subpassInfo.contents = subpassContents;

            context.CmdBeginRenderPass2KHR(commandBuffer, &beginInfo, &subpassInfo);
        }
        else
        {
            context.CmdBeginRenderPass(commandBuffer, &beginInfo, subpassContents);
        }
    }

    static void BeginDynamicRenderingToFramebuffer(
        GraphicsDeviceVk *device, FramebufferHandle framebuffer, VkCommandBuffer commandBuffer
    )
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = device->GetVulkanContext();

        VkRect2D renderArea{};
        renderArea.offset = {0, 0};
        renderArea.extent = {framebuffer->GetWidth(), framebuffer->GetHeight()};

        std::vector<VkRenderingAttachmentInfo> colourAttachments;

        // attach colour textures
        for (uint32_t colourAttachmentIndex = 0; colourAttachmentIndex < framebuffer->GetColorTextureCount();
             colourAttachmentIndex++)
        {
            FramebufferColourAttachmentDescription textureBinding =
                framebuffer->GetColorTextureBinding(colourAttachmentIndex).value();

            const TextureVk *texture = textureBinding.ColourAttachment.TargetTexture.AsDerived<const TextureVk>();
            TextureLayout layout = texture->GetTextureLayout(
                textureBinding.ColourAttachment.BaseArrayLayer, textureBinding.ColourAttachment.MipLevel
            );

            FramebufferColourAttachmentDescription colourAttachmentDesc =
                framebuffer->GetColorTextureBinding(colourAttachmentIndex).value();

            VulkanTextureViewInfo viewInfo = {};
            viewInfo.BaseMipLevel = colourAttachmentDesc.ColourAttachment.MipLevel;
            viewInfo.LevelCount = 1;
            viewInfo.BaseArrayLayer = colourAttachmentDesc.ColourAttachment.BaseArrayLayer;
            viewInfo.LayerCount = colourAttachmentDesc.ColourAttachment.LayerCount;

            VkRenderingAttachmentInfo colourAttachment = {};
            colourAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            colourAttachment.imageView = texture->GetImageView(viewInfo);
            colourAttachment.imageLayout = Vk::GetImageLayout(device, layout);
            colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colourAttachment.clearValue = {};

            // handle the case of submitting a swapchain with multisampling
            if (colourAttachmentDesc.ResolveAttachment.has_value())
            {
                FramebufferTextureDescription resolveDesc = colourAttachmentDesc.ResolveAttachment.value();
                const TextureVk *resolveAttachment = resolveDesc.TargetTexture.AsDerived<const TextureVk>();
                TextureLayout resolveLayout =
                    resolveAttachment->GetTextureLayout(resolveDesc.BaseArrayLayer, resolveDesc.MipLevel);

                VulkanTextureViewInfo viewInfo = {};
                viewInfo.BaseMipLevel = resolveDesc.MipLevel;
                viewInfo.LevelCount = 1;
                viewInfo.BaseArrayLayer = resolveDesc.BaseArrayLayer;
                viewInfo.LayerCount = resolveDesc.LayerCount;

                colourAttachment.resolveImageView = resolveAttachment->GetImageView(viewInfo);
                colourAttachment.resolveImageLayout = Vk::GetImageLayout(device, resolveLayout);
                colourAttachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
            }

            colourAttachments.push_back(colourAttachment);
        }

        // set up depth attachment (may be unused)
        VkRenderingAttachmentInfo depthAttachment = {};
        if (framebuffer->HasDepthTexture())
        {
            FramebufferTextureDescription textureBinding = framebuffer->GetDepthTextureBinding().value();
            FramebufferVk *framebufferVk = framebuffer.AsDerived<FramebufferVk>();

            const TextureVk *texture = framebufferVk->GetVulkanDepthTexture();
            TextureLayout layout = texture->GetTextureLayout(textureBinding.BaseArrayLayer, textureBinding.MipLevel);

            FramebufferTextureDescription depthAttachmentDesc = framebuffer->GetDepthTextureBinding().value();

            VulkanTextureViewInfo viewInfo = {};
            viewInfo.BaseMipLevel = depthAttachmentDesc.MipLevel;
            viewInfo.LevelCount = 1;
            viewInfo.BaseArrayLayer = depthAttachmentDesc.BaseArrayLayer;
            viewInfo.LayerCount = depthAttachmentDesc.LayerCount;

            depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            depthAttachment.imageView = texture->GetImageView(viewInfo);
            depthAttachment.imageLayout = Vk::GetImageLayout(device, layout);
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.clearValue = {};
        }

        VkRenderingInfo renderingInfo = {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = renderArea;
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = colourAttachments.size();
        renderingInfo.pColorAttachments = colourAttachments.data();

        if (framebuffer->HasDepthTexture())
        {
            renderingInfo.pDepthAttachment = &depthAttachment;
            renderingInfo.pStencilAttachment = &depthAttachment;
        }
        else
        {
            renderingInfo.pDepthAttachment = nullptr;
            renderingInfo.pStencilAttachment = nullptr;
        }

        context.CmdBeginRenderingKHR(commandBuffer, &renderingInfo);
    }

    static void BeginRenderPassToFramebuffer(
        GraphicsDeviceVk *device, FramebufferHandle framebuffer, VkCommandBuffer commandBuffer
    )
    {
        NX_PROFILE_FUNCTION();

        const FramebufferVk *framebufferVk = framebuffer.AsDerived<const FramebufferVk>();

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.renderPass = framebufferVk->GetRenderPass();
        beginInfo.framebuffer = framebufferVk->GetFramebuffer();
        beginInfo.renderArea.offset = {0, 0};
        beginInfo.renderArea.extent = {framebuffer->GetWidth(), framebuffer->GetHeight()};
        beginInfo.clearValueCount = 0;
        beginInfo.pClearValues = nullptr;

        VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE;

        BeginRenderPass(device, beginInfo, subpassContents, commandBuffer);
    }

    void CommandExecutorVk::BindFramebufferImpl(FramebufferHandle framebuffer)
    {
        StopRendering();

        if (const FramebufferVk *framebufferVk = framebuffer.AsDerived<const FramebufferVk>())
        {
            StartRenderingToFramebuffer(framebuffer);
            m_CurrentRenderTarget = framebuffer;
            m_RenderSize = {framebuffer->GetWidth(), framebuffer->GetHeight()};
        }
    }

    void CommandExecutorVk::StartRenderingToFramebuffer(FramebufferHandle framebuffer)
    {
        NX_PROFILE_FUNCTION();

        const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
        if (features.DynamicRenderingAvailable)
        {
            BeginDynamicRenderingToFramebuffer(m_Device, framebuffer, m_CommandBuffer);
        }
        else
        {
            BeginRenderPassToFramebuffer(m_Device, framebuffer, m_CommandBuffer);
        }

        m_Rendering = true;
    }

    CommandExecutorVk::CommandExecutorVk(GraphicsDeviceVk *device) : m_Device(device)
    {
        m_DispatchTable[ToIndex(CommandType::SetFramebuffer)] = BindFramebuffer;
        m_DispatchTable[ToIndex(CommandType::ClearColourTarget)] = ClearColourTarget;
        m_DispatchTable[ToIndex(CommandType::ClearDepthTarget)] = ClearDepthStencil;
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

    CommandExecutorVk::~CommandExecutorVk()
    {
    }

    void CommandExecutorVk::ExecuteCommands(ICommandList *commandList, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        CommandListVk *commandListVk = (CommandListVk *)commandList;
        m_CommandBuffer = commandListVk->GetCurrentCommandBuffer();

        // begin
        {
            // reset command buffer
            {
                context.ResetCommandBuffer(m_CommandBuffer, 0);
            }

            // begin command buffer
            {
                VkCommandBufferBeginInfo beginInfo = {};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
                if (context.BeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to begin command buffer");
                }
            }

            m_CurrentRenderTarget = {};
            m_Rendering = false;
        }

        // execute commands
        {
            auto &storage = commandList->GetStorage();

            for (size_t commandIndex = 0; commandIndex < storage.CommandDatas.Headers.size(); commandIndex++)
            {
                m_NextCommandType =
                    storage.CommandDatas.Headers.size() > 0 && storage.CommandDatas.Headers.size() + 1 > commandIndex
                        ? std::optional<CommandType>{}
                        : storage.CommandDatas.Headers[commandIndex + 1].Type;

                m_LastCommand = commandIndex >= storage.CommandDatas.Headers.size();

                const auto &header = storage.CommandDatas.Headers.at(commandIndex);

                if (auto func = m_DispatchTable[ToIndex(header.Type)])
                {
                    func(&header, storage, this);
                }
            }

            m_Commands = {};
        }

        // end
        {
            StopRendering();
            context.EndCommandBuffer(m_CommandBuffer);
        }
    }

    void CommandExecutorVk::Reset()
    {
        NX_PROFILE_FUNCTION();
    }

    void CommandExecutorVk::ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        const DeviceBufferVk *vertexBufferVk = command.View.BufferHandle.AsDerived<const DeviceBufferVk>();
        VkBuffer vertexBuffers[] = {vertexBufferVk->GetVkBuffer()};
        VkDeviceSize offsets[] = {command.View.Offset};
        VkDeviceSize sizes[] = {command.View.Size};

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (context.CmdBindVertexBuffers2)
        {
            context.CmdBindVertexBuffers2(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets, sizes, nullptr);
        }
        else if (context.CmdBindVertexBuffers2EXT)
        {
            context.CmdBindVertexBuffers2EXT(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets, sizes, nullptr);
        }
        else
        {
            context.CmdBindVertexBuffers(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets);
        }
    }

    void CommandExecutorVk::ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        const DeviceBufferVk *indexBufferVk = command.View.BufferHandle.AsDerived<const DeviceBufferVk>();
        VkBuffer indexBufferHandle = indexBufferVk->GetVkBuffer();
        VkIndexType indexType = Vk::GetVulkanIndexBufferFormat(command.View.BufferFormat);
        VkDeviceSize offset = command.View.Offset;
        VkDeviceSize size = command.View.Size;

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (context.CmdBindIndexBuffer2)
        {
            context.CmdBindIndexBuffer2(m_CommandBuffer, indexBufferHandle, offset, size, indexType);
        }
        else if (context.CmdBindIndexBuffer2KHR)
        {
            context.CmdBindIndexBuffer2KHR(m_CommandBuffer, indexBufferHandle, offset, size, indexType);
        }
        else
        {
            context.CmdBindIndexBuffer(m_CommandBuffer, indexBufferHandle, offset, indexType);
        }
    }

    void CommandExecutorVk::ExecuteCommand(PipelineHandle command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!command.IsValid())
        {
            NX_ERROR("Attempting to bind an invalid pipeline");
            return;
        }

        if (PipelineVk *pipeline = command.AsDerived<PipelineVk>())
        {
            m_CurrentlyBoundPipeline = command;

            if (command->GetType() != PipelineType::Graphics && command->GetType() != PipelineType::Meshlet)
            {
                pipeline->Bind(m_CommandBuffer, VK_NULL_HANDLE);
            }
            else
            {
                // we immediately bind the graphics/meshlet pipeline if dynamic
                // rendering is available, otherwise we need to know which
                // VkRenderPass to use with it
                GraphicsDeviceVk *deviceVk = (GraphicsDeviceVk *)device;
                const VulkanDeviceFeatures &deviceFeatures = deviceVk->GetDeviceFeatures();
                if (deviceFeatures.DynamicRenderingAvailable)
                {
                    pipeline->Bind(m_CommandBuffer, VK_NULL_HANDLE);
                }
            }
        }
    }

    void CommandExecutorVk::ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        BindGraphicsPipeline();

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdDraw(
            m_CommandBuffer, command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart
        );
    }

    void CommandExecutorVk::ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        BindGraphicsPipeline();

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdDrawIndexed(
            m_CommandBuffer, command.IndexCount, command.InstanceCount, command.IndexStart, command.VertexStart,
            command.InstanceStart
        );
    }

    void CommandExecutorVk::ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        if (const DeviceBufferVk *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferVk>())
        {
            BindGraphicsPipeline();

            const GladVulkanContext &context = m_Device->GetVulkanContext();
            context.CmdDrawIndirect(
                m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride
            );
        }
    }

    void CommandExecutorVk::ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        if (const DeviceBufferVk *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferVk>())
        {
            BindGraphicsPipeline();

            const GladVulkanContext &context = m_Device->GetVulkanContext();
            context.CmdDrawIndexedIndirect(
                m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride
            );
        }
    }

    void CommandExecutorVk::ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdDispatch(m_CommandBuffer, command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
    }

    void CommandExecutorVk::ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
        {
            return;
        }

        if (const DeviceBufferVk *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferVk>())
        {
            const GladVulkanContext &context = m_Device->GetVulkanContext();
            context.CmdDispatchIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset);
        }
    }

    void CommandExecutorVk::ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        BindGraphicsPipeline();

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (context.CmdDrawMeshTasksEXT)
        {
            context.CmdDrawMeshTasksEXT(
                m_CommandBuffer, command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ
            );
        }
    }

    void CommandExecutorVk::ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        if (const DeviceBufferVk *indirectBuffer = command.IndirectBuffer.AsDerived<const DeviceBufferVk>())
        {
            BindGraphicsPipeline();

            const GladVulkanContext &context = m_Device->GetVulkanContext();

            if (context.CmdDrawMeshTasksIndirectEXT)
            {
                context.CmdDrawMeshTasksIndirectEXT(
                    m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride
                );
            }
        }
    }

    void CommandExecutorVk::ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (PipelineVk *pipeline = m_CurrentlyBoundPipeline.AsDerived<PipelineVk>())
        {
            pipeline->SetResourceSet(m_CommandBuffer, desc);

            const ResourceSetVk *resourceSet = desc.TargetResourceSet.AsDerived<const ResourceSetVk>();
            m_CurrentlyBoundResourceSet = resourceSet;
        }
    }

    void CommandExecutorVk::ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForClearColour(m_CurrentRenderTarget, command.Index) || !ValidateIsRendering())
        {
            return;
        }

        VkClearAttachment clearAttachment{};
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        clearAttachment.clearValue.color = {
            command.Colour.Red, command.Colour.Green, command.Colour.Blue, command.Colour.Alpha
        };
        clearAttachment.colorAttachment = command.Index;

        VkClearRect clearRect;
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;

        if (command.Rect.has_value())
        {
            Graphics::ClearRect rect = command.Rect.value();
            clearRect.rect.offset = {rect.X, rect.Y};
            clearRect.rect.extent = {rect.Width, rect.Height};
        }
        else
        {
            clearRect.rect.offset = {0, 0};
            clearRect.rect.extent = {m_RenderSize};
        }

        if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
        {
            return;
        }

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandExecutorVk::ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForClearDepth(m_CurrentRenderTarget) || !ValidateIsRendering())
        {
            return;
        }

        VkClearAttachment clearAttachment{};
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        clearAttachment.clearValue.depthStencil.depth = command.Value.Depth;
        clearAttachment.clearValue.depthStencil.stencil = command.Value.Stencil;
        clearAttachment.colorAttachment = m_DepthAttachmentIndex;

        VkClearRect clearRect = {};
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;

        if (command.Rect.has_value())
        {
            Graphics::ClearRect rect = command.Rect.value();
            clearRect.rect.offset = {rect.X, rect.Y};
            clearRect.rect.extent = {rect.Width, rect.Height};
        }
        else
        {
            clearRect.rect.offset = {0, 0};
            clearRect.rect.extent = {m_RenderSize};
        }

        if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
        {
            return;
        }

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandExecutorVk::ExecuteCommand(FramebufferHandle command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        StopRendering();

        if (const FramebufferVk *framebuffer = command.AsDerived<const FramebufferVk>())
        {
            StartRenderingToFramebuffer(command);
            m_CurrentRenderTarget = command;
            m_RenderSize = {framebuffer->GetWidth(), framebuffer->GetHeight()};
        }
    }

    void CommandExecutorVk::ExecuteCommand(const Viewport &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForSetViewport(m_CurrentRenderTarget, command))
        {
            return;
        }

        if (command.Width == 0 || command.Height == 0)
            return;

        VkViewport vp;
        vp.x = command.X;
        vp.y = command.Height + command.Y;
        vp.width = command.Width;
        vp.height = -command.Height;
        vp.minDepth = command.MinDepth;
        vp.maxDepth = command.MaxDepth;

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdSetViewport(m_CommandBuffer, 0, 1, &vp);
    }

    void CommandExecutorVk::ExecuteCommand(const Scissor &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        TryStartRendering();

        if (!ValidateForSetScissor(m_CurrentRenderTarget, command))
        {
            return;
        }

        VkRect2D rect;
        rect.offset = {(int32_t)command.X, (int32_t)command.Y};
        rect.extent = {(uint32_t)command.Width, (uint32_t)command.Height};

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdSetScissor(m_CommandBuffer, 0, 1, &rect);
    }

    void CommandExecutorVk::ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
        if (!ValidateForResolve(command))
        {
            return;
        }

        StopRendering();

        auto source = command.Source.AsDerived<const TextureVk>();
        auto destination = command.Destination.AsDerived<const TextureVk>();

        Point2D<uint32_t> size =
            Utils::GetMipSize(command.Source->GetWidth(), command.Source->GetHeight(), command.SourceMipLevel);

        VkImageSubresourceLayers srcSubresource = {};
        srcSubresource.aspectMask =
            source->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        srcSubresource.mipLevel = command.SourceMipLevel;
        srcSubresource.baseArrayLayer = 0;
        srcSubresource.layerCount = 1;

        if (source->GetType() != TextureType::Texture3D)
        {
            srcSubresource.baseArrayLayer = command.SourceArrayLayer;
        }

        VkOffset3D srcOffset = {};
        srcOffset.x = 0;
        srcOffset.y = 0;
        srcOffset.z = 0;

        if (source->GetType() == TextureType::Texture3D)
        {
            srcOffset.z = command.SourceArrayLayer;
        }

        VkImageSubresourceLayers dstSubresource = {};
        dstSubresource.aspectMask = destination->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
                                                           : VK_IMAGE_ASPECT_COLOR_BIT;
        dstSubresource.mipLevel = command.DestinationMipLevel;
        dstSubresource.baseArrayLayer = 0;
        dstSubresource.layerCount = 1;

        if (destination->GetType() != TextureType::Texture3D)
        {
            dstSubresource.baseArrayLayer = command.DestinationArrayLayer;
        }

        VkOffset3D dstOffset = {};
        dstOffset.x = 0;
        dstOffset.y = 0;
        dstOffset.z = 0;

        if (destination->GetType() == TextureType::Texture3D)
        {
            dstOffset.z = command.DestinationArrayLayer;
        }

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (context.CmdResolveImage2KHR)
        {
            VkImageResolve2KHR resolve = {};
            resolve.sType = VK_STRUCTURE_TYPE_IMAGE_RESOLVE_2_KHR;
            resolve.pNext = nullptr;
            resolve.srcOffset = srcOffset;
            resolve.dstOffset = dstOffset;
            resolve.extent = {size.X, size.Y, 1};
            resolve.srcSubresource = srcSubresource;
            resolve.dstSubresource = dstSubresource;

            VkResolveImageInfo2KHR resolveInfo = {};
            resolveInfo.sType = VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2_KHR;
            resolveInfo.pNext = nullptr;
            resolveInfo.srcImage = source->GetImage();
            resolveInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            resolveInfo.dstImage = destination->GetImage();
            resolveInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            resolveInfo.regionCount = 1;
            resolveInfo.pRegions = &resolve;

            context.CmdResolveImage2KHR(m_CommandBuffer, &resolveInfo);
        }
        else
        {
            VkImageResolve resolve = {};
            resolve.srcOffset = srcOffset;
            resolve.dstOffset = dstOffset;
            resolve.extent = {size.X, size.Y, 1};
            resolve.srcSubresource = srcSubresource;
            resolve.dstSubresource = dstSubresource;

            context.CmdResolveImage(
                m_CommandBuffer, source->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destination->GetImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &resolve
            );
        }

        ExecuteCommand(m_CurrentRenderTarget, device);
    }

    void CommandExecutorVk::ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        TimingQueryHandle queryHandle = command.Query;
        if (TimingQueryVk *queryVk = queryHandle.AsDerived<TimingQueryVk>())
        {
            const GladVulkanContext &context = m_Device->GetVulkanContext();
            context.CmdResetQueryPool(m_CommandBuffer, queryVk->GetQueryPool(), 0, 2);
            context.CmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0);
        }
    }

    void CommandExecutorVk::ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        TimingQueryHandle queryHandle = command.Query;
        if (TimingQueryVk *queryVk = queryHandle.AsDerived<TimingQueryVk>())
        {
            const GladVulkanContext &context = m_Device->GetVulkanContext();
            context.CmdWriteTimestamp(
                m_CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryVk->GetQueryPool(), 1
            );
        }
    }

    void CommandExecutorVk::ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        const DeviceBufferVk *src = command.BufferCopy.Source.AsDerived<const DeviceBufferVk>();
        const DeviceBufferVk *dst = command.BufferCopy.Destination.AsDerived<const DeviceBufferVk>();

        if (!src || !dst)
        {
            return;
        }

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (context.CmdCopyBuffer2KHR)
        {
            std::vector<VkBufferCopy2KHR> bufferCopies;

            for (const auto &copy : command.BufferCopy.Copies)
            {
                VkBufferCopy2KHR &bufferCopy = bufferCopies.emplace_back();
                bufferCopy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR;
                bufferCopy.pNext = nullptr;
                bufferCopy.srcOffset = copy.ReadOffset;
                bufferCopy.dstOffset = copy.WriteOffset;
                bufferCopy.size = copy.Size;
            }

            VkCopyBufferInfo2KHR copyInfo = {};
            copyInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2_KHR;
            copyInfo.pNext = nullptr;
            copyInfo.srcBuffer = src->GetVkBuffer();
            copyInfo.dstBuffer = dst->GetVkBuffer();
            copyInfo.regionCount = bufferCopies.size();
            copyInfo.pRegions = bufferCopies.data();

            context.CmdCopyBuffer2KHR(m_CommandBuffer, &copyInfo);
        }
        else
        {
            std::vector<VkBufferCopy> bufferCopies;

            for (const auto &copy : command.BufferCopy.Copies)
            {
                VkBufferCopy &bufferCopy = bufferCopies.emplace_back();
                bufferCopy.srcOffset = copy.ReadOffset;
                bufferCopy.dstOffset = copy.WriteOffset;
                bufferCopy.size = copy.Size;
            }

            context.CmdCopyBuffer(
                m_CommandBuffer, src->GetVkBuffer(), dst->GetVkBuffer(), bufferCopies.size(), bufferCopies.data()
            );
        }
    }

    void CommandExecutorVk::ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        GraphicsDeviceVk *deviceVk = (GraphicsDeviceVk *)device;
        const DeviceBufferVk *buffer = command.BufferTextureCopy.BufferHandle.AsDerived<const DeviceBufferVk>();
        const TextureVk *texture = command.BufferTextureCopy.Texture.AsDerived<const TextureVk>();
        VkImageAspectFlagBits aspectFlags = Vk::GetAspectFlags(texture->IsDepth());

        if (!buffer)
        {
            return;
        }

        const uint32_t copyDepth = 1;

        std::map<uint32_t, VkImageLayout> previousLayouts;

        // perform copy
        {
            const GladVulkanContext &context = m_Device->GetVulkanContext();

            VkImageSubresourceLayers imageSubresource = {};
            imageSubresource.aspectMask = aspectFlags;
            imageSubresource.mipLevel = command.BufferTextureCopy.MipLevel;
            imageSubresource.baseArrayLayer = 0;
            imageSubresource.layerCount = 1;

            if (texture->GetType() != TextureType::Texture3D)
            {
                imageSubresource.baseArrayLayer = command.BufferTextureCopy.TextureOffset.Z;
                imageSubresource.layerCount = copyDepth;
            }

            VkOffset3D imageOffset = {};
            imageOffset.x = command.BufferTextureCopy.TextureOffset.X;
            imageOffset.y = command.BufferTextureCopy.TextureOffset.Y;
            imageOffset.z = 0;

            if (texture->GetType() == TextureType::Texture3D)
            {
                imageOffset.z = command.BufferTextureCopy.TextureOffset.Z;
            }

            VkExtent3D imageExtent = {};
            imageExtent.width = command.BufferTextureCopy.TextureExtent.Width;
            imageExtent.height = command.BufferTextureCopy.TextureExtent.Height;
            imageExtent.depth = copyDepth;

            if (context.CmdCopyBufferToImage2KHR)
            {
                VkBufferImageCopy2KHR copyRegion = {};
                copyRegion.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR;
                copyRegion.pNext = nullptr;
                copyRegion.bufferOffset = command.BufferTextureCopy.BufferOffset;
                copyRegion.bufferRowLength = command.BufferTextureCopy.BufferRowLength;
                copyRegion.bufferImageHeight = command.BufferTextureCopy.BufferImageHeight;
                copyRegion.imageSubresource = imageSubresource;
                copyRegion.imageOffset = imageOffset;
                copyRegion.imageExtent = imageExtent;

                VkCopyBufferToImageInfo2KHR copyInfo = {};
                copyInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2_KHR;
                copyInfo.pNext = nullptr;
                copyInfo.srcBuffer = buffer->GetVkBuffer();
                copyInfo.dstImage = texture->GetImage();
                copyInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                copyInfo.pRegions = &copyRegion;
                copyInfo.regionCount = 1;

                context.CmdCopyBufferToImage2KHR(m_CommandBuffer, &copyInfo);
            }
            else
            {
                VkBufferImageCopy copyRegion = {};
                copyRegion.bufferOffset = command.BufferTextureCopy.BufferOffset;
                copyRegion.bufferRowLength = command.BufferTextureCopy.BufferRowLength;
                copyRegion.bufferImageHeight = command.BufferTextureCopy.BufferImageHeight;
                copyRegion.imageSubresource = imageSubresource;
                copyRegion.imageOffset = imageOffset;
                copyRegion.imageExtent = imageExtent;

                context.CmdCopyBufferToImage(
                    m_CommandBuffer, buffer->GetVkBuffer(), texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &copyRegion
                );
            }
        }
    }

    void CommandExecutorVk::ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        GraphicsDeviceVk *deviceVk = (GraphicsDeviceVk *)device;
        const DeviceBufferVk *buffer = command.TextureBufferCopy.BufferHandle.AsDerived<const DeviceBufferVk>();

        if (!buffer)
        {
            return;
        }

        const TextureVk *texture = command.TextureBufferCopy.Texture.AsDerived<const TextureVk>();
        VkImageAspectFlagBits aspectFlags = Vk::GetAspectFlags(texture->IsDepth());

        std::map<uint32_t, VkImageLayout> previousLayouts;

        const bool copyDepth = 1;

        // perform copy
        {
            const GladVulkanContext &context = m_Device->GetVulkanContext();

            VkImageSubresourceLayers imageSubresource = {};
            imageSubresource.aspectMask = aspectFlags;
            imageSubresource.mipLevel = command.TextureBufferCopy.MipLevel;
            imageSubresource.baseArrayLayer = 0;
            imageSubresource.layerCount = 1;

            if (texture->GetType() != TextureType::Texture3D)
            {
                imageSubresource.baseArrayLayer = command.TextureBufferCopy.TextureOffset.Z;
                imageSubresource.layerCount = copyDepth;
            }

            VkOffset3D imageOffset = {};
            imageOffset.x = command.TextureBufferCopy.TextureOffset.X;
            imageOffset.y = command.TextureBufferCopy.TextureOffset.Y;
            imageOffset.z = 0;

            if (texture->GetType() == TextureType::Texture3D)
            {
                imageOffset.z = command.TextureBufferCopy.TextureOffset.Z;
            }

            VkExtent3D imageExtent = {};
            imageExtent.width = command.TextureBufferCopy.TextureExtent.Width;
            imageExtent.height = command.TextureBufferCopy.TextureExtent.Height;
            imageExtent.depth = copyDepth;

            if (context.CmdCopyImageToBuffer2KHR)
            {
                VkBufferImageCopy2KHR copyRegion = {};
                copyRegion.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR;
                copyRegion.pNext = nullptr;
                copyRegion.bufferOffset = command.TextureBufferCopy.BufferOffset;
                copyRegion.bufferRowLength = command.TextureBufferCopy.BufferRowLength;
                copyRegion.bufferImageHeight = command.TextureBufferCopy.BufferImageHeight;
                copyRegion.imageSubresource = imageSubresource;
                copyRegion.imageOffset = imageOffset;
                copyRegion.imageExtent = imageExtent;

                VkCopyImageToBufferInfo2KHR copyInfo = {};
                copyInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2_KHR;
                copyInfo.pNext = nullptr;
                copyInfo.srcImage = texture->GetImage();
                copyInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                copyInfo.dstBuffer = buffer->GetVkBuffer();
                copyInfo.regionCount = 1;
                copyInfo.pRegions = &copyRegion;

                context.CmdCopyImageToBuffer2KHR(m_CommandBuffer, &copyInfo);
            }
            else
            {
                VkBufferImageCopy copyRegion = {};
                copyRegion.bufferOffset = command.TextureBufferCopy.BufferOffset;
                copyRegion.bufferRowLength = command.TextureBufferCopy.BufferRowLength;
                copyRegion.bufferImageHeight = command.TextureBufferCopy.BufferImageHeight;
                copyRegion.imageSubresource = imageSubresource;
                copyRegion.imageOffset = imageOffset;
                copyRegion.imageExtent = imageExtent;

                context.CmdCopyImageToBuffer(
                    m_CommandBuffer, texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer->GetVkBuffer(),
                    1, &copyRegion
                );
            }
        }
    }

    void CommandExecutorVk::ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        GraphicsDeviceVk *deviceVk = (GraphicsDeviceVk *)device;
        const TextureVk *srcTexture = command.TextureCopy.Source.AsDerived<const TextureVk>();
        const TextureVk *dstTexture = command.TextureCopy.Destination.AsDerived<const TextureVk>();

        VkImageAspectFlagBits srcAspect = Vk::GetAspectFlags(srcTexture->IsDepth());
        VkImageAspectFlagBits dstAspect = Vk::GetAspectFlags(dstTexture->IsDepth());

        std::map<uint32_t, VkImageLayout> srcLayouts;
        std::map<uint32_t, VkImageLayout> dstLayouts;

        const bool copyDepth = 1;

        // copy image
        {
            VkImageSubresourceLayers srcSubresource = {};
            srcSubresource.aspectMask = srcAspect;
            srcSubresource.mipLevel = command.TextureCopy.SourceMipLevel;
            srcSubresource.baseArrayLayer = 0;
            srcSubresource.layerCount = 1;

            if (srcTexture->GetType() != TextureType::Texture3D)
            {
                srcSubresource.baseArrayLayer = command.TextureCopy.SourceOffset.Z;
                srcSubresource.layerCount = copyDepth;
            }

            VkOffset3D srcOffset = {};
            srcOffset.x = command.TextureCopy.SourceOffset.X;
            srcOffset.y = command.TextureCopy.SourceOffset.Y;
            srcOffset.z = 0;

            if (srcTexture->GetType() != TextureType::Texture2D)
            {
                srcOffset.z = command.TextureCopy.SourceOffset.Z;
            }

            VkImageSubresourceLayers dstSubresource = {};
            dstSubresource.aspectMask = dstAspect;
            dstSubresource.mipLevel = command.TextureCopy.DestinationMipLevel;
            dstSubresource.baseArrayLayer = 0;
            dstSubresource.layerCount = 1;

            // we can only set these parameters for array textures, i.e. not 3D
            // textures
            if (dstTexture->GetType() != TextureType::Texture3D)
            {
                dstSubresource.baseArrayLayer = command.TextureCopy.DestinationOffset.Z;
                dstSubresource.layerCount = copyDepth;
            }

            VkOffset3D dstOffset = {};
            dstOffset.x = command.TextureCopy.DestinationOffset.X;
            dstOffset.y = command.TextureCopy.DestinationOffset.Y;
            dstOffset.z = 0;

            if (dstTexture->GetType() != TextureType::Texture2D)
            {
                dstOffset.z = command.TextureCopy.DestinationOffset.Z;
            }

            VkExtent3D copyExtent = {};
            copyExtent.width = command.TextureCopy.Extent.Width;
            copyExtent.height = command.TextureCopy.Extent.Height;
            copyExtent.depth = copyDepth;

            const GladVulkanContext &context = m_Device->GetVulkanContext();

            if (context.CmdCopyImage2KHR)
            {
                VkImageCopy2KHR copyRegion = {};
                copyRegion.sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2_KHR;
                copyRegion.pNext = nullptr;
                copyRegion.srcSubresource = srcSubresource;
                copyRegion.srcOffset = srcOffset;
                copyRegion.dstSubresource = dstSubresource;
                copyRegion.dstOffset = dstOffset;
                copyRegion.extent = copyExtent;

                VkCopyImageInfo2KHR copyInfo = {};
                copyInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2_KHR;
                copyInfo.pNext = nullptr;
                copyInfo.srcImage = srcTexture->GetImage();
                copyInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                copyInfo.dstImage = dstTexture->GetImage();
                copyInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                copyInfo.regionCount = 1;
                copyInfo.pRegions = &copyRegion;

                context.CmdCopyImage2KHR(m_CommandBuffer, &copyInfo);
            }
            else
            {
                VkImageCopy copyRegion = {};

                // src
                copyRegion.srcSubresource = srcSubresource;
                copyRegion.srcOffset = srcOffset;

                // dst
                copyRegion.dstSubresource = dstSubresource;
                copyRegion.dstOffset = dstOffset;

                // copy extents
                copyRegion.extent = copyExtent;

                context.CmdCopyImage(
                    m_CommandBuffer, srcTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    dstTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion
                );
            }
        }
    }

    void CommandExecutorVk::ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (context.CmdBeginDebugUtilsLabelEXT)
        {
            VkDebugUtilsLabelEXT labelEXT = {};
            labelEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            labelEXT.pNext = nullptr;
            labelEXT.pLabelName = command.GroupName.c_str();
            labelEXT.color[0] = command.Colour.r;
            labelEXT.color[1] = command.Colour.g;
            labelEXT.color[2] = command.Colour.b;
            labelEXT.color[3] = command.Colour.a;
            context.CmdBeginDebugUtilsLabelEXT(m_CommandBuffer, &labelEXT);
        }
        else if (context.CmdDebugMarkerBeginEXT)
        {
            VkDebugMarkerMarkerInfoEXT markerInfo = {};
            markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
            markerInfo.pNext = nullptr;
            markerInfo.pMarkerName = command.GroupName.c_str();
            markerInfo.color[0] = command.Colour.r;
            markerInfo.color[1] = command.Colour.g;
            markerInfo.color[2] = command.Colour.b;
            markerInfo.color[3] = command.Colour.a;
            context.CmdDebugMarkerBeginEXT(m_CommandBuffer, &markerInfo);
        }
    }

    void CommandExecutorVk::ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        // if this is the last command in the buffer, then we must explicitly stop
        // rendering to ensure that the implict render pass management occurs in the
        // correct order
        if (this->m_LastCommand)
        {
            StopRendering();
        }
        // otherwise, if the next command is to set a new render target, we need to
        // stop rendering to ensure that they show in the correct order in debuggers
        else
        {
            if (this->m_NextCommandType.has_value() && this->m_NextCommandType.value() == CommandType::SetFramebuffer)
            {
                StopRendering();
            }
        }

        if (context.CmdEndDebugUtilsLabelEXT)
        {
            context.CmdEndDebugUtilsLabelEXT(m_CommandBuffer);
        }
        else if (context.CmdDebugMarkerEndEXT)
        {
            context.CmdDebugMarkerEndEXT(m_CommandBuffer);
        }
    }

    void CommandExecutorVk::ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (context.CmdInsertDebugUtilsLabelEXT)
        {
            VkDebugUtilsLabelEXT labelEXT = {};
            labelEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            labelEXT.pNext = nullptr;
            labelEXT.pLabelName = command.MarkerName.c_str();
            labelEXT.color[0] = command.Colour.r;
            labelEXT.color[1] = command.Colour.g;
            labelEXT.color[2] = command.Colour.b;
            labelEXT.color[3] = command.Colour.a;
            context.CmdInsertDebugUtilsLabelEXT(m_CommandBuffer, &labelEXT);
        }
        else if (context.CmdDebugMarkerInsertEXT)
        {
            VkDebugMarkerMarkerInfoEXT markerInfo = {};
            markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
            markerInfo.pNext = nullptr;
            markerInfo.pMarkerName = command.MarkerName.c_str();
            markerInfo.color[0] = command.Colour.r;
            markerInfo.color[1] = command.Colour.g;
            markerInfo.color[2] = command.Colour.b;
            markerInfo.color[3] = command.Colour.a;
            context.CmdDebugMarkerInsertEXT(m_CommandBuffer, &markerInfo);
        }
    }

    void CommandExecutorVk::ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        float blendConstants[4] = {
            command.BlendFactor.Red, command.BlendFactor.Green, command.BlendFactor.Blue, command.BlendFactor.Alpha
        };

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdSetBlendConstants(m_CommandBuffer, blendConstants);
    }

    void CommandExecutorVk::ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = m_Device->GetVulkanContext();
        context.CmdSetStencilReference(m_CommandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, command.StencilReference);
    }

    void CommandExecutorVk::ExecuteCommand(const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        // return early if the function is not available to use
        const GladVulkanContext &context = m_Device->GetVulkanContext();
        if (!context.CmdBuildAccelerationStructuresKHR)
        {
            return;
        }

        // create storage for the data
        std::vector<std::vector<VkAccelerationStructureGeometryKHR>> accelerationStructureGeometries = {};
        std::vector<VkAccelerationStructureBuildGeometryInfoKHR> buildGeometries = {};
        std::vector<std::vector<VkAccelerationStructureBuildRangeInfoKHR>> buildRanges = {};

        // loop through all requested builds
        for (const AccelerationStructureGeometryBuildDescription &buildGeometryInfo : command.BuildDescriptions)
        {
            // validate that required members have been filled in correctly
            NX_VALIDATE(
                buildGeometryInfo.Destination.IsValid(), "Acceleration structure build must have a destination"
            );
            NX_VALIDATE(buildGeometryInfo.ScratchBuffer, "Acceleration structure build must have a scratch buffer");

            if (buildGeometryInfo.Mode == AccelerationStructureBuildMode::Update)
            {
                NX_VALIDATE(buildGeometryInfo.Source.IsValid(), "Acceleration structure update must have a source");
            }

            // create a new vector to hold the information for the individual build
            std::vector<uint32_t> primitiveCounts;
            std::vector<VkAccelerationStructureGeometryKHR> &accelerationStructureGeometry =
                accelerationStructureGeometries.emplace_back();
            accelerationStructureGeometry =
                Vk::GetVulkanAccelerationStructureGeometries(buildGeometryInfo, primitiveCounts);

            // create the new build description
            buildGeometries.push_back(Vk::GetGeometryBuildInfo(buildGeometryInfo, accelerationStructureGeometry));

            // create a new vector to hold the build range
            std::vector<VkAccelerationStructureBuildRangeInfoKHR> &geometryBuildRange = buildRanges.emplace_back();

            // iterate through each build range and convert them to Vulkan types
            for (uint32_t primitiveCount : primitiveCounts)
            {
                geometryBuildRange.push_back(Vk::GetAccelerationStructureBuildRange(primitiveCount));
            }
        }

        std::vector<const VkAccelerationStructureBuildRangeInfoKHR *> buildRangePtrs;
        buildRangePtrs.reserve(buildRanges.size());
        for (const auto &range : buildRanges)
        {
            buildRangePtrs.push_back(range.data());
        }

        // execute the acceleration structure build
        context.CmdBuildAccelerationStructuresKHR(
            m_CommandBuffer, buildGeometries.size(), buildGeometries.data(), buildRangePtrs.data()
        );
    }

    void CommandExecutorVk::ExecuteCommand(const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device)
    {
        NX_PROFILE_FUNCTION();
    }

    void CommandExecutorVk::ExecuteCommand(
        const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device
    )
    {
        NX_PROFILE_FUNCTION();
    }

    void CommandExecutorVk::ExecuteCommand(
        const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device
    )
    {
        NX_PROFILE_FUNCTION();
    }

    void CommandExecutorVk::ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        if (!m_CurrentlyBoundResourceSet)
            return;

        std::optional<VkShaderStageFlags> stageFlags =
            m_CurrentlyBoundResourceSet->GetPushConstantsStageFlags(command.Name);

        if (!stageFlags.has_value())
            return;

        if (PipelineVk *pipeline = m_CurrentlyBoundPipeline.AsDerived<PipelineVk>())
        {
            const GladVulkanContext &context = m_Device->GetVulkanContext();

            if (context.CmdPushConstants2)
            {
                VkPushConstantsInfo pushConstantsInfo = {};
                pushConstantsInfo.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
                pushConstantsInfo.pNext = nullptr;
                pushConstantsInfo.layout = pipeline->GetPipelineLayout();
                pushConstantsInfo.stageFlags = stageFlags.value();
                pushConstantsInfo.offset = command.Offset;
                pushConstantsInfo.size = command.Data.size();
                pushConstantsInfo.pValues = command.Data.data();

                context.CmdPushConstants2(m_CommandBuffer, &pushConstantsInfo);
            }
            else if (context.CmdPushConstants2KHR)
            {
                VkPushConstantsInfoKHR pushConstantsInfo = {};
                pushConstantsInfo.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO_KHR;
                pushConstantsInfo.pNext = nullptr;
                pushConstantsInfo.layout = pipeline->GetPipelineLayout();
                pushConstantsInfo.stageFlags = stageFlags.value();
                pushConstantsInfo.offset = command.Offset;
                pushConstantsInfo.size = command.Data.size();
                pushConstantsInfo.pValues = command.Data.data();

                context.CmdPushConstants2KHR(m_CommandBuffer, &pushConstantsInfo);
            }
            else
            {
                context.CmdPushConstants(
                    m_CommandBuffer, pipeline->GetPipelineLayout(), stageFlags.value(), command.Offset,
                    command.Data.size(), command.Data.data()
                );
            }
        }
    }

    void CommandExecutorVk::ExecuteCommand(const BarrierGroupDescription &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        if (m_Rendering)
        {
            StopRendering();
        }

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        // for now VK_DEPENDENCY_BY_REGION_BIT is hardcoded, however this may need to
        // be exposed in future
        VkDependencyFlagBits dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        std::unordered_map<VkImage, std::deque<Vk::SubresourceRangeLayout>> ranges;

        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (const TextureBarrierDesc &textureBarrier : command.TextureBarriers)
        {
            const TextureVk *textureVk = textureBarrier.Texture.AsDerived<const TextureVk>();

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
                    Vk::SubresourceRangeLayout &range = ranges[textureVk->GetImage()].emplace_back();
                    range.range.aspectMask = Vk::GetAspectFlags(textureVk->IsDepth());
                    range.range.baseArrayLayer = arrayLayer;
                    range.range.layerCount = 1;
                    range.range.baseMipLevel = mipLevel;
                    range.range.levelCount = 1;
                    range.layout = Vk::GetImageLayout(m_Device, textureVk->GetTextureLayout(arrayLayer, mipLevel));
                }
            }
        }

        uint32_t srcQueue = VK_QUEUE_FAMILY_IGNORED;
        uint32_t dstQueue = VK_QUEUE_FAMILY_IGNORED;

        // submit a PipelineBarrier2
        if (context.CmdPipelineBarrier2)
        {
            std::vector<VkMemoryBarrier2> memoryBarriers = {};
            std::vector<VkImageMemoryBarrier2> textureBarriers = {};
            std::vector<VkBufferMemoryBarrier2> bufferBarriers = {};

            memoryBarriers.reserve(command.MemoryBarriers.size());
            textureBarriers.reserve(command.TextureBarriers.size());
            bufferBarriers.reserve(command.BufferBarriers.size());

            for (const MemoryBarrierDesc &memoryBarrier : command.MemoryBarriers)
            {
                Vk::CreateMemoryBarrier2(m_Device, memoryBarrier, memoryBarriers);
            }

            for (const TextureBarrierDesc &textureBarrier : command.TextureBarriers)
            {
                VkImageLayout layout = Vk::GetImageLayout(m_Device, textureBarrier.Layout);
                const TextureVk *textureVk = textureBarrier.Texture.AsDerived<const TextureVk>();
                Vk::CreateTextureBarrier2(
                    m_Device, textureVk->GetImage(), textureBarrier.BeforeAccess, textureBarrier.AfterAccess,
                    textureBarrier.BeforeStage, textureBarrier.AfterStage, srcQueue, dstQueue, layout, textureBarriers,
                    ranges
                );
            }

            for (const BufferBarrierDesc &bufferBarrier : command.BufferBarriers)
            {
                Vk::CreateBufferBarrier2(m_Device, bufferBarrier, bufferBarriers, srcQueue, dstQueue);
            }

            VkDependencyInfo dependencyInfo = {};
            dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            dependencyInfo.pNext = nullptr;
            dependencyInfo.dependencyFlags = dependencyFlags;
            dependencyInfo.memoryBarrierCount = static_cast<uint32_t>(memoryBarriers.size());
            dependencyInfo.pMemoryBarriers = memoryBarriers.data();
            dependencyInfo.bufferMemoryBarrierCount = static_cast<uint32_t>(bufferBarriers.size());
            dependencyInfo.pBufferMemoryBarriers = bufferBarriers.data();
            dependencyInfo.imageMemoryBarrierCount = static_cast<uint32_t>(textureBarriers.size());
            dependencyInfo.pImageMemoryBarriers = textureBarriers.data();

            context.CmdPipelineBarrier2(m_CommandBuffer, std::addressof(dependencyInfo));
        }
        // submit a legacy pipeline barrier
        else
        {
            std::vector<VkMemoryBarrier> memoryBarriers = {};
            std::vector<VkImageMemoryBarrier> textureBarriers = {};
            std::vector<VkBufferMemoryBarrier> bufferBarriers = {};

            memoryBarriers.reserve(command.MemoryBarriers.size());
            textureBarriers.reserve(command.TextureBarriers.size());
            bufferBarriers.reserve(command.BufferBarriers.size());

            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_NONE;
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_NONE;

            for (const MemoryBarrierDesc &memoryBarrier : command.MemoryBarriers)
            {
                Vk::CreateMemoryBarrier(m_Device, memoryBarrier, memoryBarriers);
            }

            for (const TextureBarrierDesc &textureBarrier : command.TextureBarriers)
            {
                VkImageLayout layout = Vk::GetImageLayout(m_Device, textureBarrier.Layout);
                const TextureVk *textureVk = textureBarrier.Texture.AsDerived<const TextureVk>();
                Vk::CreateTextureBarrier(
                    m_Device, textureVk->GetImage(), textureBarrier.BeforeAccess, textureBarrier.AfterAccess,
                    textureBarrier.BeforeStage, textureBarrier.AfterStage, srcQueue, dstQueue, layout, textureBarriers,
                    ranges
                );

                srcStageMask |= Vk::GetPipelineStageFlags(m_Device, textureBarrier.BeforeStage);
                dstStageMask |= Vk::GetPipelineStageFlags(m_Device, textureBarrier.AfterStage);
            }

            for (const BufferBarrierDesc &bufferBarrier : command.BufferBarriers)
            {
                Vk::CreateBufferBarrier(m_Device, bufferBarrier, bufferBarriers, srcQueue, dstQueue);
                srcStageMask |= Vk::GetPipelineStageFlags(m_Device, bufferBarrier.BeforeStage);
                dstStageMask |= Vk::GetPipelineStageFlags(m_Device, bufferBarrier.AfterStage);
            }

            context.CmdPipelineBarrier(
                m_CommandBuffer, srcStageMask, dstStageMask, dependencyFlags,
                static_cast<uint32_t>(memoryBarriers.size()), memoryBarriers.data(),
                static_cast<uint32_t>(bufferBarriers.size()), bufferBarriers.data(),
                static_cast<uint32_t>(textureBarriers.size()), textureBarriers.data()
            );
        }

        // update texture layouts
        // enumerate through all texture barriers and create the required subresource
        // ranges
        for (const TextureBarrierDesc &textureBarrier : command.TextureBarriers)
        {
            TextureHandle handle = textureBarrier.Texture;
            TextureVk *textureVk = handle.AsDerived<TextureVk>();

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
                    textureVk->SetTextureLayout(arrayLayer, mipLevel, textureBarrier.Layout);
                }
            }
        }
    }

    void CommandExecutorVk::ExecuteCommand(const TraceRaysDescription &desc, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (context.CmdTraceRaysKHR)
        {
            VkStridedDeviceAddressRegionKHR raygenRegion = {
                .deviceAddress = desc.RaygenRegion.Address,
                .stride = desc.RaygenRegion.Size,
                .size = desc.RaygenRegion.Size
            };

            VkStridedDeviceAddressRegionKHR missRegion = {
                .deviceAddress = desc.MissRegion.Address, .stride = desc.MissRegion.Stride, .size = desc.MissRegion.Size
            };

            VkStridedDeviceAddressRegionKHR hitRegion = {
                .deviceAddress = desc.HitRegion.Address, .stride = desc.HitRegion.Stride, .size = desc.HitRegion.Size
            };

            VkStridedDeviceAddressRegionKHR callableRegion = {
                .deviceAddress = desc.CallableRegion.Address,
                .stride = desc.CallableRegion.Stride,
                .size = desc.CallableRegion.Size
            };

            context.CmdTraceRaysKHR(
                m_CommandBuffer, &raygenRegion, &missRegion, &hitRegion, &callableRegion, desc.Width, desc.Height,
                desc.Depth
            );
        }
    }

    void CommandExecutorVk::ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device)
    {
        NX_PROFILE_FUNCTION();
    }

    void CommandExecutorVk::StopRendering()
    {
        NX_PROFILE_FUNCTION();

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        if (m_Rendering)
        {
            const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
            if (features.DynamicRenderingAvailable)
            {
                const GladVulkanContext &context = m_Device->GetVulkanContext();
                context.CmdEndRenderingKHR(m_CommandBuffer);
            }
            else
            {
                context.CmdEndRenderPass(m_CommandBuffer);
            }
        }

        m_Rendering = false;
    }

    bool CommandExecutorVk::ValidateIsRendering()
    {
        NX_PROFILE_FUNCTION();

        if (!m_Rendering)
        {
            NX_ERROR("Attempting to issue graphics command but rendering has not started");
            return false;
        }
        return true;
    }

    void CommandExecutorVk::BindGraphicsPipeline()
    {
        NX_PROFILE_FUNCTION();

        const VulkanDeviceFeatures &deviceFeatures = m_Device->GetDeviceFeatures();

        if (PipelineVk *pipeline = m_CurrentlyBoundPipeline.AsDerived<PipelineVk>())
        {
            if (m_CurrentRenderTarget.IsValid())
            {
                VkRenderPass renderPass = VK_NULL_HANDLE;

                const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
                if (!features.DynamicRenderingAvailable)
                {
                    FramebufferVk *framebufferVk = m_CurrentRenderTarget.AsDerived<FramebufferVk>();
                    renderPass = framebufferVk->GetRenderPass();
                    pipeline->Bind(m_CommandBuffer, renderPass);
                }
            }
            else
            {
                throw std::runtime_error("Failed to find a valid render target type");
            }
        }
    }

    void CommandExecutorVk::TryStartRendering()
    {
        NX_PROFILE_FUNCTION();

        if (!m_Rendering)
        {
            if (m_CurrentRenderTarget.IsValid())
            {
                StartRenderingToFramebuffer(m_CurrentRenderTarget);
            }
        }
    }
} // namespace Nexus::Graphics

#endif