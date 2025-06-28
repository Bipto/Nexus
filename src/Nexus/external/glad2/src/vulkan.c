/**
 * SPDX-License-Identifier: (WTFPL OR CC0-1.0) AND Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/vulkan.h>

#ifndef GLAD_IMPL_UTIL_C_
#define GLAD_IMPL_UTIL_C_

#ifdef _MSC_VER
#define GLAD_IMPL_UTIL_SSCANF sscanf_s
#else
#define GLAD_IMPL_UTIL_SSCANF sscanf
#endif

#endif /* GLAD_IMPL_UTIL_C_ */

#ifdef __cplusplus
extern "C" {
#endif








static void glad_vk_load_VK_VERSION_1_0(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->VERSION_1_0) return;
    context->AllocateCommandBuffers = (PFN_vkAllocateCommandBuffers) load(userptr, "vkAllocateCommandBuffers");
    context->AllocateDescriptorSets = (PFN_vkAllocateDescriptorSets) load(userptr, "vkAllocateDescriptorSets");
    context->AllocateMemory = (PFN_vkAllocateMemory) load(userptr, "vkAllocateMemory");
    context->BeginCommandBuffer = (PFN_vkBeginCommandBuffer) load(userptr, "vkBeginCommandBuffer");
    context->BindBufferMemory = (PFN_vkBindBufferMemory) load(userptr, "vkBindBufferMemory");
    context->BindImageMemory = (PFN_vkBindImageMemory) load(userptr, "vkBindImageMemory");
    context->CmdBeginQuery = (PFN_vkCmdBeginQuery) load(userptr, "vkCmdBeginQuery");
    context->CmdBeginRenderPass = (PFN_vkCmdBeginRenderPass) load(userptr, "vkCmdBeginRenderPass");
    context->CmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets) load(userptr, "vkCmdBindDescriptorSets");
    context->CmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer) load(userptr, "vkCmdBindIndexBuffer");
    context->CmdBindPipeline = (PFN_vkCmdBindPipeline) load(userptr, "vkCmdBindPipeline");
    context->CmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers) load(userptr, "vkCmdBindVertexBuffers");
    context->CmdBlitImage = (PFN_vkCmdBlitImage) load(userptr, "vkCmdBlitImage");
    context->CmdClearAttachments = (PFN_vkCmdClearAttachments) load(userptr, "vkCmdClearAttachments");
    context->CmdClearColorImage = (PFN_vkCmdClearColorImage) load(userptr, "vkCmdClearColorImage");
    context->CmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage) load(userptr, "vkCmdClearDepthStencilImage");
    context->CmdCopyBuffer = (PFN_vkCmdCopyBuffer) load(userptr, "vkCmdCopyBuffer");
    context->CmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage) load(userptr, "vkCmdCopyBufferToImage");
    context->CmdCopyImage = (PFN_vkCmdCopyImage) load(userptr, "vkCmdCopyImage");
    context->CmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer) load(userptr, "vkCmdCopyImageToBuffer");
    context->CmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults) load(userptr, "vkCmdCopyQueryPoolResults");
    context->CmdDispatch = (PFN_vkCmdDispatch) load(userptr, "vkCmdDispatch");
    context->CmdDispatchIndirect = (PFN_vkCmdDispatchIndirect) load(userptr, "vkCmdDispatchIndirect");
    context->CmdDraw = (PFN_vkCmdDraw) load(userptr, "vkCmdDraw");
    context->CmdDrawIndexed = (PFN_vkCmdDrawIndexed) load(userptr, "vkCmdDrawIndexed");
    context->CmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect) load(userptr, "vkCmdDrawIndexedIndirect");
    context->CmdDrawIndirect = (PFN_vkCmdDrawIndirect) load(userptr, "vkCmdDrawIndirect");
    context->CmdEndQuery = (PFN_vkCmdEndQuery) load(userptr, "vkCmdEndQuery");
    context->CmdEndRenderPass = (PFN_vkCmdEndRenderPass) load(userptr, "vkCmdEndRenderPass");
    context->CmdExecuteCommands = (PFN_vkCmdExecuteCommands) load(userptr, "vkCmdExecuteCommands");
    context->CmdFillBuffer = (PFN_vkCmdFillBuffer) load(userptr, "vkCmdFillBuffer");
    context->CmdNextSubpass = (PFN_vkCmdNextSubpass) load(userptr, "vkCmdNextSubpass");
    context->CmdPipelineBarrier = (PFN_vkCmdPipelineBarrier) load(userptr, "vkCmdPipelineBarrier");
    context->CmdPushConstants = (PFN_vkCmdPushConstants) load(userptr, "vkCmdPushConstants");
    context->CmdResetEvent = (PFN_vkCmdResetEvent) load(userptr, "vkCmdResetEvent");
    context->CmdResetQueryPool = (PFN_vkCmdResetQueryPool) load(userptr, "vkCmdResetQueryPool");
    context->CmdResolveImage = (PFN_vkCmdResolveImage) load(userptr, "vkCmdResolveImage");
    context->CmdSetBlendConstants = (PFN_vkCmdSetBlendConstants) load(userptr, "vkCmdSetBlendConstants");
    context->CmdSetDepthBias = (PFN_vkCmdSetDepthBias) load(userptr, "vkCmdSetDepthBias");
    context->CmdSetDepthBounds = (PFN_vkCmdSetDepthBounds) load(userptr, "vkCmdSetDepthBounds");
    context->CmdSetEvent = (PFN_vkCmdSetEvent) load(userptr, "vkCmdSetEvent");
    context->CmdSetLineWidth = (PFN_vkCmdSetLineWidth) load(userptr, "vkCmdSetLineWidth");
    context->CmdSetScissor = (PFN_vkCmdSetScissor) load(userptr, "vkCmdSetScissor");
    context->CmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask) load(userptr, "vkCmdSetStencilCompareMask");
    context->CmdSetStencilReference = (PFN_vkCmdSetStencilReference) load(userptr, "vkCmdSetStencilReference");
    context->CmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask) load(userptr, "vkCmdSetStencilWriteMask");
    context->CmdSetViewport = (PFN_vkCmdSetViewport) load(userptr, "vkCmdSetViewport");
    context->CmdUpdateBuffer = (PFN_vkCmdUpdateBuffer) load(userptr, "vkCmdUpdateBuffer");
    context->CmdWaitEvents = (PFN_vkCmdWaitEvents) load(userptr, "vkCmdWaitEvents");
    context->CmdWriteTimestamp = (PFN_vkCmdWriteTimestamp) load(userptr, "vkCmdWriteTimestamp");
    context->CreateBuffer = (PFN_vkCreateBuffer) load(userptr, "vkCreateBuffer");
    context->CreateBufferView = (PFN_vkCreateBufferView) load(userptr, "vkCreateBufferView");
    context->CreateCommandPool = (PFN_vkCreateCommandPool) load(userptr, "vkCreateCommandPool");
    context->CreateComputePipelines = (PFN_vkCreateComputePipelines) load(userptr, "vkCreateComputePipelines");
    context->CreateDescriptorPool = (PFN_vkCreateDescriptorPool) load(userptr, "vkCreateDescriptorPool");
    context->CreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout) load(userptr, "vkCreateDescriptorSetLayout");
    context->CreateDevice = (PFN_vkCreateDevice) load(userptr, "vkCreateDevice");
    context->CreateEvent = (PFN_vkCreateEvent) load(userptr, "vkCreateEvent");
    context->CreateFence = (PFN_vkCreateFence) load(userptr, "vkCreateFence");
    context->CreateFramebuffer = (PFN_vkCreateFramebuffer) load(userptr, "vkCreateFramebuffer");
    context->CreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines) load(userptr, "vkCreateGraphicsPipelines");
    context->CreateImage = (PFN_vkCreateImage) load(userptr, "vkCreateImage");
    context->CreateImageView = (PFN_vkCreateImageView) load(userptr, "vkCreateImageView");
    context->CreateInstance = (PFN_vkCreateInstance) load(userptr, "vkCreateInstance");
    context->CreatePipelineCache = (PFN_vkCreatePipelineCache) load(userptr, "vkCreatePipelineCache");
    context->CreatePipelineLayout = (PFN_vkCreatePipelineLayout) load(userptr, "vkCreatePipelineLayout");
    context->CreateQueryPool = (PFN_vkCreateQueryPool) load(userptr, "vkCreateQueryPool");
    context->CreateRenderPass = (PFN_vkCreateRenderPass) load(userptr, "vkCreateRenderPass");
    context->CreateSampler = (PFN_vkCreateSampler) load(userptr, "vkCreateSampler");
    context->CreateSemaphore = (PFN_vkCreateSemaphore) load(userptr, "vkCreateSemaphore");
    context->CreateShaderModule = (PFN_vkCreateShaderModule) load(userptr, "vkCreateShaderModule");
    context->DestroyBuffer = (PFN_vkDestroyBuffer) load(userptr, "vkDestroyBuffer");
    context->DestroyBufferView = (PFN_vkDestroyBufferView) load(userptr, "vkDestroyBufferView");
    context->DestroyCommandPool = (PFN_vkDestroyCommandPool) load(userptr, "vkDestroyCommandPool");
    context->DestroyDescriptorPool = (PFN_vkDestroyDescriptorPool) load(userptr, "vkDestroyDescriptorPool");
    context->DestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout) load(userptr, "vkDestroyDescriptorSetLayout");
    context->DestroyDevice = (PFN_vkDestroyDevice) load(userptr, "vkDestroyDevice");
    context->DestroyEvent = (PFN_vkDestroyEvent) load(userptr, "vkDestroyEvent");
    context->DestroyFence = (PFN_vkDestroyFence) load(userptr, "vkDestroyFence");
    context->DestroyFramebuffer = (PFN_vkDestroyFramebuffer) load(userptr, "vkDestroyFramebuffer");
    context->DestroyImage = (PFN_vkDestroyImage) load(userptr, "vkDestroyImage");
    context->DestroyImageView = (PFN_vkDestroyImageView) load(userptr, "vkDestroyImageView");
    context->DestroyInstance = (PFN_vkDestroyInstance) load(userptr, "vkDestroyInstance");
    context->DestroyPipeline = (PFN_vkDestroyPipeline) load(userptr, "vkDestroyPipeline");
    context->DestroyPipelineCache = (PFN_vkDestroyPipelineCache) load(userptr, "vkDestroyPipelineCache");
    context->DestroyPipelineLayout = (PFN_vkDestroyPipelineLayout) load(userptr, "vkDestroyPipelineLayout");
    context->DestroyQueryPool = (PFN_vkDestroyQueryPool) load(userptr, "vkDestroyQueryPool");
    context->DestroyRenderPass = (PFN_vkDestroyRenderPass) load(userptr, "vkDestroyRenderPass");
    context->DestroySampler = (PFN_vkDestroySampler) load(userptr, "vkDestroySampler");
    context->DestroySemaphore = (PFN_vkDestroySemaphore) load(userptr, "vkDestroySemaphore");
    context->DestroyShaderModule = (PFN_vkDestroyShaderModule) load(userptr, "vkDestroyShaderModule");
    context->DeviceWaitIdle = (PFN_vkDeviceWaitIdle) load(userptr, "vkDeviceWaitIdle");
    context->EndCommandBuffer = (PFN_vkEndCommandBuffer) load(userptr, "vkEndCommandBuffer");
    context->EnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties) load(userptr, "vkEnumerateDeviceExtensionProperties");
    context->EnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties) load(userptr, "vkEnumerateDeviceLayerProperties");
    context->EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties) load(userptr, "vkEnumerateInstanceExtensionProperties");
    context->EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) load(userptr, "vkEnumerateInstanceLayerProperties");
    context->EnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) load(userptr, "vkEnumeratePhysicalDevices");
    context->FlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges) load(userptr, "vkFlushMappedMemoryRanges");
    context->FreeCommandBuffers = (PFN_vkFreeCommandBuffers) load(userptr, "vkFreeCommandBuffers");
    context->FreeDescriptorSets = (PFN_vkFreeDescriptorSets) load(userptr, "vkFreeDescriptorSets");
    context->FreeMemory = (PFN_vkFreeMemory) load(userptr, "vkFreeMemory");
    context->GetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements) load(userptr, "vkGetBufferMemoryRequirements");
    context->GetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment) load(userptr, "vkGetDeviceMemoryCommitment");
    context->GetDeviceProcAddr = (PFN_vkGetDeviceProcAddr) load(userptr, "vkGetDeviceProcAddr");
    context->GetDeviceQueue = (PFN_vkGetDeviceQueue) load(userptr, "vkGetDeviceQueue");
    context->GetEventStatus = (PFN_vkGetEventStatus) load(userptr, "vkGetEventStatus");
    context->GetFenceStatus = (PFN_vkGetFenceStatus) load(userptr, "vkGetFenceStatus");
    context->GetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements) load(userptr, "vkGetImageMemoryRequirements");
    context->GetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements) load(userptr, "vkGetImageSparseMemoryRequirements");
    context->GetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout) load(userptr, "vkGetImageSubresourceLayout");
    context->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) load(userptr, "vkGetInstanceProcAddr");
    context->GetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures) load(userptr, "vkGetPhysicalDeviceFeatures");
    context->GetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties) load(userptr, "vkGetPhysicalDeviceFormatProperties");
    context->GetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties) load(userptr, "vkGetPhysicalDeviceImageFormatProperties");
    context->GetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties) load(userptr, "vkGetPhysicalDeviceMemoryProperties");
    context->GetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties) load(userptr, "vkGetPhysicalDeviceProperties");
    context->GetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties) load(userptr, "vkGetPhysicalDeviceQueueFamilyProperties");
    context->GetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties) load(userptr, "vkGetPhysicalDeviceSparseImageFormatProperties");
    context->GetPipelineCacheData = (PFN_vkGetPipelineCacheData) load(userptr, "vkGetPipelineCacheData");
    context->GetQueryPoolResults = (PFN_vkGetQueryPoolResults) load(userptr, "vkGetQueryPoolResults");
    context->GetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity) load(userptr, "vkGetRenderAreaGranularity");
    context->InvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges) load(userptr, "vkInvalidateMappedMemoryRanges");
    context->MapMemory = (PFN_vkMapMemory) load(userptr, "vkMapMemory");
    context->MergePipelineCaches = (PFN_vkMergePipelineCaches) load(userptr, "vkMergePipelineCaches");
    context->QueueBindSparse = (PFN_vkQueueBindSparse) load(userptr, "vkQueueBindSparse");
    context->QueueSubmit = (PFN_vkQueueSubmit) load(userptr, "vkQueueSubmit");
    context->QueueWaitIdle = (PFN_vkQueueWaitIdle) load(userptr, "vkQueueWaitIdle");
    context->ResetCommandBuffer = (PFN_vkResetCommandBuffer) load(userptr, "vkResetCommandBuffer");
    context->ResetCommandPool = (PFN_vkResetCommandPool) load(userptr, "vkResetCommandPool");
    context->ResetDescriptorPool = (PFN_vkResetDescriptorPool) load(userptr, "vkResetDescriptorPool");
    context->ResetEvent = (PFN_vkResetEvent) load(userptr, "vkResetEvent");
    context->ResetFences = (PFN_vkResetFences) load(userptr, "vkResetFences");
    context->SetEvent = (PFN_vkSetEvent) load(userptr, "vkSetEvent");
    context->UnmapMemory = (PFN_vkUnmapMemory) load(userptr, "vkUnmapMemory");
    context->UpdateDescriptorSets = (PFN_vkUpdateDescriptorSets) load(userptr, "vkUpdateDescriptorSets");
    context->WaitForFences = (PFN_vkWaitForFences) load(userptr, "vkWaitForFences");
}
static void glad_vk_load_VK_VERSION_1_1(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->VERSION_1_1) return;
    context->BindBufferMemory2 = (PFN_vkBindBufferMemory2) load(userptr, "vkBindBufferMemory2");
    context->BindImageMemory2 = (PFN_vkBindImageMemory2) load(userptr, "vkBindImageMemory2");
    context->CmdDispatchBase = (PFN_vkCmdDispatchBase) load(userptr, "vkCmdDispatchBase");
    context->CmdSetDeviceMask = (PFN_vkCmdSetDeviceMask) load(userptr, "vkCmdSetDeviceMask");
    context->CreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate) load(userptr, "vkCreateDescriptorUpdateTemplate");
    context->CreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion) load(userptr, "vkCreateSamplerYcbcrConversion");
    context->DestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate) load(userptr, "vkDestroyDescriptorUpdateTemplate");
    context->DestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion) load(userptr, "vkDestroySamplerYcbcrConversion");
    context->EnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion) load(userptr, "vkEnumerateInstanceVersion");
    context->EnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups) load(userptr, "vkEnumeratePhysicalDeviceGroups");
    context->GetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2) load(userptr, "vkGetBufferMemoryRequirements2");
    context->GetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport) load(userptr, "vkGetDescriptorSetLayoutSupport");
    context->GetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures) load(userptr, "vkGetDeviceGroupPeerMemoryFeatures");
    context->GetDeviceQueue2 = (PFN_vkGetDeviceQueue2) load(userptr, "vkGetDeviceQueue2");
    context->GetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2) load(userptr, "vkGetImageMemoryRequirements2");
    context->GetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2) load(userptr, "vkGetImageSparseMemoryRequirements2");
    context->GetPhysicalDeviceExternalBufferProperties = (PFN_vkGetPhysicalDeviceExternalBufferProperties) load(userptr, "vkGetPhysicalDeviceExternalBufferProperties");
    context->GetPhysicalDeviceExternalFenceProperties = (PFN_vkGetPhysicalDeviceExternalFenceProperties) load(userptr, "vkGetPhysicalDeviceExternalFenceProperties");
    context->GetPhysicalDeviceExternalSemaphoreProperties = (PFN_vkGetPhysicalDeviceExternalSemaphoreProperties) load(userptr, "vkGetPhysicalDeviceExternalSemaphoreProperties");
    context->GetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2) load(userptr, "vkGetPhysicalDeviceFeatures2");
    context->GetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2) load(userptr, "vkGetPhysicalDeviceFormatProperties2");
    context->GetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2) load(userptr, "vkGetPhysicalDeviceImageFormatProperties2");
    context->GetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2) load(userptr, "vkGetPhysicalDeviceMemoryProperties2");
    context->GetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2) load(userptr, "vkGetPhysicalDeviceProperties2");
    context->GetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2) load(userptr, "vkGetPhysicalDeviceQueueFamilyProperties2");
    context->GetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2) load(userptr, "vkGetPhysicalDeviceSparseImageFormatProperties2");
    context->TrimCommandPool = (PFN_vkTrimCommandPool) load(userptr, "vkTrimCommandPool");
    context->UpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate) load(userptr, "vkUpdateDescriptorSetWithTemplate");
}
static void glad_vk_load_VK_VERSION_1_2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->VERSION_1_2) return;
    context->CmdBeginRenderPass2 = (PFN_vkCmdBeginRenderPass2) load(userptr, "vkCmdBeginRenderPass2");
    context->CmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount) load(userptr, "vkCmdDrawIndexedIndirectCount");
    context->CmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount) load(userptr, "vkCmdDrawIndirectCount");
    context->CmdEndRenderPass2 = (PFN_vkCmdEndRenderPass2) load(userptr, "vkCmdEndRenderPass2");
    context->CmdNextSubpass2 = (PFN_vkCmdNextSubpass2) load(userptr, "vkCmdNextSubpass2");
    context->CreateRenderPass2 = (PFN_vkCreateRenderPass2) load(userptr, "vkCreateRenderPass2");
    context->GetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) load(userptr, "vkGetBufferDeviceAddress");
    context->GetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress) load(userptr, "vkGetBufferOpaqueCaptureAddress");
    context->GetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress) load(userptr, "vkGetDeviceMemoryOpaqueCaptureAddress");
    context->GetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue) load(userptr, "vkGetSemaphoreCounterValue");
    context->ResetQueryPool = (PFN_vkResetQueryPool) load(userptr, "vkResetQueryPool");
    context->SignalSemaphore = (PFN_vkSignalSemaphore) load(userptr, "vkSignalSemaphore");
    context->WaitSemaphores = (PFN_vkWaitSemaphores) load(userptr, "vkWaitSemaphores");
}
static void glad_vk_load_VK_VERSION_1_3(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->VERSION_1_3) return;
    context->CmdBeginRendering = (PFN_vkCmdBeginRendering) load(userptr, "vkCmdBeginRendering");
    context->CmdBindVertexBuffers2 = (PFN_vkCmdBindVertexBuffers2) load(userptr, "vkCmdBindVertexBuffers2");
    context->CmdBlitImage2 = (PFN_vkCmdBlitImage2) load(userptr, "vkCmdBlitImage2");
    context->CmdCopyBuffer2 = (PFN_vkCmdCopyBuffer2) load(userptr, "vkCmdCopyBuffer2");
    context->CmdCopyBufferToImage2 = (PFN_vkCmdCopyBufferToImage2) load(userptr, "vkCmdCopyBufferToImage2");
    context->CmdCopyImage2 = (PFN_vkCmdCopyImage2) load(userptr, "vkCmdCopyImage2");
    context->CmdCopyImageToBuffer2 = (PFN_vkCmdCopyImageToBuffer2) load(userptr, "vkCmdCopyImageToBuffer2");
    context->CmdEndRendering = (PFN_vkCmdEndRendering) load(userptr, "vkCmdEndRendering");
    context->CmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2) load(userptr, "vkCmdPipelineBarrier2");
    context->CmdResetEvent2 = (PFN_vkCmdResetEvent2) load(userptr, "vkCmdResetEvent2");
    context->CmdResolveImage2 = (PFN_vkCmdResolveImage2) load(userptr, "vkCmdResolveImage2");
    context->CmdSetCullMode = (PFN_vkCmdSetCullMode) load(userptr, "vkCmdSetCullMode");
    context->CmdSetDepthBiasEnable = (PFN_vkCmdSetDepthBiasEnable) load(userptr, "vkCmdSetDepthBiasEnable");
    context->CmdSetDepthBoundsTestEnable = (PFN_vkCmdSetDepthBoundsTestEnable) load(userptr, "vkCmdSetDepthBoundsTestEnable");
    context->CmdSetDepthCompareOp = (PFN_vkCmdSetDepthCompareOp) load(userptr, "vkCmdSetDepthCompareOp");
    context->CmdSetDepthTestEnable = (PFN_vkCmdSetDepthTestEnable) load(userptr, "vkCmdSetDepthTestEnable");
    context->CmdSetDepthWriteEnable = (PFN_vkCmdSetDepthWriteEnable) load(userptr, "vkCmdSetDepthWriteEnable");
    context->CmdSetEvent2 = (PFN_vkCmdSetEvent2) load(userptr, "vkCmdSetEvent2");
    context->CmdSetFrontFace = (PFN_vkCmdSetFrontFace) load(userptr, "vkCmdSetFrontFace");
    context->CmdSetPrimitiveRestartEnable = (PFN_vkCmdSetPrimitiveRestartEnable) load(userptr, "vkCmdSetPrimitiveRestartEnable");
    context->CmdSetPrimitiveTopology = (PFN_vkCmdSetPrimitiveTopology) load(userptr, "vkCmdSetPrimitiveTopology");
    context->CmdSetRasterizerDiscardEnable = (PFN_vkCmdSetRasterizerDiscardEnable) load(userptr, "vkCmdSetRasterizerDiscardEnable");
    context->CmdSetScissorWithCount = (PFN_vkCmdSetScissorWithCount) load(userptr, "vkCmdSetScissorWithCount");
    context->CmdSetStencilOp = (PFN_vkCmdSetStencilOp) load(userptr, "vkCmdSetStencilOp");
    context->CmdSetStencilTestEnable = (PFN_vkCmdSetStencilTestEnable) load(userptr, "vkCmdSetStencilTestEnable");
    context->CmdSetViewportWithCount = (PFN_vkCmdSetViewportWithCount) load(userptr, "vkCmdSetViewportWithCount");
    context->CmdWaitEvents2 = (PFN_vkCmdWaitEvents2) load(userptr, "vkCmdWaitEvents2");
    context->CmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2) load(userptr, "vkCmdWriteTimestamp2");
    context->CreatePrivateDataSlot = (PFN_vkCreatePrivateDataSlot) load(userptr, "vkCreatePrivateDataSlot");
    context->DestroyPrivateDataSlot = (PFN_vkDestroyPrivateDataSlot) load(userptr, "vkDestroyPrivateDataSlot");
    context->GetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements) load(userptr, "vkGetDeviceBufferMemoryRequirements");
    context->GetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements) load(userptr, "vkGetDeviceImageMemoryRequirements");
    context->GetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements) load(userptr, "vkGetDeviceImageSparseMemoryRequirements");
    context->GetPhysicalDeviceToolProperties = (PFN_vkGetPhysicalDeviceToolProperties) load(userptr, "vkGetPhysicalDeviceToolProperties");
    context->GetPrivateData = (PFN_vkGetPrivateData) load(userptr, "vkGetPrivateData");
    context->QueueSubmit2 = (PFN_vkQueueSubmit2) load(userptr, "vkQueueSubmit2");
    context->SetPrivateData = (PFN_vkSetPrivateData) load(userptr, "vkSetPrivateData");
}
static void glad_vk_load_VK_VERSION_1_4(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->VERSION_1_4) return;
    context->CmdBindDescriptorSets2 = (PFN_vkCmdBindDescriptorSets2) load(userptr, "vkCmdBindDescriptorSets2");
    context->CmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2) load(userptr, "vkCmdBindIndexBuffer2");
    context->CmdPushConstants2 = (PFN_vkCmdPushConstants2) load(userptr, "vkCmdPushConstants2");
    context->CmdPushDescriptorSet = (PFN_vkCmdPushDescriptorSet) load(userptr, "vkCmdPushDescriptorSet");
    context->CmdPushDescriptorSet2 = (PFN_vkCmdPushDescriptorSet2) load(userptr, "vkCmdPushDescriptorSet2");
    context->CmdPushDescriptorSetWithTemplate = (PFN_vkCmdPushDescriptorSetWithTemplate) load(userptr, "vkCmdPushDescriptorSetWithTemplate");
    context->CmdPushDescriptorSetWithTemplate2 = (PFN_vkCmdPushDescriptorSetWithTemplate2) load(userptr, "vkCmdPushDescriptorSetWithTemplate2");
    context->CmdSetLineStipple = (PFN_vkCmdSetLineStipple) load(userptr, "vkCmdSetLineStipple");
    context->CmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations) load(userptr, "vkCmdSetRenderingAttachmentLocations");
    context->CmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices) load(userptr, "vkCmdSetRenderingInputAttachmentIndices");
    context->CopyImageToImage = (PFN_vkCopyImageToImage) load(userptr, "vkCopyImageToImage");
    context->CopyImageToMemory = (PFN_vkCopyImageToMemory) load(userptr, "vkCopyImageToMemory");
    context->CopyMemoryToImage = (PFN_vkCopyMemoryToImage) load(userptr, "vkCopyMemoryToImage");
    context->GetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout) load(userptr, "vkGetDeviceImageSubresourceLayout");
    context->GetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2) load(userptr, "vkGetImageSubresourceLayout2");
    context->GetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity) load(userptr, "vkGetRenderingAreaGranularity");
    context->MapMemory2 = (PFN_vkMapMemory2) load(userptr, "vkMapMemory2");
    context->TransitionImageLayout = (PFN_vkTransitionImageLayout) load(userptr, "vkTransitionImageLayout");
    context->UnmapMemory2 = (PFN_vkUnmapMemory2) load(userptr, "vkUnmapMemory2");
}
#if defined(VK_ENABLE_BETA_EXTENSIONS)
static void glad_vk_load_VK_AMDX_shader_enqueue(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->AMDX_shader_enqueue) return;
    context->CmdDispatchGraphAMDX = (PFN_vkCmdDispatchGraphAMDX) load(userptr, "vkCmdDispatchGraphAMDX");
    context->CmdDispatchGraphIndirectAMDX = (PFN_vkCmdDispatchGraphIndirectAMDX) load(userptr, "vkCmdDispatchGraphIndirectAMDX");
    context->CmdDispatchGraphIndirectCountAMDX = (PFN_vkCmdDispatchGraphIndirectCountAMDX) load(userptr, "vkCmdDispatchGraphIndirectCountAMDX");
    context->CmdInitializeGraphScratchMemoryAMDX = (PFN_vkCmdInitializeGraphScratchMemoryAMDX) load(userptr, "vkCmdInitializeGraphScratchMemoryAMDX");
    context->CreateExecutionGraphPipelinesAMDX = (PFN_vkCreateExecutionGraphPipelinesAMDX) load(userptr, "vkCreateExecutionGraphPipelinesAMDX");
    context->GetExecutionGraphPipelineNodeIndexAMDX = (PFN_vkGetExecutionGraphPipelineNodeIndexAMDX) load(userptr, "vkGetExecutionGraphPipelineNodeIndexAMDX");
    context->GetExecutionGraphPipelineScratchSizeAMDX = (PFN_vkGetExecutionGraphPipelineScratchSizeAMDX) load(userptr, "vkGetExecutionGraphPipelineScratchSizeAMDX");
}

#endif
static void glad_vk_load_VK_AMD_anti_lag(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->AMD_anti_lag) return;
    context->AntiLagUpdateAMD = (PFN_vkAntiLagUpdateAMD) load(userptr, "vkAntiLagUpdateAMD");
}
static void glad_vk_load_VK_AMD_buffer_marker(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->AMD_buffer_marker) return;
    context->CmdWriteBufferMarker2AMD = (PFN_vkCmdWriteBufferMarker2AMD) load(userptr, "vkCmdWriteBufferMarker2AMD");
    context->CmdWriteBufferMarkerAMD = (PFN_vkCmdWriteBufferMarkerAMD) load(userptr, "vkCmdWriteBufferMarkerAMD");
}
static void glad_vk_load_VK_AMD_display_native_hdr(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->AMD_display_native_hdr) return;
    context->SetLocalDimmingAMD = (PFN_vkSetLocalDimmingAMD) load(userptr, "vkSetLocalDimmingAMD");
}
static void glad_vk_load_VK_AMD_draw_indirect_count(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->AMD_draw_indirect_count) return;
    context->CmdDrawIndexedIndirectCountAMD = (PFN_vkCmdDrawIndexedIndirectCountAMD) load(userptr, "vkCmdDrawIndexedIndirectCountAMD");
    context->CmdDrawIndirectCountAMD = (PFN_vkCmdDrawIndirectCountAMD) load(userptr, "vkCmdDrawIndirectCountAMD");
}
static void glad_vk_load_VK_AMD_shader_info(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->AMD_shader_info) return;
    context->GetShaderInfoAMD = (PFN_vkGetShaderInfoAMD) load(userptr, "vkGetShaderInfoAMD");
}
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
static void glad_vk_load_VK_ANDROID_external_memory_android_hardware_buffer(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ANDROID_external_memory_android_hardware_buffer) return;
    context->GetAndroidHardwareBufferPropertiesANDROID = (PFN_vkGetAndroidHardwareBufferPropertiesANDROID) load(userptr, "vkGetAndroidHardwareBufferPropertiesANDROID");
    context->GetMemoryAndroidHardwareBufferANDROID = (PFN_vkGetMemoryAndroidHardwareBufferANDROID) load(userptr, "vkGetMemoryAndroidHardwareBufferANDROID");
}

#endif
static void glad_vk_load_VK_ARM_data_graph(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ARM_data_graph) return;
    context->BindDataGraphPipelineSessionMemoryARM = (PFN_vkBindDataGraphPipelineSessionMemoryARM) load(userptr, "vkBindDataGraphPipelineSessionMemoryARM");
    context->CmdDispatchDataGraphARM = (PFN_vkCmdDispatchDataGraphARM) load(userptr, "vkCmdDispatchDataGraphARM");
    context->CreateDataGraphPipelineSessionARM = (PFN_vkCreateDataGraphPipelineSessionARM) load(userptr, "vkCreateDataGraphPipelineSessionARM");
    context->CreateDataGraphPipelinesARM = (PFN_vkCreateDataGraphPipelinesARM) load(userptr, "vkCreateDataGraphPipelinesARM");
    context->DestroyDataGraphPipelineSessionARM = (PFN_vkDestroyDataGraphPipelineSessionARM) load(userptr, "vkDestroyDataGraphPipelineSessionARM");
    context->GetDataGraphPipelineAvailablePropertiesARM = (PFN_vkGetDataGraphPipelineAvailablePropertiesARM) load(userptr, "vkGetDataGraphPipelineAvailablePropertiesARM");
    context->GetDataGraphPipelinePropertiesARM = (PFN_vkGetDataGraphPipelinePropertiesARM) load(userptr, "vkGetDataGraphPipelinePropertiesARM");
    context->GetDataGraphPipelineSessionBindPointRequirementsARM = (PFN_vkGetDataGraphPipelineSessionBindPointRequirementsARM) load(userptr, "vkGetDataGraphPipelineSessionBindPointRequirementsARM");
    context->GetDataGraphPipelineSessionMemoryRequirementsARM = (PFN_vkGetDataGraphPipelineSessionMemoryRequirementsARM) load(userptr, "vkGetDataGraphPipelineSessionMemoryRequirementsARM");
    context->GetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM) load(userptr, "vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM");
    context->GetPhysicalDeviceQueueFamilyDataGraphPropertiesARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM) load(userptr, "vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM");
}
static void glad_vk_load_VK_ARM_tensors(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ARM_tensors) return;
    context->BindTensorMemoryARM = (PFN_vkBindTensorMemoryARM) load(userptr, "vkBindTensorMemoryARM");
    context->CmdCopyTensorARM = (PFN_vkCmdCopyTensorARM) load(userptr, "vkCmdCopyTensorARM");
    context->CreateTensorARM = (PFN_vkCreateTensorARM) load(userptr, "vkCreateTensorARM");
    context->CreateTensorViewARM = (PFN_vkCreateTensorViewARM) load(userptr, "vkCreateTensorViewARM");
    context->DestroyTensorARM = (PFN_vkDestroyTensorARM) load(userptr, "vkDestroyTensorARM");
    context->DestroyTensorViewARM = (PFN_vkDestroyTensorViewARM) load(userptr, "vkDestroyTensorViewARM");
    context->GetDeviceTensorMemoryRequirementsARM = (PFN_vkGetDeviceTensorMemoryRequirementsARM) load(userptr, "vkGetDeviceTensorMemoryRequirementsARM");
    context->GetPhysicalDeviceExternalTensorPropertiesARM = (PFN_vkGetPhysicalDeviceExternalTensorPropertiesARM) load(userptr, "vkGetPhysicalDeviceExternalTensorPropertiesARM");
    context->GetTensorMemoryRequirementsARM = (PFN_vkGetTensorMemoryRequirementsARM) load(userptr, "vkGetTensorMemoryRequirementsARM");
    context->GetTensorOpaqueCaptureDescriptorDataARM = (PFN_vkGetTensorOpaqueCaptureDescriptorDataARM) load(userptr, "vkGetTensorOpaqueCaptureDescriptorDataARM");
    context->GetTensorViewOpaqueCaptureDescriptorDataARM = (PFN_vkGetTensorViewOpaqueCaptureDescriptorDataARM) load(userptr, "vkGetTensorViewOpaqueCaptureDescriptorDataARM");
}
static void glad_vk_load_VK_EXT_acquire_drm_display(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_acquire_drm_display) return;
    context->AcquireDrmDisplayEXT = (PFN_vkAcquireDrmDisplayEXT) load(userptr, "vkAcquireDrmDisplayEXT");
    context->GetDrmDisplayEXT = (PFN_vkGetDrmDisplayEXT) load(userptr, "vkGetDrmDisplayEXT");
}
#if defined(VK_USE_PLATFORM_XLIB_XRANDR_EXT)
static void glad_vk_load_VK_EXT_acquire_xlib_display(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_acquire_xlib_display) return;
    context->AcquireXlibDisplayEXT = (PFN_vkAcquireXlibDisplayEXT) load(userptr, "vkAcquireXlibDisplayEXT");
    context->GetRandROutputDisplayEXT = (PFN_vkGetRandROutputDisplayEXT) load(userptr, "vkGetRandROutputDisplayEXT");
}

#endif
static void glad_vk_load_VK_EXT_attachment_feedback_loop_dynamic_state(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_attachment_feedback_loop_dynamic_state) return;
    context->CmdSetAttachmentFeedbackLoopEnableEXT = (PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT) load(userptr, "vkCmdSetAttachmentFeedbackLoopEnableEXT");
}
static void glad_vk_load_VK_EXT_buffer_device_address(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_buffer_device_address) return;
    context->GetBufferDeviceAddressEXT = (PFN_vkGetBufferDeviceAddressEXT) load(userptr, "vkGetBufferDeviceAddressEXT");
}
static void glad_vk_load_VK_EXT_calibrated_timestamps(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_calibrated_timestamps) return;
    context->GetCalibratedTimestampsEXT = (PFN_vkGetCalibratedTimestampsEXT) load(userptr, "vkGetCalibratedTimestampsEXT");
    context->GetPhysicalDeviceCalibrateableTimeDomainsEXT = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT) load(userptr, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT");
}
static void glad_vk_load_VK_EXT_color_write_enable(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_color_write_enable) return;
    context->CmdSetColorWriteEnableEXT = (PFN_vkCmdSetColorWriteEnableEXT) load(userptr, "vkCmdSetColorWriteEnableEXT");
}
static void glad_vk_load_VK_EXT_conditional_rendering(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_conditional_rendering) return;
    context->CmdBeginConditionalRenderingEXT = (PFN_vkCmdBeginConditionalRenderingEXT) load(userptr, "vkCmdBeginConditionalRenderingEXT");
    context->CmdEndConditionalRenderingEXT = (PFN_vkCmdEndConditionalRenderingEXT) load(userptr, "vkCmdEndConditionalRenderingEXT");
}
static void glad_vk_load_VK_EXT_debug_marker(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_debug_marker) return;
    context->CmdDebugMarkerBeginEXT = (PFN_vkCmdDebugMarkerBeginEXT) load(userptr, "vkCmdDebugMarkerBeginEXT");
    context->CmdDebugMarkerEndEXT = (PFN_vkCmdDebugMarkerEndEXT) load(userptr, "vkCmdDebugMarkerEndEXT");
    context->CmdDebugMarkerInsertEXT = (PFN_vkCmdDebugMarkerInsertEXT) load(userptr, "vkCmdDebugMarkerInsertEXT");
    context->DebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT) load(userptr, "vkDebugMarkerSetObjectNameEXT");
    context->DebugMarkerSetObjectTagEXT = (PFN_vkDebugMarkerSetObjectTagEXT) load(userptr, "vkDebugMarkerSetObjectTagEXT");
}
static void glad_vk_load_VK_EXT_debug_report(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_debug_report) return;
    context->CreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT) load(userptr, "vkCreateDebugReportCallbackEXT");
    context->DebugReportMessageEXT = (PFN_vkDebugReportMessageEXT) load(userptr, "vkDebugReportMessageEXT");
    context->DestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) load(userptr, "vkDestroyDebugReportCallbackEXT");
}
static void glad_vk_load_VK_EXT_debug_utils(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_debug_utils) return;
    context->CmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT) load(userptr, "vkCmdBeginDebugUtilsLabelEXT");
    context->CmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT) load(userptr, "vkCmdEndDebugUtilsLabelEXT");
    context->CmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT) load(userptr, "vkCmdInsertDebugUtilsLabelEXT");
    context->CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) load(userptr, "vkCreateDebugUtilsMessengerEXT");
    context->DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) load(userptr, "vkDestroyDebugUtilsMessengerEXT");
    context->QueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT) load(userptr, "vkQueueBeginDebugUtilsLabelEXT");
    context->QueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT) load(userptr, "vkQueueEndDebugUtilsLabelEXT");
    context->QueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT) load(userptr, "vkQueueInsertDebugUtilsLabelEXT");
    context->SetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT) load(userptr, "vkSetDebugUtilsObjectNameEXT");
    context->SetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT) load(userptr, "vkSetDebugUtilsObjectTagEXT");
    context->SubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT) load(userptr, "vkSubmitDebugUtilsMessageEXT");
}
static void glad_vk_load_VK_EXT_depth_bias_control(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_depth_bias_control) return;
    context->CmdSetDepthBias2EXT = (PFN_vkCmdSetDepthBias2EXT) load(userptr, "vkCmdSetDepthBias2EXT");
}
static void glad_vk_load_VK_EXT_depth_clamp_control(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_depth_clamp_control) return;
    context->CmdSetDepthClampRangeEXT = (PFN_vkCmdSetDepthClampRangeEXT) load(userptr, "vkCmdSetDepthClampRangeEXT");
}
static void glad_vk_load_VK_EXT_descriptor_buffer(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_descriptor_buffer) return;
    context->CmdBindDescriptorBufferEmbeddedSamplersEXT = (PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT) load(userptr, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT");
    context->CmdBindDescriptorBuffersEXT = (PFN_vkCmdBindDescriptorBuffersEXT) load(userptr, "vkCmdBindDescriptorBuffersEXT");
    context->CmdSetDescriptorBufferOffsetsEXT = (PFN_vkCmdSetDescriptorBufferOffsetsEXT) load(userptr, "vkCmdSetDescriptorBufferOffsetsEXT");
    context->GetAccelerationStructureOpaqueCaptureDescriptorDataEXT = (PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT) load(userptr, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT");
    context->GetBufferOpaqueCaptureDescriptorDataEXT = (PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT) load(userptr, "vkGetBufferOpaqueCaptureDescriptorDataEXT");
    context->GetDescriptorEXT = (PFN_vkGetDescriptorEXT) load(userptr, "vkGetDescriptorEXT");
    context->GetDescriptorSetLayoutBindingOffsetEXT = (PFN_vkGetDescriptorSetLayoutBindingOffsetEXT) load(userptr, "vkGetDescriptorSetLayoutBindingOffsetEXT");
    context->GetDescriptorSetLayoutSizeEXT = (PFN_vkGetDescriptorSetLayoutSizeEXT) load(userptr, "vkGetDescriptorSetLayoutSizeEXT");
    context->GetImageOpaqueCaptureDescriptorDataEXT = (PFN_vkGetImageOpaqueCaptureDescriptorDataEXT) load(userptr, "vkGetImageOpaqueCaptureDescriptorDataEXT");
    context->GetImageViewOpaqueCaptureDescriptorDataEXT = (PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT) load(userptr, "vkGetImageViewOpaqueCaptureDescriptorDataEXT");
    context->GetSamplerOpaqueCaptureDescriptorDataEXT = (PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT) load(userptr, "vkGetSamplerOpaqueCaptureDescriptorDataEXT");
}
static void glad_vk_load_VK_EXT_device_fault(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_device_fault) return;
    context->GetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT) load(userptr, "vkGetDeviceFaultInfoEXT");
}
static void glad_vk_load_VK_EXT_device_generated_commands(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_device_generated_commands) return;
    context->CmdExecuteGeneratedCommandsEXT = (PFN_vkCmdExecuteGeneratedCommandsEXT) load(userptr, "vkCmdExecuteGeneratedCommandsEXT");
    context->CmdPreprocessGeneratedCommandsEXT = (PFN_vkCmdPreprocessGeneratedCommandsEXT) load(userptr, "vkCmdPreprocessGeneratedCommandsEXT");
    context->CreateIndirectCommandsLayoutEXT = (PFN_vkCreateIndirectCommandsLayoutEXT) load(userptr, "vkCreateIndirectCommandsLayoutEXT");
    context->CreateIndirectExecutionSetEXT = (PFN_vkCreateIndirectExecutionSetEXT) load(userptr, "vkCreateIndirectExecutionSetEXT");
    context->DestroyIndirectCommandsLayoutEXT = (PFN_vkDestroyIndirectCommandsLayoutEXT) load(userptr, "vkDestroyIndirectCommandsLayoutEXT");
    context->DestroyIndirectExecutionSetEXT = (PFN_vkDestroyIndirectExecutionSetEXT) load(userptr, "vkDestroyIndirectExecutionSetEXT");
    context->GetGeneratedCommandsMemoryRequirementsEXT = (PFN_vkGetGeneratedCommandsMemoryRequirementsEXT) load(userptr, "vkGetGeneratedCommandsMemoryRequirementsEXT");
    context->UpdateIndirectExecutionSetPipelineEXT = (PFN_vkUpdateIndirectExecutionSetPipelineEXT) load(userptr, "vkUpdateIndirectExecutionSetPipelineEXT");
    context->UpdateIndirectExecutionSetShaderEXT = (PFN_vkUpdateIndirectExecutionSetShaderEXT) load(userptr, "vkUpdateIndirectExecutionSetShaderEXT");
}
static void glad_vk_load_VK_EXT_direct_mode_display(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_direct_mode_display) return;
    context->ReleaseDisplayEXT = (PFN_vkReleaseDisplayEXT) load(userptr, "vkReleaseDisplayEXT");
}
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
static void glad_vk_load_VK_EXT_directfb_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_directfb_surface) return;
    context->CreateDirectFBSurfaceEXT = (PFN_vkCreateDirectFBSurfaceEXT) load(userptr, "vkCreateDirectFBSurfaceEXT");
    context->GetPhysicalDeviceDirectFBPresentationSupportEXT = (PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT) load(userptr, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT");
}

#endif
static void glad_vk_load_VK_EXT_discard_rectangles(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_discard_rectangles) return;
    context->CmdSetDiscardRectangleEXT = (PFN_vkCmdSetDiscardRectangleEXT) load(userptr, "vkCmdSetDiscardRectangleEXT");
    context->CmdSetDiscardRectangleEnableEXT = (PFN_vkCmdSetDiscardRectangleEnableEXT) load(userptr, "vkCmdSetDiscardRectangleEnableEXT");
    context->CmdSetDiscardRectangleModeEXT = (PFN_vkCmdSetDiscardRectangleModeEXT) load(userptr, "vkCmdSetDiscardRectangleModeEXT");
}
static void glad_vk_load_VK_EXT_display_control(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_display_control) return;
    context->DisplayPowerControlEXT = (PFN_vkDisplayPowerControlEXT) load(userptr, "vkDisplayPowerControlEXT");
    context->GetSwapchainCounterEXT = (PFN_vkGetSwapchainCounterEXT) load(userptr, "vkGetSwapchainCounterEXT");
    context->RegisterDeviceEventEXT = (PFN_vkRegisterDeviceEventEXT) load(userptr, "vkRegisterDeviceEventEXT");
    context->RegisterDisplayEventEXT = (PFN_vkRegisterDisplayEventEXT) load(userptr, "vkRegisterDisplayEventEXT");
}
static void glad_vk_load_VK_EXT_display_surface_counter(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_display_surface_counter) return;
    context->GetPhysicalDeviceSurfaceCapabilities2EXT = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT) load(userptr, "vkGetPhysicalDeviceSurfaceCapabilities2EXT");
}
static void glad_vk_load_VK_EXT_extended_dynamic_state(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_extended_dynamic_state) return;
    context->CmdBindVertexBuffers2EXT = (PFN_vkCmdBindVertexBuffers2EXT) load(userptr, "vkCmdBindVertexBuffers2EXT");
    context->CmdSetCullModeEXT = (PFN_vkCmdSetCullModeEXT) load(userptr, "vkCmdSetCullModeEXT");
    context->CmdSetDepthBoundsTestEnableEXT = (PFN_vkCmdSetDepthBoundsTestEnableEXT) load(userptr, "vkCmdSetDepthBoundsTestEnableEXT");
    context->CmdSetDepthCompareOpEXT = (PFN_vkCmdSetDepthCompareOpEXT) load(userptr, "vkCmdSetDepthCompareOpEXT");
    context->CmdSetDepthTestEnableEXT = (PFN_vkCmdSetDepthTestEnableEXT) load(userptr, "vkCmdSetDepthTestEnableEXT");
    context->CmdSetDepthWriteEnableEXT = (PFN_vkCmdSetDepthWriteEnableEXT) load(userptr, "vkCmdSetDepthWriteEnableEXT");
    context->CmdSetFrontFaceEXT = (PFN_vkCmdSetFrontFaceEXT) load(userptr, "vkCmdSetFrontFaceEXT");
    context->CmdSetPrimitiveTopologyEXT = (PFN_vkCmdSetPrimitiveTopologyEXT) load(userptr, "vkCmdSetPrimitiveTopologyEXT");
    context->CmdSetScissorWithCountEXT = (PFN_vkCmdSetScissorWithCountEXT) load(userptr, "vkCmdSetScissorWithCountEXT");
    context->CmdSetStencilOpEXT = (PFN_vkCmdSetStencilOpEXT) load(userptr, "vkCmdSetStencilOpEXT");
    context->CmdSetStencilTestEnableEXT = (PFN_vkCmdSetStencilTestEnableEXT) load(userptr, "vkCmdSetStencilTestEnableEXT");
    context->CmdSetViewportWithCountEXT = (PFN_vkCmdSetViewportWithCountEXT) load(userptr, "vkCmdSetViewportWithCountEXT");
}
static void glad_vk_load_VK_EXT_extended_dynamic_state2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_extended_dynamic_state2) return;
    context->CmdSetDepthBiasEnableEXT = (PFN_vkCmdSetDepthBiasEnableEXT) load(userptr, "vkCmdSetDepthBiasEnableEXT");
    context->CmdSetLogicOpEXT = (PFN_vkCmdSetLogicOpEXT) load(userptr, "vkCmdSetLogicOpEXT");
    context->CmdSetPatchControlPointsEXT = (PFN_vkCmdSetPatchControlPointsEXT) load(userptr, "vkCmdSetPatchControlPointsEXT");
    context->CmdSetPrimitiveRestartEnableEXT = (PFN_vkCmdSetPrimitiveRestartEnableEXT) load(userptr, "vkCmdSetPrimitiveRestartEnableEXT");
    context->CmdSetRasterizerDiscardEnableEXT = (PFN_vkCmdSetRasterizerDiscardEnableEXT) load(userptr, "vkCmdSetRasterizerDiscardEnableEXT");
}
static void glad_vk_load_VK_EXT_extended_dynamic_state3(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_extended_dynamic_state3) return;
    context->CmdSetAlphaToCoverageEnableEXT = (PFN_vkCmdSetAlphaToCoverageEnableEXT) load(userptr, "vkCmdSetAlphaToCoverageEnableEXT");
    context->CmdSetAlphaToOneEnableEXT = (PFN_vkCmdSetAlphaToOneEnableEXT) load(userptr, "vkCmdSetAlphaToOneEnableEXT");
    context->CmdSetColorBlendAdvancedEXT = (PFN_vkCmdSetColorBlendAdvancedEXT) load(userptr, "vkCmdSetColorBlendAdvancedEXT");
    context->CmdSetColorBlendEnableEXT = (PFN_vkCmdSetColorBlendEnableEXT) load(userptr, "vkCmdSetColorBlendEnableEXT");
    context->CmdSetColorBlendEquationEXT = (PFN_vkCmdSetColorBlendEquationEXT) load(userptr, "vkCmdSetColorBlendEquationEXT");
    context->CmdSetColorWriteMaskEXT = (PFN_vkCmdSetColorWriteMaskEXT) load(userptr, "vkCmdSetColorWriteMaskEXT");
    context->CmdSetConservativeRasterizationModeEXT = (PFN_vkCmdSetConservativeRasterizationModeEXT) load(userptr, "vkCmdSetConservativeRasterizationModeEXT");
    context->CmdSetCoverageModulationModeNV = (PFN_vkCmdSetCoverageModulationModeNV) load(userptr, "vkCmdSetCoverageModulationModeNV");
    context->CmdSetCoverageModulationTableEnableNV = (PFN_vkCmdSetCoverageModulationTableEnableNV) load(userptr, "vkCmdSetCoverageModulationTableEnableNV");
    context->CmdSetCoverageModulationTableNV = (PFN_vkCmdSetCoverageModulationTableNV) load(userptr, "vkCmdSetCoverageModulationTableNV");
    context->CmdSetCoverageReductionModeNV = (PFN_vkCmdSetCoverageReductionModeNV) load(userptr, "vkCmdSetCoverageReductionModeNV");
    context->CmdSetCoverageToColorEnableNV = (PFN_vkCmdSetCoverageToColorEnableNV) load(userptr, "vkCmdSetCoverageToColorEnableNV");
    context->CmdSetCoverageToColorLocationNV = (PFN_vkCmdSetCoverageToColorLocationNV) load(userptr, "vkCmdSetCoverageToColorLocationNV");
    context->CmdSetDepthClampEnableEXT = (PFN_vkCmdSetDepthClampEnableEXT) load(userptr, "vkCmdSetDepthClampEnableEXT");
    context->CmdSetDepthClipEnableEXT = (PFN_vkCmdSetDepthClipEnableEXT) load(userptr, "vkCmdSetDepthClipEnableEXT");
    context->CmdSetDepthClipNegativeOneToOneEXT = (PFN_vkCmdSetDepthClipNegativeOneToOneEXT) load(userptr, "vkCmdSetDepthClipNegativeOneToOneEXT");
    context->CmdSetExtraPrimitiveOverestimationSizeEXT = (PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT) load(userptr, "vkCmdSetExtraPrimitiveOverestimationSizeEXT");
    context->CmdSetLineRasterizationModeEXT = (PFN_vkCmdSetLineRasterizationModeEXT) load(userptr, "vkCmdSetLineRasterizationModeEXT");
    context->CmdSetLineStippleEnableEXT = (PFN_vkCmdSetLineStippleEnableEXT) load(userptr, "vkCmdSetLineStippleEnableEXT");
    context->CmdSetLogicOpEnableEXT = (PFN_vkCmdSetLogicOpEnableEXT) load(userptr, "vkCmdSetLogicOpEnableEXT");
    context->CmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT) load(userptr, "vkCmdSetPolygonModeEXT");
    context->CmdSetProvokingVertexModeEXT = (PFN_vkCmdSetProvokingVertexModeEXT) load(userptr, "vkCmdSetProvokingVertexModeEXT");
    context->CmdSetRasterizationSamplesEXT = (PFN_vkCmdSetRasterizationSamplesEXT) load(userptr, "vkCmdSetRasterizationSamplesEXT");
    context->CmdSetRasterizationStreamEXT = (PFN_vkCmdSetRasterizationStreamEXT) load(userptr, "vkCmdSetRasterizationStreamEXT");
    context->CmdSetRepresentativeFragmentTestEnableNV = (PFN_vkCmdSetRepresentativeFragmentTestEnableNV) load(userptr, "vkCmdSetRepresentativeFragmentTestEnableNV");
    context->CmdSetSampleLocationsEnableEXT = (PFN_vkCmdSetSampleLocationsEnableEXT) load(userptr, "vkCmdSetSampleLocationsEnableEXT");
    context->CmdSetSampleMaskEXT = (PFN_vkCmdSetSampleMaskEXT) load(userptr, "vkCmdSetSampleMaskEXT");
    context->CmdSetShadingRateImageEnableNV = (PFN_vkCmdSetShadingRateImageEnableNV) load(userptr, "vkCmdSetShadingRateImageEnableNV");
    context->CmdSetTessellationDomainOriginEXT = (PFN_vkCmdSetTessellationDomainOriginEXT) load(userptr, "vkCmdSetTessellationDomainOriginEXT");
    context->CmdSetViewportSwizzleNV = (PFN_vkCmdSetViewportSwizzleNV) load(userptr, "vkCmdSetViewportSwizzleNV");
    context->CmdSetViewportWScalingEnableNV = (PFN_vkCmdSetViewportWScalingEnableNV) load(userptr, "vkCmdSetViewportWScalingEnableNV");
}
static void glad_vk_load_VK_EXT_external_memory_host(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_external_memory_host) return;
    context->GetMemoryHostPointerPropertiesEXT = (PFN_vkGetMemoryHostPointerPropertiesEXT) load(userptr, "vkGetMemoryHostPointerPropertiesEXT");
}
#if defined(VK_USE_PLATFORM_METAL_EXT)
static void glad_vk_load_VK_EXT_external_memory_metal(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_external_memory_metal) return;
    context->GetMemoryMetalHandleEXT = (PFN_vkGetMemoryMetalHandleEXT) load(userptr, "vkGetMemoryMetalHandleEXT");
    context->GetMemoryMetalHandlePropertiesEXT = (PFN_vkGetMemoryMetalHandlePropertiesEXT) load(userptr, "vkGetMemoryMetalHandlePropertiesEXT");
}

#endif
static void glad_vk_load_VK_EXT_fragment_density_map_offset(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_fragment_density_map_offset) return;
    context->CmdEndRendering2EXT = (PFN_vkCmdEndRendering2EXT) load(userptr, "vkCmdEndRendering2EXT");
}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
static void glad_vk_load_VK_EXT_full_screen_exclusive(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_full_screen_exclusive) return;
    context->AcquireFullScreenExclusiveModeEXT = (PFN_vkAcquireFullScreenExclusiveModeEXT) load(userptr, "vkAcquireFullScreenExclusiveModeEXT");
    context->GetDeviceGroupSurfacePresentModes2EXT = (PFN_vkGetDeviceGroupSurfacePresentModes2EXT) load(userptr, "vkGetDeviceGroupSurfacePresentModes2EXT");
    context->GetPhysicalDeviceSurfacePresentModes2EXT = (PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT) load(userptr, "vkGetPhysicalDeviceSurfacePresentModes2EXT");
    context->ReleaseFullScreenExclusiveModeEXT = (PFN_vkReleaseFullScreenExclusiveModeEXT) load(userptr, "vkReleaseFullScreenExclusiveModeEXT");
}

#endif
static void glad_vk_load_VK_EXT_hdr_metadata(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_hdr_metadata) return;
    context->SetHdrMetadataEXT = (PFN_vkSetHdrMetadataEXT) load(userptr, "vkSetHdrMetadataEXT");
}
static void glad_vk_load_VK_EXT_headless_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_headless_surface) return;
    context->CreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT) load(userptr, "vkCreateHeadlessSurfaceEXT");
}
static void glad_vk_load_VK_EXT_host_image_copy(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_host_image_copy) return;
    context->CopyImageToImageEXT = (PFN_vkCopyImageToImageEXT) load(userptr, "vkCopyImageToImageEXT");
    context->CopyImageToMemoryEXT = (PFN_vkCopyImageToMemoryEXT) load(userptr, "vkCopyImageToMemoryEXT");
    context->CopyMemoryToImageEXT = (PFN_vkCopyMemoryToImageEXT) load(userptr, "vkCopyMemoryToImageEXT");
    context->GetImageSubresourceLayout2EXT = (PFN_vkGetImageSubresourceLayout2EXT) load(userptr, "vkGetImageSubresourceLayout2EXT");
    context->TransitionImageLayoutEXT = (PFN_vkTransitionImageLayoutEXT) load(userptr, "vkTransitionImageLayoutEXT");
}
static void glad_vk_load_VK_EXT_host_query_reset(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_host_query_reset) return;
    context->ResetQueryPoolEXT = (PFN_vkResetQueryPoolEXT) load(userptr, "vkResetQueryPoolEXT");
}
static void glad_vk_load_VK_EXT_image_compression_control(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_image_compression_control) return;
    context->GetImageSubresourceLayout2EXT = (PFN_vkGetImageSubresourceLayout2EXT) load(userptr, "vkGetImageSubresourceLayout2EXT");
}
static void glad_vk_load_VK_EXT_image_drm_format_modifier(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_image_drm_format_modifier) return;
    context->GetImageDrmFormatModifierPropertiesEXT = (PFN_vkGetImageDrmFormatModifierPropertiesEXT) load(userptr, "vkGetImageDrmFormatModifierPropertiesEXT");
}
static void glad_vk_load_VK_EXT_line_rasterization(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_line_rasterization) return;
    context->CmdSetLineStippleEXT = (PFN_vkCmdSetLineStippleEXT) load(userptr, "vkCmdSetLineStippleEXT");
}
static void glad_vk_load_VK_EXT_mesh_shader(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_mesh_shader) return;
    context->CmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT) load(userptr, "vkCmdDrawMeshTasksEXT");
    context->CmdDrawMeshTasksIndirectCountEXT = (PFN_vkCmdDrawMeshTasksIndirectCountEXT) load(userptr, "vkCmdDrawMeshTasksIndirectCountEXT");
    context->CmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT) load(userptr, "vkCmdDrawMeshTasksIndirectEXT");
}
#if defined(VK_USE_PLATFORM_METAL_EXT)
static void glad_vk_load_VK_EXT_metal_objects(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_metal_objects) return;
    context->ExportMetalObjectsEXT = (PFN_vkExportMetalObjectsEXT) load(userptr, "vkExportMetalObjectsEXT");
}

#endif
#if defined(VK_USE_PLATFORM_METAL_EXT)
static void glad_vk_load_VK_EXT_metal_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_metal_surface) return;
    context->CreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT) load(userptr, "vkCreateMetalSurfaceEXT");
}

#endif
static void glad_vk_load_VK_EXT_multi_draw(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_multi_draw) return;
    context->CmdDrawMultiEXT = (PFN_vkCmdDrawMultiEXT) load(userptr, "vkCmdDrawMultiEXT");
    context->CmdDrawMultiIndexedEXT = (PFN_vkCmdDrawMultiIndexedEXT) load(userptr, "vkCmdDrawMultiIndexedEXT");
}
static void glad_vk_load_VK_EXT_opacity_micromap(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_opacity_micromap) return;
    context->BuildMicromapsEXT = (PFN_vkBuildMicromapsEXT) load(userptr, "vkBuildMicromapsEXT");
    context->CmdBuildMicromapsEXT = (PFN_vkCmdBuildMicromapsEXT) load(userptr, "vkCmdBuildMicromapsEXT");
    context->CmdCopyMemoryToMicromapEXT = (PFN_vkCmdCopyMemoryToMicromapEXT) load(userptr, "vkCmdCopyMemoryToMicromapEXT");
    context->CmdCopyMicromapEXT = (PFN_vkCmdCopyMicromapEXT) load(userptr, "vkCmdCopyMicromapEXT");
    context->CmdCopyMicromapToMemoryEXT = (PFN_vkCmdCopyMicromapToMemoryEXT) load(userptr, "vkCmdCopyMicromapToMemoryEXT");
    context->CmdWriteMicromapsPropertiesEXT = (PFN_vkCmdWriteMicromapsPropertiesEXT) load(userptr, "vkCmdWriteMicromapsPropertiesEXT");
    context->CopyMemoryToMicromapEXT = (PFN_vkCopyMemoryToMicromapEXT) load(userptr, "vkCopyMemoryToMicromapEXT");
    context->CopyMicromapEXT = (PFN_vkCopyMicromapEXT) load(userptr, "vkCopyMicromapEXT");
    context->CopyMicromapToMemoryEXT = (PFN_vkCopyMicromapToMemoryEXT) load(userptr, "vkCopyMicromapToMemoryEXT");
    context->CreateMicromapEXT = (PFN_vkCreateMicromapEXT) load(userptr, "vkCreateMicromapEXT");
    context->DestroyMicromapEXT = (PFN_vkDestroyMicromapEXT) load(userptr, "vkDestroyMicromapEXT");
    context->GetDeviceMicromapCompatibilityEXT = (PFN_vkGetDeviceMicromapCompatibilityEXT) load(userptr, "vkGetDeviceMicromapCompatibilityEXT");
    context->GetMicromapBuildSizesEXT = (PFN_vkGetMicromapBuildSizesEXT) load(userptr, "vkGetMicromapBuildSizesEXT");
    context->WriteMicromapsPropertiesEXT = (PFN_vkWriteMicromapsPropertiesEXT) load(userptr, "vkWriteMicromapsPropertiesEXT");
}
static void glad_vk_load_VK_EXT_pageable_device_local_memory(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_pageable_device_local_memory) return;
    context->SetDeviceMemoryPriorityEXT = (PFN_vkSetDeviceMemoryPriorityEXT) load(userptr, "vkSetDeviceMemoryPriorityEXT");
}
static void glad_vk_load_VK_EXT_pipeline_properties(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_pipeline_properties) return;
    context->GetPipelinePropertiesEXT = (PFN_vkGetPipelinePropertiesEXT) load(userptr, "vkGetPipelinePropertiesEXT");
}
static void glad_vk_load_VK_EXT_private_data(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_private_data) return;
    context->CreatePrivateDataSlotEXT = (PFN_vkCreatePrivateDataSlotEXT) load(userptr, "vkCreatePrivateDataSlotEXT");
    context->DestroyPrivateDataSlotEXT = (PFN_vkDestroyPrivateDataSlotEXT) load(userptr, "vkDestroyPrivateDataSlotEXT");
    context->GetPrivateDataEXT = (PFN_vkGetPrivateDataEXT) load(userptr, "vkGetPrivateDataEXT");
    context->SetPrivateDataEXT = (PFN_vkSetPrivateDataEXT) load(userptr, "vkSetPrivateDataEXT");
}
static void glad_vk_load_VK_EXT_sample_locations(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_sample_locations) return;
    context->CmdSetSampleLocationsEXT = (PFN_vkCmdSetSampleLocationsEXT) load(userptr, "vkCmdSetSampleLocationsEXT");
    context->GetPhysicalDeviceMultisamplePropertiesEXT = (PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT) load(userptr, "vkGetPhysicalDeviceMultisamplePropertiesEXT");
}
static void glad_vk_load_VK_EXT_shader_module_identifier(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_shader_module_identifier) return;
    context->GetShaderModuleCreateInfoIdentifierEXT = (PFN_vkGetShaderModuleCreateInfoIdentifierEXT) load(userptr, "vkGetShaderModuleCreateInfoIdentifierEXT");
    context->GetShaderModuleIdentifierEXT = (PFN_vkGetShaderModuleIdentifierEXT) load(userptr, "vkGetShaderModuleIdentifierEXT");
}
static void glad_vk_load_VK_EXT_shader_object(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_shader_object) return;
    context->CmdBindShadersEXT = (PFN_vkCmdBindShadersEXT) load(userptr, "vkCmdBindShadersEXT");
    context->CmdBindVertexBuffers2EXT = (PFN_vkCmdBindVertexBuffers2EXT) load(userptr, "vkCmdBindVertexBuffers2EXT");
    context->CmdSetAlphaToCoverageEnableEXT = (PFN_vkCmdSetAlphaToCoverageEnableEXT) load(userptr, "vkCmdSetAlphaToCoverageEnableEXT");
    context->CmdSetAlphaToOneEnableEXT = (PFN_vkCmdSetAlphaToOneEnableEXT) load(userptr, "vkCmdSetAlphaToOneEnableEXT");
    context->CmdSetColorBlendAdvancedEXT = (PFN_vkCmdSetColorBlendAdvancedEXT) load(userptr, "vkCmdSetColorBlendAdvancedEXT");
    context->CmdSetColorBlendEnableEXT = (PFN_vkCmdSetColorBlendEnableEXT) load(userptr, "vkCmdSetColorBlendEnableEXT");
    context->CmdSetColorBlendEquationEXT = (PFN_vkCmdSetColorBlendEquationEXT) load(userptr, "vkCmdSetColorBlendEquationEXT");
    context->CmdSetColorWriteMaskEXT = (PFN_vkCmdSetColorWriteMaskEXT) load(userptr, "vkCmdSetColorWriteMaskEXT");
    context->CmdSetConservativeRasterizationModeEXT = (PFN_vkCmdSetConservativeRasterizationModeEXT) load(userptr, "vkCmdSetConservativeRasterizationModeEXT");
    context->CmdSetCoverageModulationModeNV = (PFN_vkCmdSetCoverageModulationModeNV) load(userptr, "vkCmdSetCoverageModulationModeNV");
    context->CmdSetCoverageModulationTableEnableNV = (PFN_vkCmdSetCoverageModulationTableEnableNV) load(userptr, "vkCmdSetCoverageModulationTableEnableNV");
    context->CmdSetCoverageModulationTableNV = (PFN_vkCmdSetCoverageModulationTableNV) load(userptr, "vkCmdSetCoverageModulationTableNV");
    context->CmdSetCoverageReductionModeNV = (PFN_vkCmdSetCoverageReductionModeNV) load(userptr, "vkCmdSetCoverageReductionModeNV");
    context->CmdSetCoverageToColorEnableNV = (PFN_vkCmdSetCoverageToColorEnableNV) load(userptr, "vkCmdSetCoverageToColorEnableNV");
    context->CmdSetCoverageToColorLocationNV = (PFN_vkCmdSetCoverageToColorLocationNV) load(userptr, "vkCmdSetCoverageToColorLocationNV");
    context->CmdSetCullModeEXT = (PFN_vkCmdSetCullModeEXT) load(userptr, "vkCmdSetCullModeEXT");
    context->CmdSetDepthBiasEnableEXT = (PFN_vkCmdSetDepthBiasEnableEXT) load(userptr, "vkCmdSetDepthBiasEnableEXT");
    context->CmdSetDepthBoundsTestEnableEXT = (PFN_vkCmdSetDepthBoundsTestEnableEXT) load(userptr, "vkCmdSetDepthBoundsTestEnableEXT");
    context->CmdSetDepthClampEnableEXT = (PFN_vkCmdSetDepthClampEnableEXT) load(userptr, "vkCmdSetDepthClampEnableEXT");
    context->CmdSetDepthClampRangeEXT = (PFN_vkCmdSetDepthClampRangeEXT) load(userptr, "vkCmdSetDepthClampRangeEXT");
    context->CmdSetDepthClipEnableEXT = (PFN_vkCmdSetDepthClipEnableEXT) load(userptr, "vkCmdSetDepthClipEnableEXT");
    context->CmdSetDepthClipNegativeOneToOneEXT = (PFN_vkCmdSetDepthClipNegativeOneToOneEXT) load(userptr, "vkCmdSetDepthClipNegativeOneToOneEXT");
    context->CmdSetDepthCompareOpEXT = (PFN_vkCmdSetDepthCompareOpEXT) load(userptr, "vkCmdSetDepthCompareOpEXT");
    context->CmdSetDepthTestEnableEXT = (PFN_vkCmdSetDepthTestEnableEXT) load(userptr, "vkCmdSetDepthTestEnableEXT");
    context->CmdSetDepthWriteEnableEXT = (PFN_vkCmdSetDepthWriteEnableEXT) load(userptr, "vkCmdSetDepthWriteEnableEXT");
    context->CmdSetExtraPrimitiveOverestimationSizeEXT = (PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT) load(userptr, "vkCmdSetExtraPrimitiveOverestimationSizeEXT");
    context->CmdSetFrontFaceEXT = (PFN_vkCmdSetFrontFaceEXT) load(userptr, "vkCmdSetFrontFaceEXT");
    context->CmdSetLineRasterizationModeEXT = (PFN_vkCmdSetLineRasterizationModeEXT) load(userptr, "vkCmdSetLineRasterizationModeEXT");
    context->CmdSetLineStippleEnableEXT = (PFN_vkCmdSetLineStippleEnableEXT) load(userptr, "vkCmdSetLineStippleEnableEXT");
    context->CmdSetLogicOpEXT = (PFN_vkCmdSetLogicOpEXT) load(userptr, "vkCmdSetLogicOpEXT");
    context->CmdSetLogicOpEnableEXT = (PFN_vkCmdSetLogicOpEnableEXT) load(userptr, "vkCmdSetLogicOpEnableEXT");
    context->CmdSetPatchControlPointsEXT = (PFN_vkCmdSetPatchControlPointsEXT) load(userptr, "vkCmdSetPatchControlPointsEXT");
    context->CmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT) load(userptr, "vkCmdSetPolygonModeEXT");
    context->CmdSetPrimitiveRestartEnableEXT = (PFN_vkCmdSetPrimitiveRestartEnableEXT) load(userptr, "vkCmdSetPrimitiveRestartEnableEXT");
    context->CmdSetPrimitiveTopologyEXT = (PFN_vkCmdSetPrimitiveTopologyEXT) load(userptr, "vkCmdSetPrimitiveTopologyEXT");
    context->CmdSetProvokingVertexModeEXT = (PFN_vkCmdSetProvokingVertexModeEXT) load(userptr, "vkCmdSetProvokingVertexModeEXT");
    context->CmdSetRasterizationSamplesEXT = (PFN_vkCmdSetRasterizationSamplesEXT) load(userptr, "vkCmdSetRasterizationSamplesEXT");
    context->CmdSetRasterizationStreamEXT = (PFN_vkCmdSetRasterizationStreamEXT) load(userptr, "vkCmdSetRasterizationStreamEXT");
    context->CmdSetRasterizerDiscardEnableEXT = (PFN_vkCmdSetRasterizerDiscardEnableEXT) load(userptr, "vkCmdSetRasterizerDiscardEnableEXT");
    context->CmdSetRepresentativeFragmentTestEnableNV = (PFN_vkCmdSetRepresentativeFragmentTestEnableNV) load(userptr, "vkCmdSetRepresentativeFragmentTestEnableNV");
    context->CmdSetSampleLocationsEnableEXT = (PFN_vkCmdSetSampleLocationsEnableEXT) load(userptr, "vkCmdSetSampleLocationsEnableEXT");
    context->CmdSetSampleMaskEXT = (PFN_vkCmdSetSampleMaskEXT) load(userptr, "vkCmdSetSampleMaskEXT");
    context->CmdSetScissorWithCountEXT = (PFN_vkCmdSetScissorWithCountEXT) load(userptr, "vkCmdSetScissorWithCountEXT");
    context->CmdSetShadingRateImageEnableNV = (PFN_vkCmdSetShadingRateImageEnableNV) load(userptr, "vkCmdSetShadingRateImageEnableNV");
    context->CmdSetStencilOpEXT = (PFN_vkCmdSetStencilOpEXT) load(userptr, "vkCmdSetStencilOpEXT");
    context->CmdSetStencilTestEnableEXT = (PFN_vkCmdSetStencilTestEnableEXT) load(userptr, "vkCmdSetStencilTestEnableEXT");
    context->CmdSetTessellationDomainOriginEXT = (PFN_vkCmdSetTessellationDomainOriginEXT) load(userptr, "vkCmdSetTessellationDomainOriginEXT");
    context->CmdSetVertexInputEXT = (PFN_vkCmdSetVertexInputEXT) load(userptr, "vkCmdSetVertexInputEXT");
    context->CmdSetViewportSwizzleNV = (PFN_vkCmdSetViewportSwizzleNV) load(userptr, "vkCmdSetViewportSwizzleNV");
    context->CmdSetViewportWScalingEnableNV = (PFN_vkCmdSetViewportWScalingEnableNV) load(userptr, "vkCmdSetViewportWScalingEnableNV");
    context->CmdSetViewportWithCountEXT = (PFN_vkCmdSetViewportWithCountEXT) load(userptr, "vkCmdSetViewportWithCountEXT");
    context->CreateShadersEXT = (PFN_vkCreateShadersEXT) load(userptr, "vkCreateShadersEXT");
    context->DestroyShaderEXT = (PFN_vkDestroyShaderEXT) load(userptr, "vkDestroyShaderEXT");
    context->GetShaderBinaryDataEXT = (PFN_vkGetShaderBinaryDataEXT) load(userptr, "vkGetShaderBinaryDataEXT");
}
static void glad_vk_load_VK_EXT_swapchain_maintenance1(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_swapchain_maintenance1) return;
    context->ReleaseSwapchainImagesEXT = (PFN_vkReleaseSwapchainImagesEXT) load(userptr, "vkReleaseSwapchainImagesEXT");
}
static void glad_vk_load_VK_EXT_tooling_info(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_tooling_info) return;
    context->GetPhysicalDeviceToolPropertiesEXT = (PFN_vkGetPhysicalDeviceToolPropertiesEXT) load(userptr, "vkGetPhysicalDeviceToolPropertiesEXT");
}
static void glad_vk_load_VK_EXT_transform_feedback(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_transform_feedback) return;
    context->CmdBeginQueryIndexedEXT = (PFN_vkCmdBeginQueryIndexedEXT) load(userptr, "vkCmdBeginQueryIndexedEXT");
    context->CmdBeginTransformFeedbackEXT = (PFN_vkCmdBeginTransformFeedbackEXT) load(userptr, "vkCmdBeginTransformFeedbackEXT");
    context->CmdBindTransformFeedbackBuffersEXT = (PFN_vkCmdBindTransformFeedbackBuffersEXT) load(userptr, "vkCmdBindTransformFeedbackBuffersEXT");
    context->CmdDrawIndirectByteCountEXT = (PFN_vkCmdDrawIndirectByteCountEXT) load(userptr, "vkCmdDrawIndirectByteCountEXT");
    context->CmdEndQueryIndexedEXT = (PFN_vkCmdEndQueryIndexedEXT) load(userptr, "vkCmdEndQueryIndexedEXT");
    context->CmdEndTransformFeedbackEXT = (PFN_vkCmdEndTransformFeedbackEXT) load(userptr, "vkCmdEndTransformFeedbackEXT");
}
static void glad_vk_load_VK_EXT_validation_cache(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_validation_cache) return;
    context->CreateValidationCacheEXT = (PFN_vkCreateValidationCacheEXT) load(userptr, "vkCreateValidationCacheEXT");
    context->DestroyValidationCacheEXT = (PFN_vkDestroyValidationCacheEXT) load(userptr, "vkDestroyValidationCacheEXT");
    context->GetValidationCacheDataEXT = (PFN_vkGetValidationCacheDataEXT) load(userptr, "vkGetValidationCacheDataEXT");
    context->MergeValidationCachesEXT = (PFN_vkMergeValidationCachesEXT) load(userptr, "vkMergeValidationCachesEXT");
}
static void glad_vk_load_VK_EXT_vertex_input_dynamic_state(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_vertex_input_dynamic_state) return;
    context->CmdSetVertexInputEXT = (PFN_vkCmdSetVertexInputEXT) load(userptr, "vkCmdSetVertexInputEXT");
}
#if defined(VK_USE_PLATFORM_FUCHSIA)
static void glad_vk_load_VK_FUCHSIA_buffer_collection(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->FUCHSIA_buffer_collection) return;
    context->CreateBufferCollectionFUCHSIA = (PFN_vkCreateBufferCollectionFUCHSIA) load(userptr, "vkCreateBufferCollectionFUCHSIA");
    context->DestroyBufferCollectionFUCHSIA = (PFN_vkDestroyBufferCollectionFUCHSIA) load(userptr, "vkDestroyBufferCollectionFUCHSIA");
    context->GetBufferCollectionPropertiesFUCHSIA = (PFN_vkGetBufferCollectionPropertiesFUCHSIA) load(userptr, "vkGetBufferCollectionPropertiesFUCHSIA");
    context->SetBufferCollectionBufferConstraintsFUCHSIA = (PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA) load(userptr, "vkSetBufferCollectionBufferConstraintsFUCHSIA");
    context->SetBufferCollectionImageConstraintsFUCHSIA = (PFN_vkSetBufferCollectionImageConstraintsFUCHSIA) load(userptr, "vkSetBufferCollectionImageConstraintsFUCHSIA");
}

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
static void glad_vk_load_VK_FUCHSIA_external_memory(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->FUCHSIA_external_memory) return;
    context->GetMemoryZirconHandleFUCHSIA = (PFN_vkGetMemoryZirconHandleFUCHSIA) load(userptr, "vkGetMemoryZirconHandleFUCHSIA");
    context->GetMemoryZirconHandlePropertiesFUCHSIA = (PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA) load(userptr, "vkGetMemoryZirconHandlePropertiesFUCHSIA");
}

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
static void glad_vk_load_VK_FUCHSIA_external_semaphore(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->FUCHSIA_external_semaphore) return;
    context->GetSemaphoreZirconHandleFUCHSIA = (PFN_vkGetSemaphoreZirconHandleFUCHSIA) load(userptr, "vkGetSemaphoreZirconHandleFUCHSIA");
    context->ImportSemaphoreZirconHandleFUCHSIA = (PFN_vkImportSemaphoreZirconHandleFUCHSIA) load(userptr, "vkImportSemaphoreZirconHandleFUCHSIA");
}

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
static void glad_vk_load_VK_FUCHSIA_imagepipe_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->FUCHSIA_imagepipe_surface) return;
    context->CreateImagePipeSurfaceFUCHSIA = (PFN_vkCreateImagePipeSurfaceFUCHSIA) load(userptr, "vkCreateImagePipeSurfaceFUCHSIA");
}

#endif
#if defined(VK_USE_PLATFORM_GGP)
static void glad_vk_load_VK_GGP_stream_descriptor_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->GGP_stream_descriptor_surface) return;
    context->CreateStreamDescriptorSurfaceGGP = (PFN_vkCreateStreamDescriptorSurfaceGGP) load(userptr, "vkCreateStreamDescriptorSurfaceGGP");
}

#endif
static void glad_vk_load_VK_GOOGLE_display_timing(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->GOOGLE_display_timing) return;
    context->GetPastPresentationTimingGOOGLE = (PFN_vkGetPastPresentationTimingGOOGLE) load(userptr, "vkGetPastPresentationTimingGOOGLE");
    context->GetRefreshCycleDurationGOOGLE = (PFN_vkGetRefreshCycleDurationGOOGLE) load(userptr, "vkGetRefreshCycleDurationGOOGLE");
}
static void glad_vk_load_VK_HUAWEI_cluster_culling_shader(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->HUAWEI_cluster_culling_shader) return;
    context->CmdDrawClusterHUAWEI = (PFN_vkCmdDrawClusterHUAWEI) load(userptr, "vkCmdDrawClusterHUAWEI");
    context->CmdDrawClusterIndirectHUAWEI = (PFN_vkCmdDrawClusterIndirectHUAWEI) load(userptr, "vkCmdDrawClusterIndirectHUAWEI");
}
static void glad_vk_load_VK_HUAWEI_invocation_mask(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->HUAWEI_invocation_mask) return;
    context->CmdBindInvocationMaskHUAWEI = (PFN_vkCmdBindInvocationMaskHUAWEI) load(userptr, "vkCmdBindInvocationMaskHUAWEI");
}
static void glad_vk_load_VK_HUAWEI_subpass_shading(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->HUAWEI_subpass_shading) return;
    context->CmdSubpassShadingHUAWEI = (PFN_vkCmdSubpassShadingHUAWEI) load(userptr, "vkCmdSubpassShadingHUAWEI");
    context->GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = (PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI) load(userptr, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI");
}
static void glad_vk_load_VK_INTEL_performance_query(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->INTEL_performance_query) return;
    context->AcquirePerformanceConfigurationINTEL = (PFN_vkAcquirePerformanceConfigurationINTEL) load(userptr, "vkAcquirePerformanceConfigurationINTEL");
    context->CmdSetPerformanceMarkerINTEL = (PFN_vkCmdSetPerformanceMarkerINTEL) load(userptr, "vkCmdSetPerformanceMarkerINTEL");
    context->CmdSetPerformanceOverrideINTEL = (PFN_vkCmdSetPerformanceOverrideINTEL) load(userptr, "vkCmdSetPerformanceOverrideINTEL");
    context->CmdSetPerformanceStreamMarkerINTEL = (PFN_vkCmdSetPerformanceStreamMarkerINTEL) load(userptr, "vkCmdSetPerformanceStreamMarkerINTEL");
    context->GetPerformanceParameterINTEL = (PFN_vkGetPerformanceParameterINTEL) load(userptr, "vkGetPerformanceParameterINTEL");
    context->InitializePerformanceApiINTEL = (PFN_vkInitializePerformanceApiINTEL) load(userptr, "vkInitializePerformanceApiINTEL");
    context->QueueSetPerformanceConfigurationINTEL = (PFN_vkQueueSetPerformanceConfigurationINTEL) load(userptr, "vkQueueSetPerformanceConfigurationINTEL");
    context->ReleasePerformanceConfigurationINTEL = (PFN_vkReleasePerformanceConfigurationINTEL) load(userptr, "vkReleasePerformanceConfigurationINTEL");
    context->UninitializePerformanceApiINTEL = (PFN_vkUninitializePerformanceApiINTEL) load(userptr, "vkUninitializePerformanceApiINTEL");
}
static void glad_vk_load_VK_KHR_acceleration_structure(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_acceleration_structure) return;
    context->BuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR) load(userptr, "vkBuildAccelerationStructuresKHR");
    context->CmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR) load(userptr, "vkCmdBuildAccelerationStructuresIndirectKHR");
    context->CmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR) load(userptr, "vkCmdBuildAccelerationStructuresKHR");
    context->CmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR) load(userptr, "vkCmdCopyAccelerationStructureKHR");
    context->CmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR) load(userptr, "vkCmdCopyAccelerationStructureToMemoryKHR");
    context->CmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR) load(userptr, "vkCmdCopyMemoryToAccelerationStructureKHR");
    context->CmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR) load(userptr, "vkCmdWriteAccelerationStructuresPropertiesKHR");
    context->CopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR) load(userptr, "vkCopyAccelerationStructureKHR");
    context->CopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR) load(userptr, "vkCopyAccelerationStructureToMemoryKHR");
    context->CopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR) load(userptr, "vkCopyMemoryToAccelerationStructureKHR");
    context->CreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR) load(userptr, "vkCreateAccelerationStructureKHR");
    context->DestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR) load(userptr, "vkDestroyAccelerationStructureKHR");
    context->GetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR) load(userptr, "vkGetAccelerationStructureBuildSizesKHR");
    context->GetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR) load(userptr, "vkGetAccelerationStructureDeviceAddressKHR");
    context->GetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR) load(userptr, "vkGetDeviceAccelerationStructureCompatibilityKHR");
    context->WriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR) load(userptr, "vkWriteAccelerationStructuresPropertiesKHR");
}
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
static void glad_vk_load_VK_KHR_android_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_android_surface) return;
    context->CreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR) load(userptr, "vkCreateAndroidSurfaceKHR");
}

#endif
static void glad_vk_load_VK_KHR_bind_memory2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_bind_memory2) return;
    context->BindBufferMemory2KHR = (PFN_vkBindBufferMemory2KHR) load(userptr, "vkBindBufferMemory2KHR");
    context->BindImageMemory2KHR = (PFN_vkBindImageMemory2KHR) load(userptr, "vkBindImageMemory2KHR");
}
static void glad_vk_load_VK_KHR_buffer_device_address(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_buffer_device_address) return;
    context->GetBufferDeviceAddressKHR = (PFN_vkGetBufferDeviceAddressKHR) load(userptr, "vkGetBufferDeviceAddressKHR");
    context->GetBufferOpaqueCaptureAddressKHR = (PFN_vkGetBufferOpaqueCaptureAddressKHR) load(userptr, "vkGetBufferOpaqueCaptureAddressKHR");
    context->GetDeviceMemoryOpaqueCaptureAddressKHR = (PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR) load(userptr, "vkGetDeviceMemoryOpaqueCaptureAddressKHR");
}
static void glad_vk_load_VK_KHR_calibrated_timestamps(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_calibrated_timestamps) return;
    context->GetCalibratedTimestampsKHR = (PFN_vkGetCalibratedTimestampsKHR) load(userptr, "vkGetCalibratedTimestampsKHR");
    context->GetPhysicalDeviceCalibrateableTimeDomainsKHR = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR) load(userptr, "vkGetPhysicalDeviceCalibrateableTimeDomainsKHR");
}
static void glad_vk_load_VK_KHR_cooperative_matrix(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_cooperative_matrix) return;
    context->GetPhysicalDeviceCooperativeMatrixPropertiesKHR = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR) load(userptr, "vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR");
}
static void glad_vk_load_VK_KHR_copy_commands2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_copy_commands2) return;
    context->CmdBlitImage2KHR = (PFN_vkCmdBlitImage2KHR) load(userptr, "vkCmdBlitImage2KHR");
    context->CmdCopyBuffer2KHR = (PFN_vkCmdCopyBuffer2KHR) load(userptr, "vkCmdCopyBuffer2KHR");
    context->CmdCopyBufferToImage2KHR = (PFN_vkCmdCopyBufferToImage2KHR) load(userptr, "vkCmdCopyBufferToImage2KHR");
    context->CmdCopyImage2KHR = (PFN_vkCmdCopyImage2KHR) load(userptr, "vkCmdCopyImage2KHR");
    context->CmdCopyImageToBuffer2KHR = (PFN_vkCmdCopyImageToBuffer2KHR) load(userptr, "vkCmdCopyImageToBuffer2KHR");
    context->CmdResolveImage2KHR = (PFN_vkCmdResolveImage2KHR) load(userptr, "vkCmdResolveImage2KHR");
}
static void glad_vk_load_VK_KHR_create_renderpass2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_create_renderpass2) return;
    context->CmdBeginRenderPass2KHR = (PFN_vkCmdBeginRenderPass2KHR) load(userptr, "vkCmdBeginRenderPass2KHR");
    context->CmdEndRenderPass2KHR = (PFN_vkCmdEndRenderPass2KHR) load(userptr, "vkCmdEndRenderPass2KHR");
    context->CmdNextSubpass2KHR = (PFN_vkCmdNextSubpass2KHR) load(userptr, "vkCmdNextSubpass2KHR");
    context->CreateRenderPass2KHR = (PFN_vkCreateRenderPass2KHR) load(userptr, "vkCreateRenderPass2KHR");
}
static void glad_vk_load_VK_KHR_deferred_host_operations(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_deferred_host_operations) return;
    context->CreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR) load(userptr, "vkCreateDeferredOperationKHR");
    context->DeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR) load(userptr, "vkDeferredOperationJoinKHR");
    context->DestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR) load(userptr, "vkDestroyDeferredOperationKHR");
    context->GetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR) load(userptr, "vkGetDeferredOperationMaxConcurrencyKHR");
    context->GetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR) load(userptr, "vkGetDeferredOperationResultKHR");
}
static void glad_vk_load_VK_KHR_descriptor_update_template(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_descriptor_update_template) return;
    context->CmdPushDescriptorSetWithTemplateKHR = (PFN_vkCmdPushDescriptorSetWithTemplateKHR) load(userptr, "vkCmdPushDescriptorSetWithTemplateKHR");
    context->CreateDescriptorUpdateTemplateKHR = (PFN_vkCreateDescriptorUpdateTemplateKHR) load(userptr, "vkCreateDescriptorUpdateTemplateKHR");
    context->DestroyDescriptorUpdateTemplateKHR = (PFN_vkDestroyDescriptorUpdateTemplateKHR) load(userptr, "vkDestroyDescriptorUpdateTemplateKHR");
    context->UpdateDescriptorSetWithTemplateKHR = (PFN_vkUpdateDescriptorSetWithTemplateKHR) load(userptr, "vkUpdateDescriptorSetWithTemplateKHR");
}
static void glad_vk_load_VK_KHR_device_group(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_device_group) return;
    context->AcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR) load(userptr, "vkAcquireNextImage2KHR");
    context->CmdDispatchBaseKHR = (PFN_vkCmdDispatchBaseKHR) load(userptr, "vkCmdDispatchBaseKHR");
    context->CmdSetDeviceMaskKHR = (PFN_vkCmdSetDeviceMaskKHR) load(userptr, "vkCmdSetDeviceMaskKHR");
    context->GetDeviceGroupPeerMemoryFeaturesKHR = (PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR) load(userptr, "vkGetDeviceGroupPeerMemoryFeaturesKHR");
    context->GetDeviceGroupPresentCapabilitiesKHR = (PFN_vkGetDeviceGroupPresentCapabilitiesKHR) load(userptr, "vkGetDeviceGroupPresentCapabilitiesKHR");
    context->GetDeviceGroupSurfacePresentModesKHR = (PFN_vkGetDeviceGroupSurfacePresentModesKHR) load(userptr, "vkGetDeviceGroupSurfacePresentModesKHR");
    context->GetPhysicalDevicePresentRectanglesKHR = (PFN_vkGetPhysicalDevicePresentRectanglesKHR) load(userptr, "vkGetPhysicalDevicePresentRectanglesKHR");
}
static void glad_vk_load_VK_KHR_device_group_creation(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_device_group_creation) return;
    context->EnumeratePhysicalDeviceGroupsKHR = (PFN_vkEnumeratePhysicalDeviceGroupsKHR) load(userptr, "vkEnumeratePhysicalDeviceGroupsKHR");
}
static void glad_vk_load_VK_KHR_display(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_display) return;
    context->CreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR) load(userptr, "vkCreateDisplayModeKHR");
    context->CreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR) load(userptr, "vkCreateDisplayPlaneSurfaceKHR");
    context->GetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR) load(userptr, "vkGetDisplayModePropertiesKHR");
    context->GetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR) load(userptr, "vkGetDisplayPlaneCapabilitiesKHR");
    context->GetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR) load(userptr, "vkGetDisplayPlaneSupportedDisplaysKHR");
    context->GetPhysicalDeviceDisplayPlanePropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR) load(userptr, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    context->GetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR) load(userptr, "vkGetPhysicalDeviceDisplayPropertiesKHR");
}
static void glad_vk_load_VK_KHR_display_swapchain(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_display_swapchain) return;
    context->CreateSharedSwapchainsKHR = (PFN_vkCreateSharedSwapchainsKHR) load(userptr, "vkCreateSharedSwapchainsKHR");
}
static void glad_vk_load_VK_KHR_draw_indirect_count(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_draw_indirect_count) return;
    context->CmdDrawIndexedIndirectCountKHR = (PFN_vkCmdDrawIndexedIndirectCountKHR) load(userptr, "vkCmdDrawIndexedIndirectCountKHR");
    context->CmdDrawIndirectCountKHR = (PFN_vkCmdDrawIndirectCountKHR) load(userptr, "vkCmdDrawIndirectCountKHR");
}
static void glad_vk_load_VK_KHR_dynamic_rendering(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_dynamic_rendering) return;
    context->CmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) load(userptr, "vkCmdBeginRenderingKHR");
    context->CmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) load(userptr, "vkCmdEndRenderingKHR");
}
static void glad_vk_load_VK_KHR_dynamic_rendering_local_read(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_dynamic_rendering_local_read) return;
    context->CmdSetRenderingAttachmentLocationsKHR = (PFN_vkCmdSetRenderingAttachmentLocationsKHR) load(userptr, "vkCmdSetRenderingAttachmentLocationsKHR");
    context->CmdSetRenderingInputAttachmentIndicesKHR = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR) load(userptr, "vkCmdSetRenderingInputAttachmentIndicesKHR");
}
static void glad_vk_load_VK_KHR_external_fence_capabilities(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_fence_capabilities) return;
    context->GetPhysicalDeviceExternalFencePropertiesKHR = (PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR) load(userptr, "vkGetPhysicalDeviceExternalFencePropertiesKHR");
}
static void glad_vk_load_VK_KHR_external_fence_fd(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_fence_fd) return;
    context->GetFenceFdKHR = (PFN_vkGetFenceFdKHR) load(userptr, "vkGetFenceFdKHR");
    context->ImportFenceFdKHR = (PFN_vkImportFenceFdKHR) load(userptr, "vkImportFenceFdKHR");
}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
static void glad_vk_load_VK_KHR_external_fence_win32(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_fence_win32) return;
    context->GetFenceWin32HandleKHR = (PFN_vkGetFenceWin32HandleKHR) load(userptr, "vkGetFenceWin32HandleKHR");
    context->ImportFenceWin32HandleKHR = (PFN_vkImportFenceWin32HandleKHR) load(userptr, "vkImportFenceWin32HandleKHR");
}

#endif
static void glad_vk_load_VK_KHR_external_memory_capabilities(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_memory_capabilities) return;
    context->GetPhysicalDeviceExternalBufferPropertiesKHR = (PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR) load(userptr, "vkGetPhysicalDeviceExternalBufferPropertiesKHR");
}
static void glad_vk_load_VK_KHR_external_memory_fd(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_memory_fd) return;
    context->GetMemoryFdKHR = (PFN_vkGetMemoryFdKHR) load(userptr, "vkGetMemoryFdKHR");
    context->GetMemoryFdPropertiesKHR = (PFN_vkGetMemoryFdPropertiesKHR) load(userptr, "vkGetMemoryFdPropertiesKHR");
}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
static void glad_vk_load_VK_KHR_external_memory_win32(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_memory_win32) return;
    context->GetMemoryWin32HandleKHR = (PFN_vkGetMemoryWin32HandleKHR) load(userptr, "vkGetMemoryWin32HandleKHR");
    context->GetMemoryWin32HandlePropertiesKHR = (PFN_vkGetMemoryWin32HandlePropertiesKHR) load(userptr, "vkGetMemoryWin32HandlePropertiesKHR");
}

#endif
static void glad_vk_load_VK_KHR_external_semaphore_capabilities(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_semaphore_capabilities) return;
    context->GetPhysicalDeviceExternalSemaphorePropertiesKHR = (PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR) load(userptr, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");
}
static void glad_vk_load_VK_KHR_external_semaphore_fd(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_semaphore_fd) return;
    context->GetSemaphoreFdKHR = (PFN_vkGetSemaphoreFdKHR) load(userptr, "vkGetSemaphoreFdKHR");
    context->ImportSemaphoreFdKHR = (PFN_vkImportSemaphoreFdKHR) load(userptr, "vkImportSemaphoreFdKHR");
}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
static void glad_vk_load_VK_KHR_external_semaphore_win32(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_external_semaphore_win32) return;
    context->GetSemaphoreWin32HandleKHR = (PFN_vkGetSemaphoreWin32HandleKHR) load(userptr, "vkGetSemaphoreWin32HandleKHR");
    context->ImportSemaphoreWin32HandleKHR = (PFN_vkImportSemaphoreWin32HandleKHR) load(userptr, "vkImportSemaphoreWin32HandleKHR");
}

#endif
static void glad_vk_load_VK_KHR_fragment_shading_rate(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_fragment_shading_rate) return;
    context->CmdSetFragmentShadingRateKHR = (PFN_vkCmdSetFragmentShadingRateKHR) load(userptr, "vkCmdSetFragmentShadingRateKHR");
    context->GetPhysicalDeviceFragmentShadingRatesKHR = (PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR) load(userptr, "vkGetPhysicalDeviceFragmentShadingRatesKHR");
}
static void glad_vk_load_VK_KHR_get_display_properties2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_get_display_properties2) return;
    context->GetDisplayModeProperties2KHR = (PFN_vkGetDisplayModeProperties2KHR) load(userptr, "vkGetDisplayModeProperties2KHR");
    context->GetDisplayPlaneCapabilities2KHR = (PFN_vkGetDisplayPlaneCapabilities2KHR) load(userptr, "vkGetDisplayPlaneCapabilities2KHR");
    context->GetPhysicalDeviceDisplayPlaneProperties2KHR = (PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR) load(userptr, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR");
    context->GetPhysicalDeviceDisplayProperties2KHR = (PFN_vkGetPhysicalDeviceDisplayProperties2KHR) load(userptr, "vkGetPhysicalDeviceDisplayProperties2KHR");
}
static void glad_vk_load_VK_KHR_get_memory_requirements2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_get_memory_requirements2) return;
    context->GetBufferMemoryRequirements2KHR = (PFN_vkGetBufferMemoryRequirements2KHR) load(userptr, "vkGetBufferMemoryRequirements2KHR");
    context->GetImageMemoryRequirements2KHR = (PFN_vkGetImageMemoryRequirements2KHR) load(userptr, "vkGetImageMemoryRequirements2KHR");
    context->GetImageSparseMemoryRequirements2KHR = (PFN_vkGetImageSparseMemoryRequirements2KHR) load(userptr, "vkGetImageSparseMemoryRequirements2KHR");
}
static void glad_vk_load_VK_KHR_get_physical_device_properties2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_get_physical_device_properties2) return;
    context->GetPhysicalDeviceFeatures2KHR = (PFN_vkGetPhysicalDeviceFeatures2KHR) load(userptr, "vkGetPhysicalDeviceFeatures2KHR");
    context->GetPhysicalDeviceFormatProperties2KHR = (PFN_vkGetPhysicalDeviceFormatProperties2KHR) load(userptr, "vkGetPhysicalDeviceFormatProperties2KHR");
    context->GetPhysicalDeviceImageFormatProperties2KHR = (PFN_vkGetPhysicalDeviceImageFormatProperties2KHR) load(userptr, "vkGetPhysicalDeviceImageFormatProperties2KHR");
    context->GetPhysicalDeviceMemoryProperties2KHR = (PFN_vkGetPhysicalDeviceMemoryProperties2KHR) load(userptr, "vkGetPhysicalDeviceMemoryProperties2KHR");
    context->GetPhysicalDeviceProperties2KHR = (PFN_vkGetPhysicalDeviceProperties2KHR) load(userptr, "vkGetPhysicalDeviceProperties2KHR");
    context->GetPhysicalDeviceQueueFamilyProperties2KHR = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR) load(userptr, "vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    context->GetPhysicalDeviceSparseImageFormatProperties2KHR = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR) load(userptr, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR");
}
static void glad_vk_load_VK_KHR_get_surface_capabilities2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_get_surface_capabilities2) return;
    context->GetPhysicalDeviceSurfaceCapabilities2KHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR) load(userptr, "vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    context->GetPhysicalDeviceSurfaceFormats2KHR = (PFN_vkGetPhysicalDeviceSurfaceFormats2KHR) load(userptr, "vkGetPhysicalDeviceSurfaceFormats2KHR");
}
static void glad_vk_load_VK_KHR_line_rasterization(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_line_rasterization) return;
    context->CmdSetLineStippleKHR = (PFN_vkCmdSetLineStippleKHR) load(userptr, "vkCmdSetLineStippleKHR");
}
static void glad_vk_load_VK_KHR_maintenance1(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_maintenance1) return;
    context->TrimCommandPoolKHR = (PFN_vkTrimCommandPoolKHR) load(userptr, "vkTrimCommandPoolKHR");
}
static void glad_vk_load_VK_KHR_maintenance3(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_maintenance3) return;
    context->GetDescriptorSetLayoutSupportKHR = (PFN_vkGetDescriptorSetLayoutSupportKHR) load(userptr, "vkGetDescriptorSetLayoutSupportKHR");
}
static void glad_vk_load_VK_KHR_maintenance4(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_maintenance4) return;
    context->GetDeviceBufferMemoryRequirementsKHR = (PFN_vkGetDeviceBufferMemoryRequirementsKHR) load(userptr, "vkGetDeviceBufferMemoryRequirementsKHR");
    context->GetDeviceImageMemoryRequirementsKHR = (PFN_vkGetDeviceImageMemoryRequirementsKHR) load(userptr, "vkGetDeviceImageMemoryRequirementsKHR");
    context->GetDeviceImageSparseMemoryRequirementsKHR = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR) load(userptr, "vkGetDeviceImageSparseMemoryRequirementsKHR");
}
static void glad_vk_load_VK_KHR_maintenance5(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_maintenance5) return;
    context->CmdBindIndexBuffer2KHR = (PFN_vkCmdBindIndexBuffer2KHR) load(userptr, "vkCmdBindIndexBuffer2KHR");
    context->GetDeviceImageSubresourceLayoutKHR = (PFN_vkGetDeviceImageSubresourceLayoutKHR) load(userptr, "vkGetDeviceImageSubresourceLayoutKHR");
    context->GetImageSubresourceLayout2KHR = (PFN_vkGetImageSubresourceLayout2KHR) load(userptr, "vkGetImageSubresourceLayout2KHR");
    context->GetRenderingAreaGranularityKHR = (PFN_vkGetRenderingAreaGranularityKHR) load(userptr, "vkGetRenderingAreaGranularityKHR");
}
static void glad_vk_load_VK_KHR_maintenance6(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_maintenance6) return;
    context->CmdBindDescriptorBufferEmbeddedSamplers2EXT = (PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT) load(userptr, "vkCmdBindDescriptorBufferEmbeddedSamplers2EXT");
    context->CmdBindDescriptorSets2KHR = (PFN_vkCmdBindDescriptorSets2KHR) load(userptr, "vkCmdBindDescriptorSets2KHR");
    context->CmdPushConstants2KHR = (PFN_vkCmdPushConstants2KHR) load(userptr, "vkCmdPushConstants2KHR");
    context->CmdPushDescriptorSet2KHR = (PFN_vkCmdPushDescriptorSet2KHR) load(userptr, "vkCmdPushDescriptorSet2KHR");
    context->CmdPushDescriptorSetWithTemplate2KHR = (PFN_vkCmdPushDescriptorSetWithTemplate2KHR) load(userptr, "vkCmdPushDescriptorSetWithTemplate2KHR");
    context->CmdSetDescriptorBufferOffsets2EXT = (PFN_vkCmdSetDescriptorBufferOffsets2EXT) load(userptr, "vkCmdSetDescriptorBufferOffsets2EXT");
}
static void glad_vk_load_VK_KHR_map_memory2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_map_memory2) return;
    context->MapMemory2KHR = (PFN_vkMapMemory2KHR) load(userptr, "vkMapMemory2KHR");
    context->UnmapMemory2KHR = (PFN_vkUnmapMemory2KHR) load(userptr, "vkUnmapMemory2KHR");
}
static void glad_vk_load_VK_KHR_performance_query(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_performance_query) return;
    context->AcquireProfilingLockKHR = (PFN_vkAcquireProfilingLockKHR) load(userptr, "vkAcquireProfilingLockKHR");
    context->EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = (PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR) load(userptr, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR");
    context->GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR = (PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR) load(userptr, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR");
    context->ReleaseProfilingLockKHR = (PFN_vkReleaseProfilingLockKHR) load(userptr, "vkReleaseProfilingLockKHR");
}
static void glad_vk_load_VK_KHR_pipeline_binary(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_pipeline_binary) return;
    context->CreatePipelineBinariesKHR = (PFN_vkCreatePipelineBinariesKHR) load(userptr, "vkCreatePipelineBinariesKHR");
    context->DestroyPipelineBinaryKHR = (PFN_vkDestroyPipelineBinaryKHR) load(userptr, "vkDestroyPipelineBinaryKHR");
    context->GetPipelineBinaryDataKHR = (PFN_vkGetPipelineBinaryDataKHR) load(userptr, "vkGetPipelineBinaryDataKHR");
    context->GetPipelineKeyKHR = (PFN_vkGetPipelineKeyKHR) load(userptr, "vkGetPipelineKeyKHR");
    context->ReleaseCapturedPipelineDataKHR = (PFN_vkReleaseCapturedPipelineDataKHR) load(userptr, "vkReleaseCapturedPipelineDataKHR");
}
static void glad_vk_load_VK_KHR_pipeline_executable_properties(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_pipeline_executable_properties) return;
    context->GetPipelineExecutableInternalRepresentationsKHR = (PFN_vkGetPipelineExecutableInternalRepresentationsKHR) load(userptr, "vkGetPipelineExecutableInternalRepresentationsKHR");
    context->GetPipelineExecutablePropertiesKHR = (PFN_vkGetPipelineExecutablePropertiesKHR) load(userptr, "vkGetPipelineExecutablePropertiesKHR");
    context->GetPipelineExecutableStatisticsKHR = (PFN_vkGetPipelineExecutableStatisticsKHR) load(userptr, "vkGetPipelineExecutableStatisticsKHR");
}
static void glad_vk_load_VK_KHR_present_wait(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_present_wait) return;
    context->WaitForPresentKHR = (PFN_vkWaitForPresentKHR) load(userptr, "vkWaitForPresentKHR");
}
static void glad_vk_load_VK_KHR_present_wait2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_present_wait2) return;
    context->WaitForPresent2KHR = (PFN_vkWaitForPresent2KHR) load(userptr, "vkWaitForPresent2KHR");
}
static void glad_vk_load_VK_KHR_push_descriptor(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_push_descriptor) return;
    context->CmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR) load(userptr, "vkCmdPushDescriptorSetKHR");
    context->CmdPushDescriptorSetWithTemplateKHR = (PFN_vkCmdPushDescriptorSetWithTemplateKHR) load(userptr, "vkCmdPushDescriptorSetWithTemplateKHR");
}
static void glad_vk_load_VK_KHR_ray_tracing_maintenance1(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_ray_tracing_maintenance1) return;
    context->CmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR) load(userptr, "vkCmdTraceRaysIndirect2KHR");
}
static void glad_vk_load_VK_KHR_ray_tracing_pipeline(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_ray_tracing_pipeline) return;
    context->CmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR) load(userptr, "vkCmdSetRayTracingPipelineStackSizeKHR");
    context->CmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR) load(userptr, "vkCmdTraceRaysIndirectKHR");
    context->CmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR) load(userptr, "vkCmdTraceRaysKHR");
    context->CreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR) load(userptr, "vkCreateRayTracingPipelinesKHR");
    context->GetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR) load(userptr, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
    context->GetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR) load(userptr, "vkGetRayTracingShaderGroupHandlesKHR");
    context->GetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR) load(userptr, "vkGetRayTracingShaderGroupStackSizeKHR");
}
static void glad_vk_load_VK_KHR_sampler_ycbcr_conversion(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_sampler_ycbcr_conversion) return;
    context->CreateSamplerYcbcrConversionKHR = (PFN_vkCreateSamplerYcbcrConversionKHR) load(userptr, "vkCreateSamplerYcbcrConversionKHR");
    context->DestroySamplerYcbcrConversionKHR = (PFN_vkDestroySamplerYcbcrConversionKHR) load(userptr, "vkDestroySamplerYcbcrConversionKHR");
}
static void glad_vk_load_VK_KHR_shared_presentable_image(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_shared_presentable_image) return;
    context->GetSwapchainStatusKHR = (PFN_vkGetSwapchainStatusKHR) load(userptr, "vkGetSwapchainStatusKHR");
}
static void glad_vk_load_VK_KHR_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_surface) return;
    context->DestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) load(userptr, "vkDestroySurfaceKHR");
    context->GetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR) load(userptr, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    context->GetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR) load(userptr, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    context->GetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR) load(userptr, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    context->GetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) load(userptr, "vkGetPhysicalDeviceSurfaceSupportKHR");
}
static void glad_vk_load_VK_KHR_swapchain(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_swapchain) return;
    context->AcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR) load(userptr, "vkAcquireNextImage2KHR");
    context->AcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) load(userptr, "vkAcquireNextImageKHR");
    context->CreateSwapchainKHR = (PFN_vkCreateSwapchainKHR) load(userptr, "vkCreateSwapchainKHR");
    context->DestroySwapchainKHR = (PFN_vkDestroySwapchainKHR) load(userptr, "vkDestroySwapchainKHR");
    context->GetDeviceGroupPresentCapabilitiesKHR = (PFN_vkGetDeviceGroupPresentCapabilitiesKHR) load(userptr, "vkGetDeviceGroupPresentCapabilitiesKHR");
    context->GetDeviceGroupSurfacePresentModesKHR = (PFN_vkGetDeviceGroupSurfacePresentModesKHR) load(userptr, "vkGetDeviceGroupSurfacePresentModesKHR");
    context->GetPhysicalDevicePresentRectanglesKHR = (PFN_vkGetPhysicalDevicePresentRectanglesKHR) load(userptr, "vkGetPhysicalDevicePresentRectanglesKHR");
    context->GetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR) load(userptr, "vkGetSwapchainImagesKHR");
    context->QueuePresentKHR = (PFN_vkQueuePresentKHR) load(userptr, "vkQueuePresentKHR");
}
static void glad_vk_load_VK_KHR_synchronization2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_synchronization2) return;
    context->CmdPipelineBarrier2KHR = (PFN_vkCmdPipelineBarrier2KHR) load(userptr, "vkCmdPipelineBarrier2KHR");
    context->CmdResetEvent2KHR = (PFN_vkCmdResetEvent2KHR) load(userptr, "vkCmdResetEvent2KHR");
    context->CmdSetEvent2KHR = (PFN_vkCmdSetEvent2KHR) load(userptr, "vkCmdSetEvent2KHR");
    context->CmdWaitEvents2KHR = (PFN_vkCmdWaitEvents2KHR) load(userptr, "vkCmdWaitEvents2KHR");
    context->CmdWriteTimestamp2KHR = (PFN_vkCmdWriteTimestamp2KHR) load(userptr, "vkCmdWriteTimestamp2KHR");
    context->QueueSubmit2KHR = (PFN_vkQueueSubmit2KHR) load(userptr, "vkQueueSubmit2KHR");
}
static void glad_vk_load_VK_KHR_timeline_semaphore(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_timeline_semaphore) return;
    context->GetSemaphoreCounterValueKHR = (PFN_vkGetSemaphoreCounterValueKHR) load(userptr, "vkGetSemaphoreCounterValueKHR");
    context->SignalSemaphoreKHR = (PFN_vkSignalSemaphoreKHR) load(userptr, "vkSignalSemaphoreKHR");
    context->WaitSemaphoresKHR = (PFN_vkWaitSemaphoresKHR) load(userptr, "vkWaitSemaphoresKHR");
}
static void glad_vk_load_VK_KHR_video_decode_queue(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_video_decode_queue) return;
    context->CmdDecodeVideoKHR = (PFN_vkCmdDecodeVideoKHR) load(userptr, "vkCmdDecodeVideoKHR");
}
static void glad_vk_load_VK_KHR_video_encode_queue(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_video_encode_queue) return;
    context->CmdEncodeVideoKHR = (PFN_vkCmdEncodeVideoKHR) load(userptr, "vkCmdEncodeVideoKHR");
    context->GetEncodedVideoSessionParametersKHR = (PFN_vkGetEncodedVideoSessionParametersKHR) load(userptr, "vkGetEncodedVideoSessionParametersKHR");
    context->GetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR) load(userptr, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR");
}
static void glad_vk_load_VK_KHR_video_queue(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_video_queue) return;
    context->BindVideoSessionMemoryKHR = (PFN_vkBindVideoSessionMemoryKHR) load(userptr, "vkBindVideoSessionMemoryKHR");
    context->CmdBeginVideoCodingKHR = (PFN_vkCmdBeginVideoCodingKHR) load(userptr, "vkCmdBeginVideoCodingKHR");
    context->CmdControlVideoCodingKHR = (PFN_vkCmdControlVideoCodingKHR) load(userptr, "vkCmdControlVideoCodingKHR");
    context->CmdEndVideoCodingKHR = (PFN_vkCmdEndVideoCodingKHR) load(userptr, "vkCmdEndVideoCodingKHR");
    context->CreateVideoSessionKHR = (PFN_vkCreateVideoSessionKHR) load(userptr, "vkCreateVideoSessionKHR");
    context->CreateVideoSessionParametersKHR = (PFN_vkCreateVideoSessionParametersKHR) load(userptr, "vkCreateVideoSessionParametersKHR");
    context->DestroyVideoSessionKHR = (PFN_vkDestroyVideoSessionKHR) load(userptr, "vkDestroyVideoSessionKHR");
    context->DestroyVideoSessionParametersKHR = (PFN_vkDestroyVideoSessionParametersKHR) load(userptr, "vkDestroyVideoSessionParametersKHR");
    context->GetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR) load(userptr, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    context->GetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR) load(userptr, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    context->GetVideoSessionMemoryRequirementsKHR = (PFN_vkGetVideoSessionMemoryRequirementsKHR) load(userptr, "vkGetVideoSessionMemoryRequirementsKHR");
    context->UpdateVideoSessionParametersKHR = (PFN_vkUpdateVideoSessionParametersKHR) load(userptr, "vkUpdateVideoSessionParametersKHR");
}
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
static void glad_vk_load_VK_KHR_wayland_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_wayland_surface) return;
    context->CreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR) load(userptr, "vkCreateWaylandSurfaceKHR");
    context->GetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR) load(userptr, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
}

#endif
#if defined(VK_USE_PLATFORM_WIN32_KHR)
static void glad_vk_load_VK_KHR_win32_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_win32_surface) return;
    context->CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) load(userptr, "vkCreateWin32SurfaceKHR");
    context->GetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR) load(userptr, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
}

#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
static void glad_vk_load_VK_KHR_xcb_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_xcb_surface) return;
    context->CreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR) load(userptr, "vkCreateXcbSurfaceKHR");
    context->GetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR) load(userptr, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
}

#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
static void glad_vk_load_VK_KHR_xlib_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_xlib_surface) return;
    context->CreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR) load(userptr, "vkCreateXlibSurfaceKHR");
    context->GetPhysicalDeviceXlibPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR) load(userptr, "vkGetPhysicalDeviceXlibPresentationSupportKHR");
}

#endif
#if defined(VK_USE_PLATFORM_IOS_MVK)
static void glad_vk_load_VK_MVK_ios_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->MVK_ios_surface) return;
    context->CreateIOSSurfaceMVK = (PFN_vkCreateIOSSurfaceMVK) load(userptr, "vkCreateIOSSurfaceMVK");
}

#endif
#if defined(VK_USE_PLATFORM_MACOS_MVK)
static void glad_vk_load_VK_MVK_macos_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->MVK_macos_surface) return;
    context->CreateMacOSSurfaceMVK = (PFN_vkCreateMacOSSurfaceMVK) load(userptr, "vkCreateMacOSSurfaceMVK");
}

#endif
#if defined(VK_USE_PLATFORM_VI_NN)
static void glad_vk_load_VK_NN_vi_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NN_vi_surface) return;
    context->CreateViSurfaceNN = (PFN_vkCreateViSurfaceNN) load(userptr, "vkCreateViSurfaceNN");
}

#endif
static void glad_vk_load_VK_NVX_binary_import(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NVX_binary_import) return;
    context->CmdCuLaunchKernelNVX = (PFN_vkCmdCuLaunchKernelNVX) load(userptr, "vkCmdCuLaunchKernelNVX");
    context->CreateCuFunctionNVX = (PFN_vkCreateCuFunctionNVX) load(userptr, "vkCreateCuFunctionNVX");
    context->CreateCuModuleNVX = (PFN_vkCreateCuModuleNVX) load(userptr, "vkCreateCuModuleNVX");
    context->DestroyCuFunctionNVX = (PFN_vkDestroyCuFunctionNVX) load(userptr, "vkDestroyCuFunctionNVX");
    context->DestroyCuModuleNVX = (PFN_vkDestroyCuModuleNVX) load(userptr, "vkDestroyCuModuleNVX");
}
static void glad_vk_load_VK_NVX_image_view_handle(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NVX_image_view_handle) return;
    context->GetImageViewAddressNVX = (PFN_vkGetImageViewAddressNVX) load(userptr, "vkGetImageViewAddressNVX");
    context->GetImageViewHandle64NVX = (PFN_vkGetImageViewHandle64NVX) load(userptr, "vkGetImageViewHandle64NVX");
    context->GetImageViewHandleNVX = (PFN_vkGetImageViewHandleNVX) load(userptr, "vkGetImageViewHandleNVX");
}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
static void glad_vk_load_VK_NV_acquire_winrt_display(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_acquire_winrt_display) return;
    context->AcquireWinrtDisplayNV = (PFN_vkAcquireWinrtDisplayNV) load(userptr, "vkAcquireWinrtDisplayNV");
    context->GetWinrtDisplayNV = (PFN_vkGetWinrtDisplayNV) load(userptr, "vkGetWinrtDisplayNV");
}

#endif
static void glad_vk_load_VK_NV_clip_space_w_scaling(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_clip_space_w_scaling) return;
    context->CmdSetViewportWScalingNV = (PFN_vkCmdSetViewportWScalingNV) load(userptr, "vkCmdSetViewportWScalingNV");
}
static void glad_vk_load_VK_NV_cluster_acceleration_structure(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_cluster_acceleration_structure) return;
    context->CmdBuildClusterAccelerationStructureIndirectNV = (PFN_vkCmdBuildClusterAccelerationStructureIndirectNV) load(userptr, "vkCmdBuildClusterAccelerationStructureIndirectNV");
    context->GetClusterAccelerationStructureBuildSizesNV = (PFN_vkGetClusterAccelerationStructureBuildSizesNV) load(userptr, "vkGetClusterAccelerationStructureBuildSizesNV");
}
static void glad_vk_load_VK_NV_cooperative_matrix(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_cooperative_matrix) return;
    context->GetPhysicalDeviceCooperativeMatrixPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV) load(userptr, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
}
static void glad_vk_load_VK_NV_cooperative_matrix2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_cooperative_matrix2) return;
    context->GetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV) load(userptr, "vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV");
}
static void glad_vk_load_VK_NV_cooperative_vector(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_cooperative_vector) return;
    context->CmdConvertCooperativeVectorMatrixNV = (PFN_vkCmdConvertCooperativeVectorMatrixNV) load(userptr, "vkCmdConvertCooperativeVectorMatrixNV");
    context->ConvertCooperativeVectorMatrixNV = (PFN_vkConvertCooperativeVectorMatrixNV) load(userptr, "vkConvertCooperativeVectorMatrixNV");
    context->GetPhysicalDeviceCooperativeVectorPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeVectorPropertiesNV) load(userptr, "vkGetPhysicalDeviceCooperativeVectorPropertiesNV");
}
static void glad_vk_load_VK_NV_copy_memory_indirect(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_copy_memory_indirect) return;
    context->CmdCopyMemoryIndirectNV = (PFN_vkCmdCopyMemoryIndirectNV) load(userptr, "vkCmdCopyMemoryIndirectNV");
    context->CmdCopyMemoryToImageIndirectNV = (PFN_vkCmdCopyMemoryToImageIndirectNV) load(userptr, "vkCmdCopyMemoryToImageIndirectNV");
}
static void glad_vk_load_VK_NV_coverage_reduction_mode(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_coverage_reduction_mode) return;
    context->GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = (PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV) load(userptr, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
}
#if defined(VK_ENABLE_BETA_EXTENSIONS)
static void glad_vk_load_VK_NV_cuda_kernel_launch(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_cuda_kernel_launch) return;
    context->CmdCudaLaunchKernelNV = (PFN_vkCmdCudaLaunchKernelNV) load(userptr, "vkCmdCudaLaunchKernelNV");
    context->CreateCudaFunctionNV = (PFN_vkCreateCudaFunctionNV) load(userptr, "vkCreateCudaFunctionNV");
    context->CreateCudaModuleNV = (PFN_vkCreateCudaModuleNV) load(userptr, "vkCreateCudaModuleNV");
    context->DestroyCudaFunctionNV = (PFN_vkDestroyCudaFunctionNV) load(userptr, "vkDestroyCudaFunctionNV");
    context->DestroyCudaModuleNV = (PFN_vkDestroyCudaModuleNV) load(userptr, "vkDestroyCudaModuleNV");
    context->GetCudaModuleCacheNV = (PFN_vkGetCudaModuleCacheNV) load(userptr, "vkGetCudaModuleCacheNV");
}

#endif
static void glad_vk_load_VK_NV_device_diagnostic_checkpoints(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_device_diagnostic_checkpoints) return;
    context->CmdSetCheckpointNV = (PFN_vkCmdSetCheckpointNV) load(userptr, "vkCmdSetCheckpointNV");
    context->GetQueueCheckpointData2NV = (PFN_vkGetQueueCheckpointData2NV) load(userptr, "vkGetQueueCheckpointData2NV");
    context->GetQueueCheckpointDataNV = (PFN_vkGetQueueCheckpointDataNV) load(userptr, "vkGetQueueCheckpointDataNV");
}
static void glad_vk_load_VK_NV_device_generated_commands(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_device_generated_commands) return;
    context->CmdBindPipelineShaderGroupNV = (PFN_vkCmdBindPipelineShaderGroupNV) load(userptr, "vkCmdBindPipelineShaderGroupNV");
    context->CmdExecuteGeneratedCommandsNV = (PFN_vkCmdExecuteGeneratedCommandsNV) load(userptr, "vkCmdExecuteGeneratedCommandsNV");
    context->CmdPreprocessGeneratedCommandsNV = (PFN_vkCmdPreprocessGeneratedCommandsNV) load(userptr, "vkCmdPreprocessGeneratedCommandsNV");
    context->CreateIndirectCommandsLayoutNV = (PFN_vkCreateIndirectCommandsLayoutNV) load(userptr, "vkCreateIndirectCommandsLayoutNV");
    context->DestroyIndirectCommandsLayoutNV = (PFN_vkDestroyIndirectCommandsLayoutNV) load(userptr, "vkDestroyIndirectCommandsLayoutNV");
    context->GetGeneratedCommandsMemoryRequirementsNV = (PFN_vkGetGeneratedCommandsMemoryRequirementsNV) load(userptr, "vkGetGeneratedCommandsMemoryRequirementsNV");
}
static void glad_vk_load_VK_NV_device_generated_commands_compute(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_device_generated_commands_compute) return;
    context->CmdUpdatePipelineIndirectBufferNV = (PFN_vkCmdUpdatePipelineIndirectBufferNV) load(userptr, "vkCmdUpdatePipelineIndirectBufferNV");
    context->GetPipelineIndirectDeviceAddressNV = (PFN_vkGetPipelineIndirectDeviceAddressNV) load(userptr, "vkGetPipelineIndirectDeviceAddressNV");
    context->GetPipelineIndirectMemoryRequirementsNV = (PFN_vkGetPipelineIndirectMemoryRequirementsNV) load(userptr, "vkGetPipelineIndirectMemoryRequirementsNV");
}
static void glad_vk_load_VK_NV_external_compute_queue(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_external_compute_queue) return;
    context->CreateExternalComputeQueueNV = (PFN_vkCreateExternalComputeQueueNV) load(userptr, "vkCreateExternalComputeQueueNV");
    context->DestroyExternalComputeQueueNV = (PFN_vkDestroyExternalComputeQueueNV) load(userptr, "vkDestroyExternalComputeQueueNV");
    context->GetExternalComputeQueueDataNV = (PFN_vkGetExternalComputeQueueDataNV) load(userptr, "vkGetExternalComputeQueueDataNV");
}
static void glad_vk_load_VK_NV_external_memory_capabilities(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_external_memory_capabilities) return;
    context->GetPhysicalDeviceExternalImageFormatPropertiesNV = (PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV) load(userptr, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV");
}
static void glad_vk_load_VK_NV_external_memory_rdma(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_external_memory_rdma) return;
    context->GetMemoryRemoteAddressNV = (PFN_vkGetMemoryRemoteAddressNV) load(userptr, "vkGetMemoryRemoteAddressNV");
}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
static void glad_vk_load_VK_NV_external_memory_win32(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_external_memory_win32) return;
    context->GetMemoryWin32HandleNV = (PFN_vkGetMemoryWin32HandleNV) load(userptr, "vkGetMemoryWin32HandleNV");
}

#endif
static void glad_vk_load_VK_NV_fragment_shading_rate_enums(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_fragment_shading_rate_enums) return;
    context->CmdSetFragmentShadingRateEnumNV = (PFN_vkCmdSetFragmentShadingRateEnumNV) load(userptr, "vkCmdSetFragmentShadingRateEnumNV");
}
static void glad_vk_load_VK_NV_low_latency2(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_low_latency2) return;
    context->GetLatencyTimingsNV = (PFN_vkGetLatencyTimingsNV) load(userptr, "vkGetLatencyTimingsNV");
    context->LatencySleepNV = (PFN_vkLatencySleepNV) load(userptr, "vkLatencySleepNV");
    context->QueueNotifyOutOfBandNV = (PFN_vkQueueNotifyOutOfBandNV) load(userptr, "vkQueueNotifyOutOfBandNV");
    context->SetLatencyMarkerNV = (PFN_vkSetLatencyMarkerNV) load(userptr, "vkSetLatencyMarkerNV");
    context->SetLatencySleepModeNV = (PFN_vkSetLatencySleepModeNV) load(userptr, "vkSetLatencySleepModeNV");
}
static void glad_vk_load_VK_NV_memory_decompression(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_memory_decompression) return;
    context->CmdDecompressMemoryIndirectCountNV = (PFN_vkCmdDecompressMemoryIndirectCountNV) load(userptr, "vkCmdDecompressMemoryIndirectCountNV");
    context->CmdDecompressMemoryNV = (PFN_vkCmdDecompressMemoryNV) load(userptr, "vkCmdDecompressMemoryNV");
}
static void glad_vk_load_VK_NV_mesh_shader(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_mesh_shader) return;
    context->CmdDrawMeshTasksIndirectCountNV = (PFN_vkCmdDrawMeshTasksIndirectCountNV) load(userptr, "vkCmdDrawMeshTasksIndirectCountNV");
    context->CmdDrawMeshTasksIndirectNV = (PFN_vkCmdDrawMeshTasksIndirectNV) load(userptr, "vkCmdDrawMeshTasksIndirectNV");
    context->CmdDrawMeshTasksNV = (PFN_vkCmdDrawMeshTasksNV) load(userptr, "vkCmdDrawMeshTasksNV");
}
static void glad_vk_load_VK_NV_optical_flow(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_optical_flow) return;
    context->BindOpticalFlowSessionImageNV = (PFN_vkBindOpticalFlowSessionImageNV) load(userptr, "vkBindOpticalFlowSessionImageNV");
    context->CmdOpticalFlowExecuteNV = (PFN_vkCmdOpticalFlowExecuteNV) load(userptr, "vkCmdOpticalFlowExecuteNV");
    context->CreateOpticalFlowSessionNV = (PFN_vkCreateOpticalFlowSessionNV) load(userptr, "vkCreateOpticalFlowSessionNV");
    context->DestroyOpticalFlowSessionNV = (PFN_vkDestroyOpticalFlowSessionNV) load(userptr, "vkDestroyOpticalFlowSessionNV");
    context->GetPhysicalDeviceOpticalFlowImageFormatsNV = (PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV) load(userptr, "vkGetPhysicalDeviceOpticalFlowImageFormatsNV");
}
static void glad_vk_load_VK_NV_partitioned_acceleration_structure(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_partitioned_acceleration_structure) return;
    context->CmdBuildPartitionedAccelerationStructuresNV = (PFN_vkCmdBuildPartitionedAccelerationStructuresNV) load(userptr, "vkCmdBuildPartitionedAccelerationStructuresNV");
    context->GetPartitionedAccelerationStructuresBuildSizesNV = (PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV) load(userptr, "vkGetPartitionedAccelerationStructuresBuildSizesNV");
}
static void glad_vk_load_VK_NV_ray_tracing(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_ray_tracing) return;
    context->BindAccelerationStructureMemoryNV = (PFN_vkBindAccelerationStructureMemoryNV) load(userptr, "vkBindAccelerationStructureMemoryNV");
    context->CmdBuildAccelerationStructureNV = (PFN_vkCmdBuildAccelerationStructureNV) load(userptr, "vkCmdBuildAccelerationStructureNV");
    context->CmdCopyAccelerationStructureNV = (PFN_vkCmdCopyAccelerationStructureNV) load(userptr, "vkCmdCopyAccelerationStructureNV");
    context->CmdTraceRaysNV = (PFN_vkCmdTraceRaysNV) load(userptr, "vkCmdTraceRaysNV");
    context->CmdWriteAccelerationStructuresPropertiesNV = (PFN_vkCmdWriteAccelerationStructuresPropertiesNV) load(userptr, "vkCmdWriteAccelerationStructuresPropertiesNV");
    context->CompileDeferredNV = (PFN_vkCompileDeferredNV) load(userptr, "vkCompileDeferredNV");
    context->CreateAccelerationStructureNV = (PFN_vkCreateAccelerationStructureNV) load(userptr, "vkCreateAccelerationStructureNV");
    context->CreateRayTracingPipelinesNV = (PFN_vkCreateRayTracingPipelinesNV) load(userptr, "vkCreateRayTracingPipelinesNV");
    context->DestroyAccelerationStructureNV = (PFN_vkDestroyAccelerationStructureNV) load(userptr, "vkDestroyAccelerationStructureNV");
    context->GetAccelerationStructureHandleNV = (PFN_vkGetAccelerationStructureHandleNV) load(userptr, "vkGetAccelerationStructureHandleNV");
    context->GetAccelerationStructureMemoryRequirementsNV = (PFN_vkGetAccelerationStructureMemoryRequirementsNV) load(userptr, "vkGetAccelerationStructureMemoryRequirementsNV");
    context->GetRayTracingShaderGroupHandlesNV = (PFN_vkGetRayTracingShaderGroupHandlesNV) load(userptr, "vkGetRayTracingShaderGroupHandlesNV");
}
static void glad_vk_load_VK_NV_scissor_exclusive(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_scissor_exclusive) return;
    context->CmdSetExclusiveScissorEnableNV = (PFN_vkCmdSetExclusiveScissorEnableNV) load(userptr, "vkCmdSetExclusiveScissorEnableNV");
    context->CmdSetExclusiveScissorNV = (PFN_vkCmdSetExclusiveScissorNV) load(userptr, "vkCmdSetExclusiveScissorNV");
}
static void glad_vk_load_VK_NV_shading_rate_image(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_shading_rate_image) return;
    context->CmdBindShadingRateImageNV = (PFN_vkCmdBindShadingRateImageNV) load(userptr, "vkCmdBindShadingRateImageNV");
    context->CmdSetCoarseSampleOrderNV = (PFN_vkCmdSetCoarseSampleOrderNV) load(userptr, "vkCmdSetCoarseSampleOrderNV");
    context->CmdSetViewportShadingRatePaletteNV = (PFN_vkCmdSetViewportShadingRatePaletteNV) load(userptr, "vkCmdSetViewportShadingRatePaletteNV");
}
#if defined(VK_USE_PLATFORM_OHOS)
static void glad_vk_load_VK_OHOS_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OHOS_surface) return;
    context->CreateSurfaceOHOS = (PFN_vkCreateSurfaceOHOS) load(userptr, "vkCreateSurfaceOHOS");
}

#endif
static void glad_vk_load_VK_QCOM_tile_memory_heap(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_tile_memory_heap) return;
    context->CmdBindTileMemoryQCOM = (PFN_vkCmdBindTileMemoryQCOM) load(userptr, "vkCmdBindTileMemoryQCOM");
}
static void glad_vk_load_VK_QCOM_tile_properties(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_tile_properties) return;
    context->GetDynamicRenderingTilePropertiesQCOM = (PFN_vkGetDynamicRenderingTilePropertiesQCOM) load(userptr, "vkGetDynamicRenderingTilePropertiesQCOM");
    context->GetFramebufferTilePropertiesQCOM = (PFN_vkGetFramebufferTilePropertiesQCOM) load(userptr, "vkGetFramebufferTilePropertiesQCOM");
}
static void glad_vk_load_VK_QCOM_tile_shading(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_tile_shading) return;
    context->CmdBeginPerTileExecutionQCOM = (PFN_vkCmdBeginPerTileExecutionQCOM) load(userptr, "vkCmdBeginPerTileExecutionQCOM");
    context->CmdDispatchTileQCOM = (PFN_vkCmdDispatchTileQCOM) load(userptr, "vkCmdDispatchTileQCOM");
    context->CmdEndPerTileExecutionQCOM = (PFN_vkCmdEndPerTileExecutionQCOM) load(userptr, "vkCmdEndPerTileExecutionQCOM");
}
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
static void glad_vk_load_VK_QNX_external_memory_screen_buffer(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QNX_external_memory_screen_buffer) return;
    context->GetScreenBufferPropertiesQNX = (PFN_vkGetScreenBufferPropertiesQNX) load(userptr, "vkGetScreenBufferPropertiesQNX");
}

#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
static void glad_vk_load_VK_QNX_screen_surface(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QNX_screen_surface) return;
    context->CreateScreenSurfaceQNX = (PFN_vkCreateScreenSurfaceQNX) load(userptr, "vkCreateScreenSurfaceQNX");
    context->GetPhysicalDeviceScreenPresentationSupportQNX = (PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX) load(userptr, "vkGetPhysicalDeviceScreenPresentationSupportQNX");
}

#endif
static void glad_vk_load_VK_VALVE_descriptor_set_host_mapping(GladVulkanContext *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->VALVE_descriptor_set_host_mapping) return;
    context->GetDescriptorSetHostMappingVALVE = (PFN_vkGetDescriptorSetHostMappingVALVE) load(userptr, "vkGetDescriptorSetHostMappingVALVE");
    context->GetDescriptorSetLayoutHostMappingInfoVALVE = (PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE) load(userptr, "vkGetDescriptorSetLayoutHostMappingInfoVALVE");
}



static int glad_vk_get_extensions(GladVulkanContext *context, VkPhysicalDevice physical_device, uint32_t *out_extension_count, char ***out_extensions) {
    uint32_t i;
    uint32_t instance_extension_count = 0;
    uint32_t device_extension_count = 0;
    uint32_t max_extension_count = 0;
    uint32_t total_extension_count = 0;
    char **extensions = NULL;
    VkExtensionProperties *ext_properties = NULL;
    VkResult result;

    if (context->EnumerateInstanceExtensionProperties == NULL || (physical_device != NULL && context->EnumerateDeviceExtensionProperties == NULL)) {
        return 0;
    }

    result = context->EnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
    if (result != VK_SUCCESS) {
        return 0;
    }

    if (physical_device != NULL) {
        result = context->EnumerateDeviceExtensionProperties(physical_device, NULL, &device_extension_count, NULL);
        if (result != VK_SUCCESS) {
            return 0;
        }
    }

    total_extension_count = instance_extension_count + device_extension_count;
    if (total_extension_count <= 0) {
        return 0;
    }

    max_extension_count = instance_extension_count > device_extension_count
        ? instance_extension_count : device_extension_count;

    ext_properties = (VkExtensionProperties*) malloc(max_extension_count * sizeof(VkExtensionProperties));
    if (ext_properties == NULL) {
        goto glad_vk_get_extensions_error;
    }

    result = context->EnumerateInstanceExtensionProperties(NULL, &instance_extension_count, ext_properties);
    if (result != VK_SUCCESS) {
        goto glad_vk_get_extensions_error;
    }

    extensions = (char**) calloc(total_extension_count, sizeof(char*));
    if (extensions == NULL) {
        goto glad_vk_get_extensions_error;
    }

    for (i = 0; i < instance_extension_count; ++i) {
        VkExtensionProperties ext = ext_properties[i];

        size_t extension_name_length = strlen(ext.extensionName) + 1;
        extensions[i] = (char*) malloc(extension_name_length * sizeof(char));
        if (extensions[i] == NULL) {
            goto glad_vk_get_extensions_error;
        }
        memcpy(extensions[i], ext.extensionName, extension_name_length * sizeof(char));
    }

    if (physical_device != NULL) {
        result = context->EnumerateDeviceExtensionProperties(physical_device, NULL, &device_extension_count, ext_properties);
        if (result != VK_SUCCESS) {
            goto glad_vk_get_extensions_error;
        }

        for (i = 0; i < device_extension_count; ++i) {
            VkExtensionProperties ext = ext_properties[i];

            size_t extension_name_length = strlen(ext.extensionName) + 1;
            extensions[instance_extension_count + i] = (char*) malloc(extension_name_length * sizeof(char));
            if (extensions[instance_extension_count + i] == NULL) {
                goto glad_vk_get_extensions_error;
            }
            memcpy(extensions[instance_extension_count + i], ext.extensionName, extension_name_length * sizeof(char));
        }
    }

    free((void*) ext_properties);

    *out_extension_count = total_extension_count;
    *out_extensions = extensions;

    return 1;

glad_vk_get_extensions_error:
    free((void*) ext_properties);
    if (extensions != NULL) {
        for (i = 0; i < total_extension_count; ++i) {
            free((void*) extensions[i]);
        }
        free(extensions);
    }
    return 0;
}

static void glad_vk_free_extensions(uint32_t extension_count, char **extensions) {
    uint32_t i;

    for(i = 0; i < extension_count ; ++i) {
        free((void*) (extensions[i]));
    }

    free((void*) extensions);
}

static int glad_vk_has_extension(const char *name, uint32_t extension_count, char **extensions) {
    uint32_t i;

    for (i = 0; i < extension_count; ++i) {
        if(extensions[i] != NULL && strcmp(name, extensions[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

static GLADapiproc glad_vk_get_proc_from_userptr(void *userptr, const char* name) {
    return (GLAD_GNUC_EXTENSION (GLADapiproc (*)(const char *name)) userptr)(name);
}

static int glad_vk_find_extensions_vulkan(GladVulkanContext *context, VkPhysicalDevice physical_device) {
    uint32_t extension_count = 0;
    char **extensions = NULL;
    if (!glad_vk_get_extensions(context, physical_device, &extension_count, &extensions)) return 0;

#if defined(VK_ENABLE_BETA_EXTENSIONS)
    context->AMDX_shader_enqueue = glad_vk_has_extension("VK_AMDX_shader_enqueue", extension_count, extensions);

#endif
    context->AMD_anti_lag = glad_vk_has_extension("VK_AMD_anti_lag", extension_count, extensions);
    context->AMD_buffer_marker = glad_vk_has_extension("VK_AMD_buffer_marker", extension_count, extensions);
    context->AMD_device_coherent_memory = glad_vk_has_extension("VK_AMD_device_coherent_memory", extension_count, extensions);
    context->AMD_display_native_hdr = glad_vk_has_extension("VK_AMD_display_native_hdr", extension_count, extensions);
    context->AMD_draw_indirect_count = glad_vk_has_extension("VK_AMD_draw_indirect_count", extension_count, extensions);
    context->AMD_gcn_shader = glad_vk_has_extension("VK_AMD_gcn_shader", extension_count, extensions);
    context->AMD_gpu_shader_half_float = glad_vk_has_extension("VK_AMD_gpu_shader_half_float", extension_count, extensions);
    context->AMD_gpu_shader_int16 = glad_vk_has_extension("VK_AMD_gpu_shader_int16", extension_count, extensions);
    context->AMD_memory_overallocation_behavior = glad_vk_has_extension("VK_AMD_memory_overallocation_behavior", extension_count, extensions);
    context->AMD_mixed_attachment_samples = glad_vk_has_extension("VK_AMD_mixed_attachment_samples", extension_count, extensions);
    context->AMD_negative_viewport_height = glad_vk_has_extension("VK_AMD_negative_viewport_height", extension_count, extensions);
    context->AMD_pipeline_compiler_control = glad_vk_has_extension("VK_AMD_pipeline_compiler_control", extension_count, extensions);
    context->AMD_rasterization_order = glad_vk_has_extension("VK_AMD_rasterization_order", extension_count, extensions);
    context->AMD_shader_ballot = glad_vk_has_extension("VK_AMD_shader_ballot", extension_count, extensions);
    context->AMD_shader_core_properties = glad_vk_has_extension("VK_AMD_shader_core_properties", extension_count, extensions);
    context->AMD_shader_core_properties2 = glad_vk_has_extension("VK_AMD_shader_core_properties2", extension_count, extensions);
    context->AMD_shader_early_and_late_fragment_tests = glad_vk_has_extension("VK_AMD_shader_early_and_late_fragment_tests", extension_count, extensions);
    context->AMD_shader_explicit_vertex_parameter = glad_vk_has_extension("VK_AMD_shader_explicit_vertex_parameter", extension_count, extensions);
    context->AMD_shader_fragment_mask = glad_vk_has_extension("VK_AMD_shader_fragment_mask", extension_count, extensions);
    context->AMD_shader_image_load_store_lod = glad_vk_has_extension("VK_AMD_shader_image_load_store_lod", extension_count, extensions);
    context->AMD_shader_info = glad_vk_has_extension("VK_AMD_shader_info", extension_count, extensions);
    context->AMD_shader_trinary_minmax = glad_vk_has_extension("VK_AMD_shader_trinary_minmax", extension_count, extensions);
    context->AMD_texture_gather_bias_lod = glad_vk_has_extension("VK_AMD_texture_gather_bias_lod", extension_count, extensions);
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    context->ANDROID_external_format_resolve = glad_vk_has_extension("VK_ANDROID_external_format_resolve", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    context->ANDROID_external_memory_android_hardware_buffer = glad_vk_has_extension("VK_ANDROID_external_memory_android_hardware_buffer", extension_count, extensions);

#endif
    context->ARM_data_graph = glad_vk_has_extension("VK_ARM_data_graph", extension_count, extensions);
    context->ARM_format_pack = glad_vk_has_extension("VK_ARM_format_pack", extension_count, extensions);
    context->ARM_pipeline_opacity_micromap = glad_vk_has_extension("VK_ARM_pipeline_opacity_micromap", extension_count, extensions);
    context->ARM_rasterization_order_attachment_access = glad_vk_has_extension("VK_ARM_rasterization_order_attachment_access", extension_count, extensions);
    context->ARM_render_pass_striped = glad_vk_has_extension("VK_ARM_render_pass_striped", extension_count, extensions);
    context->ARM_scheduling_controls = glad_vk_has_extension("VK_ARM_scheduling_controls", extension_count, extensions);
    context->ARM_shader_core_builtins = glad_vk_has_extension("VK_ARM_shader_core_builtins", extension_count, extensions);
    context->ARM_shader_core_properties = glad_vk_has_extension("VK_ARM_shader_core_properties", extension_count, extensions);
    context->ARM_tensors = glad_vk_has_extension("VK_ARM_tensors", extension_count, extensions);
    context->EXT_4444_formats = glad_vk_has_extension("VK_EXT_4444_formats", extension_count, extensions);
    context->EXT_acquire_drm_display = glad_vk_has_extension("VK_EXT_acquire_drm_display", extension_count, extensions);
#if defined(VK_USE_PLATFORM_XLIB_XRANDR_EXT)
    context->EXT_acquire_xlib_display = glad_vk_has_extension("VK_EXT_acquire_xlib_display", extension_count, extensions);

#endif
    context->EXT_astc_decode_mode = glad_vk_has_extension("VK_EXT_astc_decode_mode", extension_count, extensions);
    context->EXT_attachment_feedback_loop_dynamic_state = glad_vk_has_extension("VK_EXT_attachment_feedback_loop_dynamic_state", extension_count, extensions);
    context->EXT_attachment_feedback_loop_layout = glad_vk_has_extension("VK_EXT_attachment_feedback_loop_layout", extension_count, extensions);
    context->EXT_blend_operation_advanced = glad_vk_has_extension("VK_EXT_blend_operation_advanced", extension_count, extensions);
    context->EXT_border_color_swizzle = glad_vk_has_extension("VK_EXT_border_color_swizzle", extension_count, extensions);
    context->EXT_buffer_device_address = glad_vk_has_extension("VK_EXT_buffer_device_address", extension_count, extensions);
    context->EXT_calibrated_timestamps = glad_vk_has_extension("VK_EXT_calibrated_timestamps", extension_count, extensions);
    context->EXT_color_write_enable = glad_vk_has_extension("VK_EXT_color_write_enable", extension_count, extensions);
    context->EXT_conditional_rendering = glad_vk_has_extension("VK_EXT_conditional_rendering", extension_count, extensions);
    context->EXT_conservative_rasterization = glad_vk_has_extension("VK_EXT_conservative_rasterization", extension_count, extensions);
    context->EXT_custom_border_color = glad_vk_has_extension("VK_EXT_custom_border_color", extension_count, extensions);
    context->EXT_debug_marker = glad_vk_has_extension("VK_EXT_debug_marker", extension_count, extensions);
    context->EXT_debug_report = glad_vk_has_extension("VK_EXT_debug_report", extension_count, extensions);
    context->EXT_debug_utils = glad_vk_has_extension("VK_EXT_debug_utils", extension_count, extensions);
    context->EXT_depth_bias_control = glad_vk_has_extension("VK_EXT_depth_bias_control", extension_count, extensions);
    context->EXT_depth_clamp_control = glad_vk_has_extension("VK_EXT_depth_clamp_control", extension_count, extensions);
    context->EXT_depth_clamp_zero_one = glad_vk_has_extension("VK_EXT_depth_clamp_zero_one", extension_count, extensions);
    context->EXT_depth_clip_control = glad_vk_has_extension("VK_EXT_depth_clip_control", extension_count, extensions);
    context->EXT_depth_clip_enable = glad_vk_has_extension("VK_EXT_depth_clip_enable", extension_count, extensions);
    context->EXT_depth_range_unrestricted = glad_vk_has_extension("VK_EXT_depth_range_unrestricted", extension_count, extensions);
    context->EXT_descriptor_buffer = glad_vk_has_extension("VK_EXT_descriptor_buffer", extension_count, extensions);
    context->EXT_descriptor_indexing = glad_vk_has_extension("VK_EXT_descriptor_indexing", extension_count, extensions);
    context->EXT_device_address_binding_report = glad_vk_has_extension("VK_EXT_device_address_binding_report", extension_count, extensions);
    context->EXT_device_fault = glad_vk_has_extension("VK_EXT_device_fault", extension_count, extensions);
    context->EXT_device_generated_commands = glad_vk_has_extension("VK_EXT_device_generated_commands", extension_count, extensions);
    context->EXT_device_memory_report = glad_vk_has_extension("VK_EXT_device_memory_report", extension_count, extensions);
    context->EXT_direct_mode_display = glad_vk_has_extension("VK_EXT_direct_mode_display", extension_count, extensions);
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
    context->EXT_directfb_surface = glad_vk_has_extension("VK_EXT_directfb_surface", extension_count, extensions);

#endif
    context->EXT_discard_rectangles = glad_vk_has_extension("VK_EXT_discard_rectangles", extension_count, extensions);
    context->EXT_display_control = glad_vk_has_extension("VK_EXT_display_control", extension_count, extensions);
    context->EXT_display_surface_counter = glad_vk_has_extension("VK_EXT_display_surface_counter", extension_count, extensions);
    context->EXT_dynamic_rendering_unused_attachments = glad_vk_has_extension("VK_EXT_dynamic_rendering_unused_attachments", extension_count, extensions);
    context->EXT_extended_dynamic_state = glad_vk_has_extension("VK_EXT_extended_dynamic_state", extension_count, extensions);
    context->EXT_extended_dynamic_state2 = glad_vk_has_extension("VK_EXT_extended_dynamic_state2", extension_count, extensions);
    context->EXT_extended_dynamic_state3 = glad_vk_has_extension("VK_EXT_extended_dynamic_state3", extension_count, extensions);
    context->EXT_external_memory_acquire_unmodified = glad_vk_has_extension("VK_EXT_external_memory_acquire_unmodified", extension_count, extensions);
    context->EXT_external_memory_dma_buf = glad_vk_has_extension("VK_EXT_external_memory_dma_buf", extension_count, extensions);
    context->EXT_external_memory_host = glad_vk_has_extension("VK_EXT_external_memory_host", extension_count, extensions);
#if defined(VK_USE_PLATFORM_METAL_EXT)
    context->EXT_external_memory_metal = glad_vk_has_extension("VK_EXT_external_memory_metal", extension_count, extensions);

#endif
    context->EXT_filter_cubic = glad_vk_has_extension("VK_EXT_filter_cubic", extension_count, extensions);
    context->EXT_fragment_density_map = glad_vk_has_extension("VK_EXT_fragment_density_map", extension_count, extensions);
    context->EXT_fragment_density_map2 = glad_vk_has_extension("VK_EXT_fragment_density_map2", extension_count, extensions);
    context->EXT_fragment_density_map_offset = glad_vk_has_extension("VK_EXT_fragment_density_map_offset", extension_count, extensions);
    context->EXT_fragment_shader_interlock = glad_vk_has_extension("VK_EXT_fragment_shader_interlock", extension_count, extensions);
    context->EXT_frame_boundary = glad_vk_has_extension("VK_EXT_frame_boundary", extension_count, extensions);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->EXT_full_screen_exclusive = glad_vk_has_extension("VK_EXT_full_screen_exclusive", extension_count, extensions);

#endif
    context->EXT_global_priority = glad_vk_has_extension("VK_EXT_global_priority", extension_count, extensions);
    context->EXT_global_priority_query = glad_vk_has_extension("VK_EXT_global_priority_query", extension_count, extensions);
    context->EXT_graphics_pipeline_library = glad_vk_has_extension("VK_EXT_graphics_pipeline_library", extension_count, extensions);
    context->EXT_hdr_metadata = glad_vk_has_extension("VK_EXT_hdr_metadata", extension_count, extensions);
    context->EXT_headless_surface = glad_vk_has_extension("VK_EXT_headless_surface", extension_count, extensions);
    context->EXT_host_image_copy = glad_vk_has_extension("VK_EXT_host_image_copy", extension_count, extensions);
    context->EXT_host_query_reset = glad_vk_has_extension("VK_EXT_host_query_reset", extension_count, extensions);
    context->EXT_image_2d_view_of_3d = glad_vk_has_extension("VK_EXT_image_2d_view_of_3d", extension_count, extensions);
    context->EXT_image_compression_control = glad_vk_has_extension("VK_EXT_image_compression_control", extension_count, extensions);
    context->EXT_image_compression_control_swapchain = glad_vk_has_extension("VK_EXT_image_compression_control_swapchain", extension_count, extensions);
    context->EXT_image_drm_format_modifier = glad_vk_has_extension("VK_EXT_image_drm_format_modifier", extension_count, extensions);
    context->EXT_image_robustness = glad_vk_has_extension("VK_EXT_image_robustness", extension_count, extensions);
    context->EXT_image_sliced_view_of_3d = glad_vk_has_extension("VK_EXT_image_sliced_view_of_3d", extension_count, extensions);
    context->EXT_image_view_min_lod = glad_vk_has_extension("VK_EXT_image_view_min_lod", extension_count, extensions);
    context->EXT_index_type_uint8 = glad_vk_has_extension("VK_EXT_index_type_uint8", extension_count, extensions);
    context->EXT_inline_uniform_block = glad_vk_has_extension("VK_EXT_inline_uniform_block", extension_count, extensions);
    context->EXT_layer_settings = glad_vk_has_extension("VK_EXT_layer_settings", extension_count, extensions);
    context->EXT_legacy_dithering = glad_vk_has_extension("VK_EXT_legacy_dithering", extension_count, extensions);
    context->EXT_legacy_vertex_attributes = glad_vk_has_extension("VK_EXT_legacy_vertex_attributes", extension_count, extensions);
    context->EXT_line_rasterization = glad_vk_has_extension("VK_EXT_line_rasterization", extension_count, extensions);
    context->EXT_load_store_op_none = glad_vk_has_extension("VK_EXT_load_store_op_none", extension_count, extensions);
    context->EXT_map_memory_placed = glad_vk_has_extension("VK_EXT_map_memory_placed", extension_count, extensions);
    context->EXT_memory_budget = glad_vk_has_extension("VK_EXT_memory_budget", extension_count, extensions);
    context->EXT_memory_priority = glad_vk_has_extension("VK_EXT_memory_priority", extension_count, extensions);
    context->EXT_mesh_shader = glad_vk_has_extension("VK_EXT_mesh_shader", extension_count, extensions);
#if defined(VK_USE_PLATFORM_METAL_EXT)
    context->EXT_metal_objects = glad_vk_has_extension("VK_EXT_metal_objects", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_METAL_EXT)
    context->EXT_metal_surface = glad_vk_has_extension("VK_EXT_metal_surface", extension_count, extensions);

#endif
    context->EXT_multi_draw = glad_vk_has_extension("VK_EXT_multi_draw", extension_count, extensions);
    context->EXT_multisampled_render_to_single_sampled = glad_vk_has_extension("VK_EXT_multisampled_render_to_single_sampled", extension_count, extensions);
    context->EXT_mutable_descriptor_type = glad_vk_has_extension("VK_EXT_mutable_descriptor_type", extension_count, extensions);
    context->EXT_nested_command_buffer = glad_vk_has_extension("VK_EXT_nested_command_buffer", extension_count, extensions);
    context->EXT_non_seamless_cube_map = glad_vk_has_extension("VK_EXT_non_seamless_cube_map", extension_count, extensions);
    context->EXT_opacity_micromap = glad_vk_has_extension("VK_EXT_opacity_micromap", extension_count, extensions);
    context->EXT_pageable_device_local_memory = glad_vk_has_extension("VK_EXT_pageable_device_local_memory", extension_count, extensions);
    context->EXT_pci_bus_info = glad_vk_has_extension("VK_EXT_pci_bus_info", extension_count, extensions);
    context->EXT_physical_device_drm = glad_vk_has_extension("VK_EXT_physical_device_drm", extension_count, extensions);
    context->EXT_pipeline_creation_cache_control = glad_vk_has_extension("VK_EXT_pipeline_creation_cache_control", extension_count, extensions);
    context->EXT_pipeline_creation_feedback = glad_vk_has_extension("VK_EXT_pipeline_creation_feedback", extension_count, extensions);
    context->EXT_pipeline_library_group_handles = glad_vk_has_extension("VK_EXT_pipeline_library_group_handles", extension_count, extensions);
    context->EXT_pipeline_properties = glad_vk_has_extension("VK_EXT_pipeline_properties", extension_count, extensions);
    context->EXT_pipeline_protected_access = glad_vk_has_extension("VK_EXT_pipeline_protected_access", extension_count, extensions);
    context->EXT_pipeline_robustness = glad_vk_has_extension("VK_EXT_pipeline_robustness", extension_count, extensions);
    context->EXT_post_depth_coverage = glad_vk_has_extension("VK_EXT_post_depth_coverage", extension_count, extensions);
    context->EXT_present_mode_fifo_latest_ready = glad_vk_has_extension("VK_EXT_present_mode_fifo_latest_ready", extension_count, extensions);
    context->EXT_primitive_topology_list_restart = glad_vk_has_extension("VK_EXT_primitive_topology_list_restart", extension_count, extensions);
    context->EXT_primitives_generated_query = glad_vk_has_extension("VK_EXT_primitives_generated_query", extension_count, extensions);
    context->EXT_private_data = glad_vk_has_extension("VK_EXT_private_data", extension_count, extensions);
    context->EXT_provoking_vertex = glad_vk_has_extension("VK_EXT_provoking_vertex", extension_count, extensions);
    context->EXT_queue_family_foreign = glad_vk_has_extension("VK_EXT_queue_family_foreign", extension_count, extensions);
    context->EXT_rasterization_order_attachment_access = glad_vk_has_extension("VK_EXT_rasterization_order_attachment_access", extension_count, extensions);
    context->EXT_rgba10x6_formats = glad_vk_has_extension("VK_EXT_rgba10x6_formats", extension_count, extensions);
    context->EXT_robustness2 = glad_vk_has_extension("VK_EXT_robustness2", extension_count, extensions);
    context->EXT_sample_locations = glad_vk_has_extension("VK_EXT_sample_locations", extension_count, extensions);
    context->EXT_sampler_filter_minmax = glad_vk_has_extension("VK_EXT_sampler_filter_minmax", extension_count, extensions);
    context->EXT_scalar_block_layout = glad_vk_has_extension("VK_EXT_scalar_block_layout", extension_count, extensions);
    context->EXT_separate_stencil_usage = glad_vk_has_extension("VK_EXT_separate_stencil_usage", extension_count, extensions);
    context->EXT_shader_atomic_float = glad_vk_has_extension("VK_EXT_shader_atomic_float", extension_count, extensions);
    context->EXT_shader_atomic_float2 = glad_vk_has_extension("VK_EXT_shader_atomic_float2", extension_count, extensions);
    context->EXT_shader_demote_to_helper_invocation = glad_vk_has_extension("VK_EXT_shader_demote_to_helper_invocation", extension_count, extensions);
    context->EXT_shader_float8 = glad_vk_has_extension("VK_EXT_shader_float8", extension_count, extensions);
    context->EXT_shader_image_atomic_int64 = glad_vk_has_extension("VK_EXT_shader_image_atomic_int64", extension_count, extensions);
    context->EXT_shader_module_identifier = glad_vk_has_extension("VK_EXT_shader_module_identifier", extension_count, extensions);
    context->EXT_shader_object = glad_vk_has_extension("VK_EXT_shader_object", extension_count, extensions);
    context->EXT_shader_replicated_composites = glad_vk_has_extension("VK_EXT_shader_replicated_composites", extension_count, extensions);
    context->EXT_shader_stencil_export = glad_vk_has_extension("VK_EXT_shader_stencil_export", extension_count, extensions);
    context->EXT_shader_subgroup_ballot = glad_vk_has_extension("VK_EXT_shader_subgroup_ballot", extension_count, extensions);
    context->EXT_shader_subgroup_vote = glad_vk_has_extension("VK_EXT_shader_subgroup_vote", extension_count, extensions);
    context->EXT_shader_tile_image = glad_vk_has_extension("VK_EXT_shader_tile_image", extension_count, extensions);
    context->EXT_shader_viewport_index_layer = glad_vk_has_extension("VK_EXT_shader_viewport_index_layer", extension_count, extensions);
    context->EXT_subgroup_size_control = glad_vk_has_extension("VK_EXT_subgroup_size_control", extension_count, extensions);
    context->EXT_subpass_merge_feedback = glad_vk_has_extension("VK_EXT_subpass_merge_feedback", extension_count, extensions);
    context->EXT_surface_maintenance1 = glad_vk_has_extension("VK_EXT_surface_maintenance1", extension_count, extensions);
    context->EXT_swapchain_colorspace = glad_vk_has_extension("VK_EXT_swapchain_colorspace", extension_count, extensions);
    context->EXT_swapchain_maintenance1 = glad_vk_has_extension("VK_EXT_swapchain_maintenance1", extension_count, extensions);
    context->EXT_texel_buffer_alignment = glad_vk_has_extension("VK_EXT_texel_buffer_alignment", extension_count, extensions);
    context->EXT_texture_compression_astc_hdr = glad_vk_has_extension("VK_EXT_texture_compression_astc_hdr", extension_count, extensions);
    context->EXT_tooling_info = glad_vk_has_extension("VK_EXT_tooling_info", extension_count, extensions);
    context->EXT_transform_feedback = glad_vk_has_extension("VK_EXT_transform_feedback", extension_count, extensions);
    context->EXT_validation_cache = glad_vk_has_extension("VK_EXT_validation_cache", extension_count, extensions);
    context->EXT_validation_features = glad_vk_has_extension("VK_EXT_validation_features", extension_count, extensions);
    context->EXT_validation_flags = glad_vk_has_extension("VK_EXT_validation_flags", extension_count, extensions);
    context->EXT_vertex_attribute_divisor = glad_vk_has_extension("VK_EXT_vertex_attribute_divisor", extension_count, extensions);
    context->EXT_vertex_attribute_robustness = glad_vk_has_extension("VK_EXT_vertex_attribute_robustness", extension_count, extensions);
    context->EXT_vertex_input_dynamic_state = glad_vk_has_extension("VK_EXT_vertex_input_dynamic_state", extension_count, extensions);
    context->EXT_ycbcr_2plane_444_formats = glad_vk_has_extension("VK_EXT_ycbcr_2plane_444_formats", extension_count, extensions);
    context->EXT_ycbcr_image_arrays = glad_vk_has_extension("VK_EXT_ycbcr_image_arrays", extension_count, extensions);
    context->EXT_zero_initialize_device_memory = glad_vk_has_extension("VK_EXT_zero_initialize_device_memory", extension_count, extensions);
#if defined(VK_USE_PLATFORM_FUCHSIA)
    context->FUCHSIA_buffer_collection = glad_vk_has_extension("VK_FUCHSIA_buffer_collection", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
    context->FUCHSIA_external_memory = glad_vk_has_extension("VK_FUCHSIA_external_memory", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
    context->FUCHSIA_external_semaphore = glad_vk_has_extension("VK_FUCHSIA_external_semaphore", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
    context->FUCHSIA_imagepipe_surface = glad_vk_has_extension("VK_FUCHSIA_imagepipe_surface", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_GGP)
    context->GGP_frame_token = glad_vk_has_extension("VK_GGP_frame_token", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_GGP)
    context->GGP_stream_descriptor_surface = glad_vk_has_extension("VK_GGP_stream_descriptor_surface", extension_count, extensions);

#endif
    context->GOOGLE_decorate_string = glad_vk_has_extension("VK_GOOGLE_decorate_string", extension_count, extensions);
    context->GOOGLE_display_timing = glad_vk_has_extension("VK_GOOGLE_display_timing", extension_count, extensions);
    context->GOOGLE_hlsl_functionality1 = glad_vk_has_extension("VK_GOOGLE_hlsl_functionality1", extension_count, extensions);
    context->GOOGLE_surfaceless_query = glad_vk_has_extension("VK_GOOGLE_surfaceless_query", extension_count, extensions);
    context->GOOGLE_user_type = glad_vk_has_extension("VK_GOOGLE_user_type", extension_count, extensions);
    context->HUAWEI_cluster_culling_shader = glad_vk_has_extension("VK_HUAWEI_cluster_culling_shader", extension_count, extensions);
    context->HUAWEI_hdr_vivid = glad_vk_has_extension("VK_HUAWEI_hdr_vivid", extension_count, extensions);
    context->HUAWEI_invocation_mask = glad_vk_has_extension("VK_HUAWEI_invocation_mask", extension_count, extensions);
    context->HUAWEI_subpass_shading = glad_vk_has_extension("VK_HUAWEI_subpass_shading", extension_count, extensions);
    context->IMG_filter_cubic = glad_vk_has_extension("VK_IMG_filter_cubic", extension_count, extensions);
    context->IMG_format_pvrtc = glad_vk_has_extension("VK_IMG_format_pvrtc", extension_count, extensions);
    context->IMG_relaxed_line_rasterization = glad_vk_has_extension("VK_IMG_relaxed_line_rasterization", extension_count, extensions);
    context->INTEL_performance_query = glad_vk_has_extension("VK_INTEL_performance_query", extension_count, extensions);
    context->INTEL_shader_integer_functions2 = glad_vk_has_extension("VK_INTEL_shader_integer_functions2", extension_count, extensions);
    context->KHR_16bit_storage = glad_vk_has_extension("VK_KHR_16bit_storage", extension_count, extensions);
    context->KHR_8bit_storage = glad_vk_has_extension("VK_KHR_8bit_storage", extension_count, extensions);
    context->KHR_acceleration_structure = glad_vk_has_extension("VK_KHR_acceleration_structure", extension_count, extensions);
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    context->KHR_android_surface = glad_vk_has_extension("VK_KHR_android_surface", extension_count, extensions);

#endif
    context->KHR_bind_memory2 = glad_vk_has_extension("VK_KHR_bind_memory2", extension_count, extensions);
    context->KHR_buffer_device_address = glad_vk_has_extension("VK_KHR_buffer_device_address", extension_count, extensions);
    context->KHR_calibrated_timestamps = glad_vk_has_extension("VK_KHR_calibrated_timestamps", extension_count, extensions);
    context->KHR_compute_shader_derivatives = glad_vk_has_extension("VK_KHR_compute_shader_derivatives", extension_count, extensions);
    context->KHR_cooperative_matrix = glad_vk_has_extension("VK_KHR_cooperative_matrix", extension_count, extensions);
    context->KHR_copy_commands2 = glad_vk_has_extension("VK_KHR_copy_commands2", extension_count, extensions);
    context->KHR_create_renderpass2 = glad_vk_has_extension("VK_KHR_create_renderpass2", extension_count, extensions);
    context->KHR_dedicated_allocation = glad_vk_has_extension("VK_KHR_dedicated_allocation", extension_count, extensions);
    context->KHR_deferred_host_operations = glad_vk_has_extension("VK_KHR_deferred_host_operations", extension_count, extensions);
    context->KHR_depth_clamp_zero_one = glad_vk_has_extension("VK_KHR_depth_clamp_zero_one", extension_count, extensions);
    context->KHR_depth_stencil_resolve = glad_vk_has_extension("VK_KHR_depth_stencil_resolve", extension_count, extensions);
    context->KHR_descriptor_update_template = glad_vk_has_extension("VK_KHR_descriptor_update_template", extension_count, extensions);
    context->KHR_device_group = glad_vk_has_extension("VK_KHR_device_group", extension_count, extensions);
    context->KHR_device_group_creation = glad_vk_has_extension("VK_KHR_device_group_creation", extension_count, extensions);
    context->KHR_display = glad_vk_has_extension("VK_KHR_display", extension_count, extensions);
    context->KHR_display_swapchain = glad_vk_has_extension("VK_KHR_display_swapchain", extension_count, extensions);
    context->KHR_draw_indirect_count = glad_vk_has_extension("VK_KHR_draw_indirect_count", extension_count, extensions);
    context->KHR_driver_properties = glad_vk_has_extension("VK_KHR_driver_properties", extension_count, extensions);
    context->KHR_dynamic_rendering = glad_vk_has_extension("VK_KHR_dynamic_rendering", extension_count, extensions);
    context->KHR_dynamic_rendering_local_read = glad_vk_has_extension("VK_KHR_dynamic_rendering_local_read", extension_count, extensions);
    context->KHR_external_fence = glad_vk_has_extension("VK_KHR_external_fence", extension_count, extensions);
    context->KHR_external_fence_capabilities = glad_vk_has_extension("VK_KHR_external_fence_capabilities", extension_count, extensions);
    context->KHR_external_fence_fd = glad_vk_has_extension("VK_KHR_external_fence_fd", extension_count, extensions);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->KHR_external_fence_win32 = glad_vk_has_extension("VK_KHR_external_fence_win32", extension_count, extensions);

#endif
    context->KHR_external_memory = glad_vk_has_extension("VK_KHR_external_memory", extension_count, extensions);
    context->KHR_external_memory_capabilities = glad_vk_has_extension("VK_KHR_external_memory_capabilities", extension_count, extensions);
    context->KHR_external_memory_fd = glad_vk_has_extension("VK_KHR_external_memory_fd", extension_count, extensions);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->KHR_external_memory_win32 = glad_vk_has_extension("VK_KHR_external_memory_win32", extension_count, extensions);

#endif
    context->KHR_external_semaphore = glad_vk_has_extension("VK_KHR_external_semaphore", extension_count, extensions);
    context->KHR_external_semaphore_capabilities = glad_vk_has_extension("VK_KHR_external_semaphore_capabilities", extension_count, extensions);
    context->KHR_external_semaphore_fd = glad_vk_has_extension("VK_KHR_external_semaphore_fd", extension_count, extensions);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->KHR_external_semaphore_win32 = glad_vk_has_extension("VK_KHR_external_semaphore_win32", extension_count, extensions);

#endif
    context->KHR_format_feature_flags2 = glad_vk_has_extension("VK_KHR_format_feature_flags2", extension_count, extensions);
    context->KHR_fragment_shader_barycentric = glad_vk_has_extension("VK_KHR_fragment_shader_barycentric", extension_count, extensions);
    context->KHR_fragment_shading_rate = glad_vk_has_extension("VK_KHR_fragment_shading_rate", extension_count, extensions);
    context->KHR_get_display_properties2 = glad_vk_has_extension("VK_KHR_get_display_properties2", extension_count, extensions);
    context->KHR_get_memory_requirements2 = glad_vk_has_extension("VK_KHR_get_memory_requirements2", extension_count, extensions);
    context->KHR_get_physical_device_properties2 = glad_vk_has_extension("VK_KHR_get_physical_device_properties2", extension_count, extensions);
    context->KHR_get_surface_capabilities2 = glad_vk_has_extension("VK_KHR_get_surface_capabilities2", extension_count, extensions);
    context->KHR_global_priority = glad_vk_has_extension("VK_KHR_global_priority", extension_count, extensions);
    context->KHR_image_format_list = glad_vk_has_extension("VK_KHR_image_format_list", extension_count, extensions);
    context->KHR_imageless_framebuffer = glad_vk_has_extension("VK_KHR_imageless_framebuffer", extension_count, extensions);
    context->KHR_incremental_present = glad_vk_has_extension("VK_KHR_incremental_present", extension_count, extensions);
    context->KHR_index_type_uint8 = glad_vk_has_extension("VK_KHR_index_type_uint8", extension_count, extensions);
    context->KHR_line_rasterization = glad_vk_has_extension("VK_KHR_line_rasterization", extension_count, extensions);
    context->KHR_load_store_op_none = glad_vk_has_extension("VK_KHR_load_store_op_none", extension_count, extensions);
    context->KHR_maintenance1 = glad_vk_has_extension("VK_KHR_maintenance1", extension_count, extensions);
    context->KHR_maintenance2 = glad_vk_has_extension("VK_KHR_maintenance2", extension_count, extensions);
    context->KHR_maintenance3 = glad_vk_has_extension("VK_KHR_maintenance3", extension_count, extensions);
    context->KHR_maintenance4 = glad_vk_has_extension("VK_KHR_maintenance4", extension_count, extensions);
    context->KHR_maintenance5 = glad_vk_has_extension("VK_KHR_maintenance5", extension_count, extensions);
    context->KHR_maintenance6 = glad_vk_has_extension("VK_KHR_maintenance6", extension_count, extensions);
    context->KHR_maintenance7 = glad_vk_has_extension("VK_KHR_maintenance7", extension_count, extensions);
    context->KHR_maintenance8 = glad_vk_has_extension("VK_KHR_maintenance8", extension_count, extensions);
    context->KHR_maintenance9 = glad_vk_has_extension("VK_KHR_maintenance9", extension_count, extensions);
    context->KHR_map_memory2 = glad_vk_has_extension("VK_KHR_map_memory2", extension_count, extensions);
    context->KHR_multiview = glad_vk_has_extension("VK_KHR_multiview", extension_count, extensions);
    context->KHR_performance_query = glad_vk_has_extension("VK_KHR_performance_query", extension_count, extensions);
    context->KHR_pipeline_binary = glad_vk_has_extension("VK_KHR_pipeline_binary", extension_count, extensions);
    context->KHR_pipeline_executable_properties = glad_vk_has_extension("VK_KHR_pipeline_executable_properties", extension_count, extensions);
    context->KHR_pipeline_library = glad_vk_has_extension("VK_KHR_pipeline_library", extension_count, extensions);
    context->KHR_portability_enumeration = glad_vk_has_extension("VK_KHR_portability_enumeration", extension_count, extensions);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    context->KHR_portability_subset = glad_vk_has_extension("VK_KHR_portability_subset", extension_count, extensions);

#endif
    context->KHR_present_id = glad_vk_has_extension("VK_KHR_present_id", extension_count, extensions);
    context->KHR_present_id2 = glad_vk_has_extension("VK_KHR_present_id2", extension_count, extensions);
    context->KHR_present_wait = glad_vk_has_extension("VK_KHR_present_wait", extension_count, extensions);
    context->KHR_present_wait2 = glad_vk_has_extension("VK_KHR_present_wait2", extension_count, extensions);
    context->KHR_push_descriptor = glad_vk_has_extension("VK_KHR_push_descriptor", extension_count, extensions);
    context->KHR_ray_query = glad_vk_has_extension("VK_KHR_ray_query", extension_count, extensions);
    context->KHR_ray_tracing_maintenance1 = glad_vk_has_extension("VK_KHR_ray_tracing_maintenance1", extension_count, extensions);
    context->KHR_ray_tracing_pipeline = glad_vk_has_extension("VK_KHR_ray_tracing_pipeline", extension_count, extensions);
    context->KHR_ray_tracing_position_fetch = glad_vk_has_extension("VK_KHR_ray_tracing_position_fetch", extension_count, extensions);
    context->KHR_relaxed_block_layout = glad_vk_has_extension("VK_KHR_relaxed_block_layout", extension_count, extensions);
    context->KHR_robustness2 = glad_vk_has_extension("VK_KHR_robustness2", extension_count, extensions);
    context->KHR_sampler_mirror_clamp_to_edge = glad_vk_has_extension("VK_KHR_sampler_mirror_clamp_to_edge", extension_count, extensions);
    context->KHR_sampler_ycbcr_conversion = glad_vk_has_extension("VK_KHR_sampler_ycbcr_conversion", extension_count, extensions);
    context->KHR_separate_depth_stencil_layouts = glad_vk_has_extension("VK_KHR_separate_depth_stencil_layouts", extension_count, extensions);
    context->KHR_shader_atomic_int64 = glad_vk_has_extension("VK_KHR_shader_atomic_int64", extension_count, extensions);
    context->KHR_shader_bfloat16 = glad_vk_has_extension("VK_KHR_shader_bfloat16", extension_count, extensions);
    context->KHR_shader_clock = glad_vk_has_extension("VK_KHR_shader_clock", extension_count, extensions);
    context->KHR_shader_draw_parameters = glad_vk_has_extension("VK_KHR_shader_draw_parameters", extension_count, extensions);
    context->KHR_shader_expect_assume = glad_vk_has_extension("VK_KHR_shader_expect_assume", extension_count, extensions);
    context->KHR_shader_float16_int8 = glad_vk_has_extension("VK_KHR_shader_float16_int8", extension_count, extensions);
    context->KHR_shader_float_controls = glad_vk_has_extension("VK_KHR_shader_float_controls", extension_count, extensions);
    context->KHR_shader_float_controls2 = glad_vk_has_extension("VK_KHR_shader_float_controls2", extension_count, extensions);
    context->KHR_shader_integer_dot_product = glad_vk_has_extension("VK_KHR_shader_integer_dot_product", extension_count, extensions);
    context->KHR_shader_maximal_reconvergence = glad_vk_has_extension("VK_KHR_shader_maximal_reconvergence", extension_count, extensions);
    context->KHR_shader_non_semantic_info = glad_vk_has_extension("VK_KHR_shader_non_semantic_info", extension_count, extensions);
    context->KHR_shader_quad_control = glad_vk_has_extension("VK_KHR_shader_quad_control", extension_count, extensions);
    context->KHR_shader_relaxed_extended_instruction = glad_vk_has_extension("VK_KHR_shader_relaxed_extended_instruction", extension_count, extensions);
    context->KHR_shader_subgroup_extended_types = glad_vk_has_extension("VK_KHR_shader_subgroup_extended_types", extension_count, extensions);
    context->KHR_shader_subgroup_rotate = glad_vk_has_extension("VK_KHR_shader_subgroup_rotate", extension_count, extensions);
    context->KHR_shader_subgroup_uniform_control_flow = glad_vk_has_extension("VK_KHR_shader_subgroup_uniform_control_flow", extension_count, extensions);
    context->KHR_shader_terminate_invocation = glad_vk_has_extension("VK_KHR_shader_terminate_invocation", extension_count, extensions);
    context->KHR_shared_presentable_image = glad_vk_has_extension("VK_KHR_shared_presentable_image", extension_count, extensions);
    context->KHR_spirv_1_4 = glad_vk_has_extension("VK_KHR_spirv_1_4", extension_count, extensions);
    context->KHR_storage_buffer_storage_class = glad_vk_has_extension("VK_KHR_storage_buffer_storage_class", extension_count, extensions);
    context->KHR_surface = glad_vk_has_extension("VK_KHR_surface", extension_count, extensions);
    context->KHR_surface_protected_capabilities = glad_vk_has_extension("VK_KHR_surface_protected_capabilities", extension_count, extensions);
    context->KHR_swapchain = glad_vk_has_extension("VK_KHR_swapchain", extension_count, extensions);
    context->KHR_swapchain_mutable_format = glad_vk_has_extension("VK_KHR_swapchain_mutable_format", extension_count, extensions);
    context->KHR_synchronization2 = glad_vk_has_extension("VK_KHR_synchronization2", extension_count, extensions);
    context->KHR_timeline_semaphore = glad_vk_has_extension("VK_KHR_timeline_semaphore", extension_count, extensions);
    context->KHR_unified_image_layouts = glad_vk_has_extension("VK_KHR_unified_image_layouts", extension_count, extensions);
    context->KHR_uniform_buffer_standard_layout = glad_vk_has_extension("VK_KHR_uniform_buffer_standard_layout", extension_count, extensions);
    context->KHR_variable_pointers = glad_vk_has_extension("VK_KHR_variable_pointers", extension_count, extensions);
    context->KHR_vertex_attribute_divisor = glad_vk_has_extension("VK_KHR_vertex_attribute_divisor", extension_count, extensions);
    context->KHR_video_decode_av1 = glad_vk_has_extension("VK_KHR_video_decode_av1", extension_count, extensions);
    context->KHR_video_decode_h264 = glad_vk_has_extension("VK_KHR_video_decode_h264", extension_count, extensions);
    context->KHR_video_decode_h265 = glad_vk_has_extension("VK_KHR_video_decode_h265", extension_count, extensions);
    context->KHR_video_decode_queue = glad_vk_has_extension("VK_KHR_video_decode_queue", extension_count, extensions);
    context->KHR_video_decode_vp9 = glad_vk_has_extension("VK_KHR_video_decode_vp9", extension_count, extensions);
    context->KHR_video_encode_av1 = glad_vk_has_extension("VK_KHR_video_encode_av1", extension_count, extensions);
    context->KHR_video_encode_h264 = glad_vk_has_extension("VK_KHR_video_encode_h264", extension_count, extensions);
    context->KHR_video_encode_h265 = glad_vk_has_extension("VK_KHR_video_encode_h265", extension_count, extensions);
    context->KHR_video_encode_quantization_map = glad_vk_has_extension("VK_KHR_video_encode_quantization_map", extension_count, extensions);
    context->KHR_video_encode_queue = glad_vk_has_extension("VK_KHR_video_encode_queue", extension_count, extensions);
    context->KHR_video_maintenance1 = glad_vk_has_extension("VK_KHR_video_maintenance1", extension_count, extensions);
    context->KHR_video_maintenance2 = glad_vk_has_extension("VK_KHR_video_maintenance2", extension_count, extensions);
    context->KHR_video_queue = glad_vk_has_extension("VK_KHR_video_queue", extension_count, extensions);
    context->KHR_vulkan_memory_model = glad_vk_has_extension("VK_KHR_vulkan_memory_model", extension_count, extensions);
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    context->KHR_wayland_surface = glad_vk_has_extension("VK_KHR_wayland_surface", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->KHR_win32_keyed_mutex = glad_vk_has_extension("VK_KHR_win32_keyed_mutex", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->KHR_win32_surface = glad_vk_has_extension("VK_KHR_win32_surface", extension_count, extensions);

#endif
    context->KHR_workgroup_memory_explicit_layout = glad_vk_has_extension("VK_KHR_workgroup_memory_explicit_layout", extension_count, extensions);
#if defined(VK_USE_PLATFORM_XCB_KHR)
    context->KHR_xcb_surface = glad_vk_has_extension("VK_KHR_xcb_surface", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    context->KHR_xlib_surface = glad_vk_has_extension("VK_KHR_xlib_surface", extension_count, extensions);

#endif
    context->KHR_zero_initialize_workgroup_memory = glad_vk_has_extension("VK_KHR_zero_initialize_workgroup_memory", extension_count, extensions);
    context->LUNARG_direct_driver_loading = glad_vk_has_extension("VK_LUNARG_direct_driver_loading", extension_count, extensions);
    context->MESA_image_alignment_control = glad_vk_has_extension("VK_MESA_image_alignment_control", extension_count, extensions);
    context->MSFT_layered_driver = glad_vk_has_extension("VK_MSFT_layered_driver", extension_count, extensions);
#if defined(VK_USE_PLATFORM_IOS_MVK)
    context->MVK_ios_surface = glad_vk_has_extension("VK_MVK_ios_surface", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_MACOS_MVK)
    context->MVK_macos_surface = glad_vk_has_extension("VK_MVK_macos_surface", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_VI_NN)
    context->NN_vi_surface = glad_vk_has_extension("VK_NN_vi_surface", extension_count, extensions);

#endif
    context->NVX_binary_import = glad_vk_has_extension("VK_NVX_binary_import", extension_count, extensions);
    context->NVX_image_view_handle = glad_vk_has_extension("VK_NVX_image_view_handle", extension_count, extensions);
    context->NVX_multiview_per_view_attributes = glad_vk_has_extension("VK_NVX_multiview_per_view_attributes", extension_count, extensions);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->NV_acquire_winrt_display = glad_vk_has_extension("VK_NV_acquire_winrt_display", extension_count, extensions);

#endif
    context->NV_clip_space_w_scaling = glad_vk_has_extension("VK_NV_clip_space_w_scaling", extension_count, extensions);
    context->NV_cluster_acceleration_structure = glad_vk_has_extension("VK_NV_cluster_acceleration_structure", extension_count, extensions);
    context->NV_command_buffer_inheritance = glad_vk_has_extension("VK_NV_command_buffer_inheritance", extension_count, extensions);
    context->NV_compute_shader_derivatives = glad_vk_has_extension("VK_NV_compute_shader_derivatives", extension_count, extensions);
    context->NV_cooperative_matrix = glad_vk_has_extension("VK_NV_cooperative_matrix", extension_count, extensions);
    context->NV_cooperative_matrix2 = glad_vk_has_extension("VK_NV_cooperative_matrix2", extension_count, extensions);
    context->NV_cooperative_vector = glad_vk_has_extension("VK_NV_cooperative_vector", extension_count, extensions);
    context->NV_copy_memory_indirect = glad_vk_has_extension("VK_NV_copy_memory_indirect", extension_count, extensions);
    context->NV_corner_sampled_image = glad_vk_has_extension("VK_NV_corner_sampled_image", extension_count, extensions);
    context->NV_coverage_reduction_mode = glad_vk_has_extension("VK_NV_coverage_reduction_mode", extension_count, extensions);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    context->NV_cuda_kernel_launch = glad_vk_has_extension("VK_NV_cuda_kernel_launch", extension_count, extensions);

#endif
    context->NV_dedicated_allocation = glad_vk_has_extension("VK_NV_dedicated_allocation", extension_count, extensions);
    context->NV_dedicated_allocation_image_aliasing = glad_vk_has_extension("VK_NV_dedicated_allocation_image_aliasing", extension_count, extensions);
    context->NV_descriptor_pool_overallocation = glad_vk_has_extension("VK_NV_descriptor_pool_overallocation", extension_count, extensions);
    context->NV_device_diagnostic_checkpoints = glad_vk_has_extension("VK_NV_device_diagnostic_checkpoints", extension_count, extensions);
    context->NV_device_diagnostics_config = glad_vk_has_extension("VK_NV_device_diagnostics_config", extension_count, extensions);
    context->NV_device_generated_commands = glad_vk_has_extension("VK_NV_device_generated_commands", extension_count, extensions);
    context->NV_device_generated_commands_compute = glad_vk_has_extension("VK_NV_device_generated_commands_compute", extension_count, extensions);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    context->NV_displacement_micromap = glad_vk_has_extension("VK_NV_displacement_micromap", extension_count, extensions);

#endif
    context->NV_display_stereo = glad_vk_has_extension("VK_NV_display_stereo", extension_count, extensions);
    context->NV_extended_sparse_address_space = glad_vk_has_extension("VK_NV_extended_sparse_address_space", extension_count, extensions);
    context->NV_external_compute_queue = glad_vk_has_extension("VK_NV_external_compute_queue", extension_count, extensions);
    context->NV_external_memory = glad_vk_has_extension("VK_NV_external_memory", extension_count, extensions);
    context->NV_external_memory_capabilities = glad_vk_has_extension("VK_NV_external_memory_capabilities", extension_count, extensions);
    context->NV_external_memory_rdma = glad_vk_has_extension("VK_NV_external_memory_rdma", extension_count, extensions);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->NV_external_memory_win32 = glad_vk_has_extension("VK_NV_external_memory_win32", extension_count, extensions);

#endif
    context->NV_fill_rectangle = glad_vk_has_extension("VK_NV_fill_rectangle", extension_count, extensions);
    context->NV_fragment_coverage_to_color = glad_vk_has_extension("VK_NV_fragment_coverage_to_color", extension_count, extensions);
    context->NV_fragment_shader_barycentric = glad_vk_has_extension("VK_NV_fragment_shader_barycentric", extension_count, extensions);
    context->NV_fragment_shading_rate_enums = glad_vk_has_extension("VK_NV_fragment_shading_rate_enums", extension_count, extensions);
    context->NV_framebuffer_mixed_samples = glad_vk_has_extension("VK_NV_framebuffer_mixed_samples", extension_count, extensions);
    context->NV_geometry_shader_passthrough = glad_vk_has_extension("VK_NV_geometry_shader_passthrough", extension_count, extensions);
    context->NV_glsl_shader = glad_vk_has_extension("VK_NV_glsl_shader", extension_count, extensions);
    context->NV_inherited_viewport_scissor = glad_vk_has_extension("VK_NV_inherited_viewport_scissor", extension_count, extensions);
    context->NV_linear_color_attachment = glad_vk_has_extension("VK_NV_linear_color_attachment", extension_count, extensions);
    context->NV_low_latency = glad_vk_has_extension("VK_NV_low_latency", extension_count, extensions);
    context->NV_low_latency2 = glad_vk_has_extension("VK_NV_low_latency2", extension_count, extensions);
    context->NV_memory_decompression = glad_vk_has_extension("VK_NV_memory_decompression", extension_count, extensions);
    context->NV_mesh_shader = glad_vk_has_extension("VK_NV_mesh_shader", extension_count, extensions);
    context->NV_optical_flow = glad_vk_has_extension("VK_NV_optical_flow", extension_count, extensions);
    context->NV_partitioned_acceleration_structure = glad_vk_has_extension("VK_NV_partitioned_acceleration_structure", extension_count, extensions);
    context->NV_per_stage_descriptor_set = glad_vk_has_extension("VK_NV_per_stage_descriptor_set", extension_count, extensions);
    context->NV_present_barrier = glad_vk_has_extension("VK_NV_present_barrier", extension_count, extensions);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    context->NV_present_metering = glad_vk_has_extension("VK_NV_present_metering", extension_count, extensions);

#endif
    context->NV_raw_access_chains = glad_vk_has_extension("VK_NV_raw_access_chains", extension_count, extensions);
    context->NV_ray_tracing = glad_vk_has_extension("VK_NV_ray_tracing", extension_count, extensions);
    context->NV_ray_tracing_invocation_reorder = glad_vk_has_extension("VK_NV_ray_tracing_invocation_reorder", extension_count, extensions);
    context->NV_ray_tracing_linear_swept_spheres = glad_vk_has_extension("VK_NV_ray_tracing_linear_swept_spheres", extension_count, extensions);
    context->NV_ray_tracing_motion_blur = glad_vk_has_extension("VK_NV_ray_tracing_motion_blur", extension_count, extensions);
    context->NV_ray_tracing_validation = glad_vk_has_extension("VK_NV_ray_tracing_validation", extension_count, extensions);
    context->NV_representative_fragment_test = glad_vk_has_extension("VK_NV_representative_fragment_test", extension_count, extensions);
    context->NV_sample_mask_override_coverage = glad_vk_has_extension("VK_NV_sample_mask_override_coverage", extension_count, extensions);
    context->NV_scissor_exclusive = glad_vk_has_extension("VK_NV_scissor_exclusive", extension_count, extensions);
    context->NV_shader_atomic_float16_vector = glad_vk_has_extension("VK_NV_shader_atomic_float16_vector", extension_count, extensions);
    context->NV_shader_image_footprint = glad_vk_has_extension("VK_NV_shader_image_footprint", extension_count, extensions);
    context->NV_shader_sm_builtins = glad_vk_has_extension("VK_NV_shader_sm_builtins", extension_count, extensions);
    context->NV_shader_subgroup_partitioned = glad_vk_has_extension("VK_NV_shader_subgroup_partitioned", extension_count, extensions);
    context->NV_shading_rate_image = glad_vk_has_extension("VK_NV_shading_rate_image", extension_count, extensions);
    context->NV_viewport_array2 = glad_vk_has_extension("VK_NV_viewport_array2", extension_count, extensions);
    context->NV_viewport_swizzle = glad_vk_has_extension("VK_NV_viewport_swizzle", extension_count, extensions);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->NV_win32_keyed_mutex = glad_vk_has_extension("VK_NV_win32_keyed_mutex", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_OHOS)
    context->OHOS_surface = glad_vk_has_extension("VK_OHOS_surface", extension_count, extensions);

#endif
    context->QCOM_filter_cubic_clamp = glad_vk_has_extension("VK_QCOM_filter_cubic_clamp", extension_count, extensions);
    context->QCOM_filter_cubic_weights = glad_vk_has_extension("VK_QCOM_filter_cubic_weights", extension_count, extensions);
    context->QCOM_fragment_density_map_offset = glad_vk_has_extension("VK_QCOM_fragment_density_map_offset", extension_count, extensions);
    context->QCOM_image_processing = glad_vk_has_extension("VK_QCOM_image_processing", extension_count, extensions);
    context->QCOM_image_processing2 = glad_vk_has_extension("VK_QCOM_image_processing2", extension_count, extensions);
    context->QCOM_multiview_per_view_render_areas = glad_vk_has_extension("VK_QCOM_multiview_per_view_render_areas", extension_count, extensions);
    context->QCOM_multiview_per_view_viewports = glad_vk_has_extension("VK_QCOM_multiview_per_view_viewports", extension_count, extensions);
    context->QCOM_render_pass_shader_resolve = glad_vk_has_extension("VK_QCOM_render_pass_shader_resolve", extension_count, extensions);
    context->QCOM_render_pass_store_ops = glad_vk_has_extension("VK_QCOM_render_pass_store_ops", extension_count, extensions);
    context->QCOM_render_pass_transform = glad_vk_has_extension("VK_QCOM_render_pass_transform", extension_count, extensions);
    context->QCOM_rotated_copy_commands = glad_vk_has_extension("VK_QCOM_rotated_copy_commands", extension_count, extensions);
    context->QCOM_tile_memory_heap = glad_vk_has_extension("VK_QCOM_tile_memory_heap", extension_count, extensions);
    context->QCOM_tile_properties = glad_vk_has_extension("VK_QCOM_tile_properties", extension_count, extensions);
    context->QCOM_tile_shading = glad_vk_has_extension("VK_QCOM_tile_shading", extension_count, extensions);
    context->QCOM_ycbcr_degamma = glad_vk_has_extension("VK_QCOM_ycbcr_degamma", extension_count, extensions);
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
    context->QNX_external_memory_screen_buffer = glad_vk_has_extension("VK_QNX_external_memory_screen_buffer", extension_count, extensions);

#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
    context->QNX_screen_surface = glad_vk_has_extension("VK_QNX_screen_surface", extension_count, extensions);

#endif
    context->SEC_amigo_profiling = glad_vk_has_extension("VK_SEC_amigo_profiling", extension_count, extensions);
    context->VALVE_descriptor_set_host_mapping = glad_vk_has_extension("VK_VALVE_descriptor_set_host_mapping", extension_count, extensions);
    context->VALVE_fragment_density_map_layered = glad_vk_has_extension("VK_VALVE_fragment_density_map_layered", extension_count, extensions);
    context->VALVE_mutable_descriptor_type = glad_vk_has_extension("VK_VALVE_mutable_descriptor_type", extension_count, extensions);

    GLAD_UNUSED(&glad_vk_has_extension);

    glad_vk_free_extensions(extension_count, extensions);

    return 1;
}

static int glad_vk_find_core_vulkan(GladVulkanContext *context, VkPhysicalDevice physical_device) {
    int major = 1;
    int minor = 0;

#ifdef VK_VERSION_1_1
    if (context->EnumerateInstanceVersion != NULL) {
        uint32_t version;
        VkResult result;

        result = context->EnumerateInstanceVersion(&version);
        if (result == VK_SUCCESS) {
            major = (int) VK_VERSION_MAJOR(version);
            minor = (int) VK_VERSION_MINOR(version);
        }
    }
#endif

    if (physical_device != NULL && context->GetPhysicalDeviceProperties != NULL) {
        VkPhysicalDeviceProperties properties;
        context->GetPhysicalDeviceProperties(physical_device, &properties);

        major = (int) VK_VERSION_MAJOR(properties.apiVersion);
        minor = (int) VK_VERSION_MINOR(properties.apiVersion);
    }

    context->VERSION_1_0 = (major == 1 && minor >= 0) || major > 1;
    context->VERSION_1_1 = (major == 1 && minor >= 1) || major > 1;
    context->VERSION_1_2 = (major == 1 && minor >= 2) || major > 1;
    context->VERSION_1_3 = (major == 1 && minor >= 3) || major > 1;
    context->VERSION_1_4 = (major == 1 && minor >= 4) || major > 1;

    return GLAD_MAKE_VERSION(major, minor);
}

int gladLoadVulkanContextUserPtr(GladVulkanContext *context, VkPhysicalDevice physical_device, GLADuserptrloadfunc load, void *userptr) {
    int version;

#ifdef VK_VERSION_1_1
    context->EnumerateInstanceVersion  = (PFN_vkEnumerateInstanceVersion) load(userptr, "vkEnumerateInstanceVersion");
#endif
    version = glad_vk_find_core_vulkan(context, physical_device);
    if (!version) {
        return 0;
    }

    glad_vk_load_VK_VERSION_1_0(context, load, userptr);
    glad_vk_load_VK_VERSION_1_1(context, load, userptr);
    glad_vk_load_VK_VERSION_1_2(context, load, userptr);
    glad_vk_load_VK_VERSION_1_3(context, load, userptr);
    glad_vk_load_VK_VERSION_1_4(context, load, userptr);

    if (!glad_vk_find_extensions_vulkan(context, physical_device)) return 0;
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    glad_vk_load_VK_AMDX_shader_enqueue(context, load, userptr);

#endif
    glad_vk_load_VK_AMD_anti_lag(context, load, userptr);
    glad_vk_load_VK_AMD_buffer_marker(context, load, userptr);
    glad_vk_load_VK_AMD_display_native_hdr(context, load, userptr);
    glad_vk_load_VK_AMD_draw_indirect_count(context, load, userptr);
    glad_vk_load_VK_AMD_shader_info(context, load, userptr);
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    glad_vk_load_VK_ANDROID_external_memory_android_hardware_buffer(context, load, userptr);

#endif
    glad_vk_load_VK_ARM_data_graph(context, load, userptr);
    glad_vk_load_VK_ARM_tensors(context, load, userptr);
    glad_vk_load_VK_EXT_acquire_drm_display(context, load, userptr);
#if defined(VK_USE_PLATFORM_XLIB_XRANDR_EXT)
    glad_vk_load_VK_EXT_acquire_xlib_display(context, load, userptr);

#endif
    glad_vk_load_VK_EXT_attachment_feedback_loop_dynamic_state(context, load, userptr);
    glad_vk_load_VK_EXT_buffer_device_address(context, load, userptr);
    glad_vk_load_VK_EXT_calibrated_timestamps(context, load, userptr);
    glad_vk_load_VK_EXT_color_write_enable(context, load, userptr);
    glad_vk_load_VK_EXT_conditional_rendering(context, load, userptr);
    glad_vk_load_VK_EXT_debug_marker(context, load, userptr);
    glad_vk_load_VK_EXT_debug_report(context, load, userptr);
    glad_vk_load_VK_EXT_debug_utils(context, load, userptr);
    glad_vk_load_VK_EXT_depth_bias_control(context, load, userptr);
    glad_vk_load_VK_EXT_depth_clamp_control(context, load, userptr);
    glad_vk_load_VK_EXT_descriptor_buffer(context, load, userptr);
    glad_vk_load_VK_EXT_device_fault(context, load, userptr);
    glad_vk_load_VK_EXT_device_generated_commands(context, load, userptr);
    glad_vk_load_VK_EXT_direct_mode_display(context, load, userptr);
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
    glad_vk_load_VK_EXT_directfb_surface(context, load, userptr);

#endif
    glad_vk_load_VK_EXT_discard_rectangles(context, load, userptr);
    glad_vk_load_VK_EXT_display_control(context, load, userptr);
    glad_vk_load_VK_EXT_display_surface_counter(context, load, userptr);
    glad_vk_load_VK_EXT_extended_dynamic_state(context, load, userptr);
    glad_vk_load_VK_EXT_extended_dynamic_state2(context, load, userptr);
    glad_vk_load_VK_EXT_extended_dynamic_state3(context, load, userptr);
    glad_vk_load_VK_EXT_external_memory_host(context, load, userptr);
#if defined(VK_USE_PLATFORM_METAL_EXT)
    glad_vk_load_VK_EXT_external_memory_metal(context, load, userptr);

#endif
    glad_vk_load_VK_EXT_fragment_density_map_offset(context, load, userptr);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    glad_vk_load_VK_EXT_full_screen_exclusive(context, load, userptr);

#endif
    glad_vk_load_VK_EXT_hdr_metadata(context, load, userptr);
    glad_vk_load_VK_EXT_headless_surface(context, load, userptr);
    glad_vk_load_VK_EXT_host_image_copy(context, load, userptr);
    glad_vk_load_VK_EXT_host_query_reset(context, load, userptr);
    glad_vk_load_VK_EXT_image_compression_control(context, load, userptr);
    glad_vk_load_VK_EXT_image_drm_format_modifier(context, load, userptr);
    glad_vk_load_VK_EXT_line_rasterization(context, load, userptr);
    glad_vk_load_VK_EXT_mesh_shader(context, load, userptr);
#if defined(VK_USE_PLATFORM_METAL_EXT)
    glad_vk_load_VK_EXT_metal_objects(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_METAL_EXT)
    glad_vk_load_VK_EXT_metal_surface(context, load, userptr);

#endif
    glad_vk_load_VK_EXT_multi_draw(context, load, userptr);
    glad_vk_load_VK_EXT_opacity_micromap(context, load, userptr);
    glad_vk_load_VK_EXT_pageable_device_local_memory(context, load, userptr);
    glad_vk_load_VK_EXT_pipeline_properties(context, load, userptr);
    glad_vk_load_VK_EXT_private_data(context, load, userptr);
    glad_vk_load_VK_EXT_sample_locations(context, load, userptr);
    glad_vk_load_VK_EXT_shader_module_identifier(context, load, userptr);
    glad_vk_load_VK_EXT_shader_object(context, load, userptr);
    glad_vk_load_VK_EXT_swapchain_maintenance1(context, load, userptr);
    glad_vk_load_VK_EXT_tooling_info(context, load, userptr);
    glad_vk_load_VK_EXT_transform_feedback(context, load, userptr);
    glad_vk_load_VK_EXT_validation_cache(context, load, userptr);
    glad_vk_load_VK_EXT_vertex_input_dynamic_state(context, load, userptr);
#if defined(VK_USE_PLATFORM_FUCHSIA)
    glad_vk_load_VK_FUCHSIA_buffer_collection(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
    glad_vk_load_VK_FUCHSIA_external_memory(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
    glad_vk_load_VK_FUCHSIA_external_semaphore(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_FUCHSIA)
    glad_vk_load_VK_FUCHSIA_imagepipe_surface(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_GGP)
    glad_vk_load_VK_GGP_stream_descriptor_surface(context, load, userptr);

#endif
    glad_vk_load_VK_GOOGLE_display_timing(context, load, userptr);
    glad_vk_load_VK_HUAWEI_cluster_culling_shader(context, load, userptr);
    glad_vk_load_VK_HUAWEI_invocation_mask(context, load, userptr);
    glad_vk_load_VK_HUAWEI_subpass_shading(context, load, userptr);
    glad_vk_load_VK_INTEL_performance_query(context, load, userptr);
    glad_vk_load_VK_KHR_acceleration_structure(context, load, userptr);
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    glad_vk_load_VK_KHR_android_surface(context, load, userptr);

#endif
    glad_vk_load_VK_KHR_bind_memory2(context, load, userptr);
    glad_vk_load_VK_KHR_buffer_device_address(context, load, userptr);
    glad_vk_load_VK_KHR_calibrated_timestamps(context, load, userptr);
    glad_vk_load_VK_KHR_cooperative_matrix(context, load, userptr);
    glad_vk_load_VK_KHR_copy_commands2(context, load, userptr);
    glad_vk_load_VK_KHR_create_renderpass2(context, load, userptr);
    glad_vk_load_VK_KHR_deferred_host_operations(context, load, userptr);
    glad_vk_load_VK_KHR_descriptor_update_template(context, load, userptr);
    glad_vk_load_VK_KHR_device_group(context, load, userptr);
    glad_vk_load_VK_KHR_device_group_creation(context, load, userptr);
    glad_vk_load_VK_KHR_display(context, load, userptr);
    glad_vk_load_VK_KHR_display_swapchain(context, load, userptr);
    glad_vk_load_VK_KHR_draw_indirect_count(context, load, userptr);
    glad_vk_load_VK_KHR_dynamic_rendering(context, load, userptr);
    glad_vk_load_VK_KHR_dynamic_rendering_local_read(context, load, userptr);
    glad_vk_load_VK_KHR_external_fence_capabilities(context, load, userptr);
    glad_vk_load_VK_KHR_external_fence_fd(context, load, userptr);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    glad_vk_load_VK_KHR_external_fence_win32(context, load, userptr);

#endif
    glad_vk_load_VK_KHR_external_memory_capabilities(context, load, userptr);
    glad_vk_load_VK_KHR_external_memory_fd(context, load, userptr);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    glad_vk_load_VK_KHR_external_memory_win32(context, load, userptr);

#endif
    glad_vk_load_VK_KHR_external_semaphore_capabilities(context, load, userptr);
    glad_vk_load_VK_KHR_external_semaphore_fd(context, load, userptr);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    glad_vk_load_VK_KHR_external_semaphore_win32(context, load, userptr);

#endif
    glad_vk_load_VK_KHR_fragment_shading_rate(context, load, userptr);
    glad_vk_load_VK_KHR_get_display_properties2(context, load, userptr);
    glad_vk_load_VK_KHR_get_memory_requirements2(context, load, userptr);
    glad_vk_load_VK_KHR_get_physical_device_properties2(context, load, userptr);
    glad_vk_load_VK_KHR_get_surface_capabilities2(context, load, userptr);
    glad_vk_load_VK_KHR_line_rasterization(context, load, userptr);
    glad_vk_load_VK_KHR_maintenance1(context, load, userptr);
    glad_vk_load_VK_KHR_maintenance3(context, load, userptr);
    glad_vk_load_VK_KHR_maintenance4(context, load, userptr);
    glad_vk_load_VK_KHR_maintenance5(context, load, userptr);
    glad_vk_load_VK_KHR_maintenance6(context, load, userptr);
    glad_vk_load_VK_KHR_map_memory2(context, load, userptr);
    glad_vk_load_VK_KHR_performance_query(context, load, userptr);
    glad_vk_load_VK_KHR_pipeline_binary(context, load, userptr);
    glad_vk_load_VK_KHR_pipeline_executable_properties(context, load, userptr);
    glad_vk_load_VK_KHR_present_wait(context, load, userptr);
    glad_vk_load_VK_KHR_present_wait2(context, load, userptr);
    glad_vk_load_VK_KHR_push_descriptor(context, load, userptr);
    glad_vk_load_VK_KHR_ray_tracing_maintenance1(context, load, userptr);
    glad_vk_load_VK_KHR_ray_tracing_pipeline(context, load, userptr);
    glad_vk_load_VK_KHR_sampler_ycbcr_conversion(context, load, userptr);
    glad_vk_load_VK_KHR_shared_presentable_image(context, load, userptr);
    glad_vk_load_VK_KHR_surface(context, load, userptr);
    glad_vk_load_VK_KHR_swapchain(context, load, userptr);
    glad_vk_load_VK_KHR_synchronization2(context, load, userptr);
    glad_vk_load_VK_KHR_timeline_semaphore(context, load, userptr);
    glad_vk_load_VK_KHR_video_decode_queue(context, load, userptr);
    glad_vk_load_VK_KHR_video_encode_queue(context, load, userptr);
    glad_vk_load_VK_KHR_video_queue(context, load, userptr);
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    glad_vk_load_VK_KHR_wayland_surface(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    glad_vk_load_VK_KHR_win32_surface(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
    glad_vk_load_VK_KHR_xcb_surface(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    glad_vk_load_VK_KHR_xlib_surface(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_IOS_MVK)
    glad_vk_load_VK_MVK_ios_surface(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_MACOS_MVK)
    glad_vk_load_VK_MVK_macos_surface(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_VI_NN)
    glad_vk_load_VK_NN_vi_surface(context, load, userptr);

#endif
    glad_vk_load_VK_NVX_binary_import(context, load, userptr);
    glad_vk_load_VK_NVX_image_view_handle(context, load, userptr);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    glad_vk_load_VK_NV_acquire_winrt_display(context, load, userptr);

#endif
    glad_vk_load_VK_NV_clip_space_w_scaling(context, load, userptr);
    glad_vk_load_VK_NV_cluster_acceleration_structure(context, load, userptr);
    glad_vk_load_VK_NV_cooperative_matrix(context, load, userptr);
    glad_vk_load_VK_NV_cooperative_matrix2(context, load, userptr);
    glad_vk_load_VK_NV_cooperative_vector(context, load, userptr);
    glad_vk_load_VK_NV_copy_memory_indirect(context, load, userptr);
    glad_vk_load_VK_NV_coverage_reduction_mode(context, load, userptr);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    glad_vk_load_VK_NV_cuda_kernel_launch(context, load, userptr);

#endif
    glad_vk_load_VK_NV_device_diagnostic_checkpoints(context, load, userptr);
    glad_vk_load_VK_NV_device_generated_commands(context, load, userptr);
    glad_vk_load_VK_NV_device_generated_commands_compute(context, load, userptr);
    glad_vk_load_VK_NV_external_compute_queue(context, load, userptr);
    glad_vk_load_VK_NV_external_memory_capabilities(context, load, userptr);
    glad_vk_load_VK_NV_external_memory_rdma(context, load, userptr);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    glad_vk_load_VK_NV_external_memory_win32(context, load, userptr);

#endif
    glad_vk_load_VK_NV_fragment_shading_rate_enums(context, load, userptr);
    glad_vk_load_VK_NV_low_latency2(context, load, userptr);
    glad_vk_load_VK_NV_memory_decompression(context, load, userptr);
    glad_vk_load_VK_NV_mesh_shader(context, load, userptr);
    glad_vk_load_VK_NV_optical_flow(context, load, userptr);
    glad_vk_load_VK_NV_partitioned_acceleration_structure(context, load, userptr);
    glad_vk_load_VK_NV_ray_tracing(context, load, userptr);
    glad_vk_load_VK_NV_scissor_exclusive(context, load, userptr);
    glad_vk_load_VK_NV_shading_rate_image(context, load, userptr);
#if defined(VK_USE_PLATFORM_OHOS)
    glad_vk_load_VK_OHOS_surface(context, load, userptr);

#endif
    glad_vk_load_VK_QCOM_tile_memory_heap(context, load, userptr);
    glad_vk_load_VK_QCOM_tile_properties(context, load, userptr);
    glad_vk_load_VK_QCOM_tile_shading(context, load, userptr);
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
    glad_vk_load_VK_QNX_external_memory_screen_buffer(context, load, userptr);

#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
    glad_vk_load_VK_QNX_screen_surface(context, load, userptr);

#endif
    glad_vk_load_VK_VALVE_descriptor_set_host_mapping(context, load, userptr);


    return version;
}


int gladLoadVulkanContext(GladVulkanContext *context, VkPhysicalDevice physical_device, GLADloadfunc load) {
    return gladLoadVulkanContextUserPtr(context, physical_device, glad_vk_get_proc_from_userptr, GLAD_GNUC_EXTENSION (void*) load);
}



 

#ifdef GLAD_VULKAN

#ifndef GLAD_LOADER_LIBRARY_C_
#define GLAD_LOADER_LIBRARY_C_

#include <stddef.h>
#include <stdlib.h>

#if GLAD_PLATFORM_WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


static void* glad_get_dlopen_handle(const char *lib_names[], int length) {
    void *handle = NULL;
    int i;

    for (i = 0; i < length; ++i) {
#if GLAD_PLATFORM_WIN32
  #if GLAD_PLATFORM_UWP
        size_t buffer_size = (strlen(lib_names[i]) + 1) * sizeof(WCHAR);
        LPWSTR buffer = (LPWSTR) malloc(buffer_size);
        if (buffer != NULL) {
            int ret = MultiByteToWideChar(CP_ACP, 0, lib_names[i], -1, buffer, buffer_size);
            if (ret != 0) {
                handle = (void*) LoadPackagedLibrary(buffer, 0);
            }
            free((void*) buffer);
        }
  #else
        handle = (void*) LoadLibraryA(lib_names[i]);
  #endif
#else
        handle = dlopen(lib_names[i], RTLD_LAZY | RTLD_LOCAL);
#endif
        if (handle != NULL) {
            return handle;
        }
    }

    return NULL;
}

static void glad_close_dlopen_handle(void* handle) {
    if (handle != NULL) {
#if GLAD_PLATFORM_WIN32
        FreeLibrary((HMODULE) handle);
#else
        dlclose(handle);
#endif
    }
}

static GLADapiproc glad_dlsym_handle(void* handle, const char *name) {
    if (handle == NULL) {
        return NULL;
    }

#if GLAD_PLATFORM_WIN32
    return (GLADapiproc) GetProcAddress((HMODULE) handle, name);
#else
    return GLAD_GNUC_EXTENSION (GLADapiproc) dlsym(handle, name);
#endif
}

#endif /* GLAD_LOADER_LIBRARY_C_ */


static const char* DEVICE_FUNCTIONS[] = {
    "vkAcquireFullScreenExclusiveModeEXT",
    "vkAcquireNextImage2KHR",
    "vkAcquireNextImageKHR",
    "vkAcquirePerformanceConfigurationINTEL",
    "vkAcquireProfilingLockKHR",
    "vkAllocateCommandBuffers",
    "vkAllocateDescriptorSets",
    "vkAllocateMemory",
    "vkAntiLagUpdateAMD",
    "vkBeginCommandBuffer",
    "vkBindAccelerationStructureMemoryNV",
    "vkBindBufferMemory",
    "vkBindBufferMemory2",
    "vkBindBufferMemory2KHR",
    "vkBindDataGraphPipelineSessionMemoryARM",
    "vkBindImageMemory",
    "vkBindImageMemory2",
    "vkBindImageMemory2KHR",
    "vkBindOpticalFlowSessionImageNV",
    "vkBindTensorMemoryARM",
    "vkBindVideoSessionMemoryKHR",
    "vkBuildAccelerationStructuresKHR",
    "vkBuildMicromapsEXT",
    "vkCmdBeginConditionalRenderingEXT",
    "vkCmdBeginDebugUtilsLabelEXT",
    "vkCmdBeginPerTileExecutionQCOM",
    "vkCmdBeginQuery",
    "vkCmdBeginQueryIndexedEXT",
    "vkCmdBeginRenderPass",
    "vkCmdBeginRenderPass2",
    "vkCmdBeginRenderPass2KHR",
    "vkCmdBeginRendering",
    "vkCmdBeginRenderingKHR",
    "vkCmdBeginTransformFeedbackEXT",
    "vkCmdBeginVideoCodingKHR",
    "vkCmdBindDescriptorBufferEmbeddedSamplers2EXT",
    "vkCmdBindDescriptorBufferEmbeddedSamplersEXT",
    "vkCmdBindDescriptorBuffersEXT",
    "vkCmdBindDescriptorSets",
    "vkCmdBindDescriptorSets2",
    "vkCmdBindDescriptorSets2KHR",
    "vkCmdBindIndexBuffer",
    "vkCmdBindIndexBuffer2",
    "vkCmdBindIndexBuffer2KHR",
    "vkCmdBindInvocationMaskHUAWEI",
    "vkCmdBindPipeline",
    "vkCmdBindPipelineShaderGroupNV",
    "vkCmdBindShadersEXT",
    "vkCmdBindShadingRateImageNV",
    "vkCmdBindTileMemoryQCOM",
    "vkCmdBindTransformFeedbackBuffersEXT",
    "vkCmdBindVertexBuffers",
    "vkCmdBindVertexBuffers2",
    "vkCmdBindVertexBuffers2EXT",
    "vkCmdBlitImage",
    "vkCmdBlitImage2",
    "vkCmdBlitImage2KHR",
    "vkCmdBuildAccelerationStructureNV",
    "vkCmdBuildAccelerationStructuresIndirectKHR",
    "vkCmdBuildAccelerationStructuresKHR",
    "vkCmdBuildClusterAccelerationStructureIndirectNV",
    "vkCmdBuildMicromapsEXT",
    "vkCmdBuildPartitionedAccelerationStructuresNV",
    "vkCmdClearAttachments",
    "vkCmdClearColorImage",
    "vkCmdClearDepthStencilImage",
    "vkCmdControlVideoCodingKHR",
    "vkCmdConvertCooperativeVectorMatrixNV",
    "vkCmdCopyAccelerationStructureKHR",
    "vkCmdCopyAccelerationStructureNV",
    "vkCmdCopyAccelerationStructureToMemoryKHR",
    "vkCmdCopyBuffer",
    "vkCmdCopyBuffer2",
    "vkCmdCopyBuffer2KHR",
    "vkCmdCopyBufferToImage",
    "vkCmdCopyBufferToImage2",
    "vkCmdCopyBufferToImage2KHR",
    "vkCmdCopyImage",
    "vkCmdCopyImage2",
    "vkCmdCopyImage2KHR",
    "vkCmdCopyImageToBuffer",
    "vkCmdCopyImageToBuffer2",
    "vkCmdCopyImageToBuffer2KHR",
    "vkCmdCopyMemoryIndirectNV",
    "vkCmdCopyMemoryToAccelerationStructureKHR",
    "vkCmdCopyMemoryToImageIndirectNV",
    "vkCmdCopyMemoryToMicromapEXT",
    "vkCmdCopyMicromapEXT",
    "vkCmdCopyMicromapToMemoryEXT",
    "vkCmdCopyQueryPoolResults",
    "vkCmdCopyTensorARM",
    "vkCmdCuLaunchKernelNVX",
    "vkCmdCudaLaunchKernelNV",
    "vkCmdDebugMarkerBeginEXT",
    "vkCmdDebugMarkerEndEXT",
    "vkCmdDebugMarkerInsertEXT",
    "vkCmdDecodeVideoKHR",
    "vkCmdDecompressMemoryIndirectCountNV",
    "vkCmdDecompressMemoryNV",
    "vkCmdDispatch",
    "vkCmdDispatchBase",
    "vkCmdDispatchBaseKHR",
    "vkCmdDispatchDataGraphARM",
    "vkCmdDispatchGraphAMDX",
    "vkCmdDispatchGraphIndirectAMDX",
    "vkCmdDispatchGraphIndirectCountAMDX",
    "vkCmdDispatchIndirect",
    "vkCmdDispatchTileQCOM",
    "vkCmdDraw",
    "vkCmdDrawClusterHUAWEI",
    "vkCmdDrawClusterIndirectHUAWEI",
    "vkCmdDrawIndexed",
    "vkCmdDrawIndexedIndirect",
    "vkCmdDrawIndexedIndirectCount",
    "vkCmdDrawIndexedIndirectCountAMD",
    "vkCmdDrawIndexedIndirectCountKHR",
    "vkCmdDrawIndirect",
    "vkCmdDrawIndirectByteCountEXT",
    "vkCmdDrawIndirectCount",
    "vkCmdDrawIndirectCountAMD",
    "vkCmdDrawIndirectCountKHR",
    "vkCmdDrawMeshTasksEXT",
    "vkCmdDrawMeshTasksIndirectCountEXT",
    "vkCmdDrawMeshTasksIndirectCountNV",
    "vkCmdDrawMeshTasksIndirectEXT",
    "vkCmdDrawMeshTasksIndirectNV",
    "vkCmdDrawMeshTasksNV",
    "vkCmdDrawMultiEXT",
    "vkCmdDrawMultiIndexedEXT",
    "vkCmdEncodeVideoKHR",
    "vkCmdEndConditionalRenderingEXT",
    "vkCmdEndDebugUtilsLabelEXT",
    "vkCmdEndPerTileExecutionQCOM",
    "vkCmdEndQuery",
    "vkCmdEndQueryIndexedEXT",
    "vkCmdEndRenderPass",
    "vkCmdEndRenderPass2",
    "vkCmdEndRenderPass2KHR",
    "vkCmdEndRendering",
    "vkCmdEndRendering2EXT",
    "vkCmdEndRenderingKHR",
    "vkCmdEndTransformFeedbackEXT",
    "vkCmdEndVideoCodingKHR",
    "vkCmdExecuteCommands",
    "vkCmdExecuteGeneratedCommandsEXT",
    "vkCmdExecuteGeneratedCommandsNV",
    "vkCmdFillBuffer",
    "vkCmdInitializeGraphScratchMemoryAMDX",
    "vkCmdInsertDebugUtilsLabelEXT",
    "vkCmdNextSubpass",
    "vkCmdNextSubpass2",
    "vkCmdNextSubpass2KHR",
    "vkCmdOpticalFlowExecuteNV",
    "vkCmdPipelineBarrier",
    "vkCmdPipelineBarrier2",
    "vkCmdPipelineBarrier2KHR",
    "vkCmdPreprocessGeneratedCommandsEXT",
    "vkCmdPreprocessGeneratedCommandsNV",
    "vkCmdPushConstants",
    "vkCmdPushConstants2",
    "vkCmdPushConstants2KHR",
    "vkCmdPushDescriptorSet",
    "vkCmdPushDescriptorSet2",
    "vkCmdPushDescriptorSet2KHR",
    "vkCmdPushDescriptorSetKHR",
    "vkCmdPushDescriptorSetWithTemplate",
    "vkCmdPushDescriptorSetWithTemplate2",
    "vkCmdPushDescriptorSetWithTemplate2KHR",
    "vkCmdPushDescriptorSetWithTemplateKHR",
    "vkCmdResetEvent",
    "vkCmdResetEvent2",
    "vkCmdResetEvent2KHR",
    "vkCmdResetQueryPool",
    "vkCmdResolveImage",
    "vkCmdResolveImage2",
    "vkCmdResolveImage2KHR",
    "vkCmdSetAlphaToCoverageEnableEXT",
    "vkCmdSetAlphaToOneEnableEXT",
    "vkCmdSetAttachmentFeedbackLoopEnableEXT",
    "vkCmdSetBlendConstants",
    "vkCmdSetCheckpointNV",
    "vkCmdSetCoarseSampleOrderNV",
    "vkCmdSetColorBlendAdvancedEXT",
    "vkCmdSetColorBlendEnableEXT",
    "vkCmdSetColorBlendEquationEXT",
    "vkCmdSetColorWriteEnableEXT",
    "vkCmdSetColorWriteMaskEXT",
    "vkCmdSetConservativeRasterizationModeEXT",
    "vkCmdSetCoverageModulationModeNV",
    "vkCmdSetCoverageModulationTableEnableNV",
    "vkCmdSetCoverageModulationTableNV",
    "vkCmdSetCoverageReductionModeNV",
    "vkCmdSetCoverageToColorEnableNV",
    "vkCmdSetCoverageToColorLocationNV",
    "vkCmdSetCullMode",
    "vkCmdSetCullModeEXT",
    "vkCmdSetDepthBias",
    "vkCmdSetDepthBias2EXT",
    "vkCmdSetDepthBiasEnable",
    "vkCmdSetDepthBiasEnableEXT",
    "vkCmdSetDepthBounds",
    "vkCmdSetDepthBoundsTestEnable",
    "vkCmdSetDepthBoundsTestEnableEXT",
    "vkCmdSetDepthClampEnableEXT",
    "vkCmdSetDepthClampRangeEXT",
    "vkCmdSetDepthClipEnableEXT",
    "vkCmdSetDepthClipNegativeOneToOneEXT",
    "vkCmdSetDepthCompareOp",
    "vkCmdSetDepthCompareOpEXT",
    "vkCmdSetDepthTestEnable",
    "vkCmdSetDepthTestEnableEXT",
    "vkCmdSetDepthWriteEnable",
    "vkCmdSetDepthWriteEnableEXT",
    "vkCmdSetDescriptorBufferOffsets2EXT",
    "vkCmdSetDescriptorBufferOffsetsEXT",
    "vkCmdSetDeviceMask",
    "vkCmdSetDeviceMaskKHR",
    "vkCmdSetDiscardRectangleEXT",
    "vkCmdSetDiscardRectangleEnableEXT",
    "vkCmdSetDiscardRectangleModeEXT",
    "vkCmdSetEvent",
    "vkCmdSetEvent2",
    "vkCmdSetEvent2KHR",
    "vkCmdSetExclusiveScissorEnableNV",
    "vkCmdSetExclusiveScissorNV",
    "vkCmdSetExtraPrimitiveOverestimationSizeEXT",
    "vkCmdSetFragmentShadingRateEnumNV",
    "vkCmdSetFragmentShadingRateKHR",
    "vkCmdSetFrontFace",
    "vkCmdSetFrontFaceEXT",
    "vkCmdSetLineRasterizationModeEXT",
    "vkCmdSetLineStipple",
    "vkCmdSetLineStippleEXT",
    "vkCmdSetLineStippleEnableEXT",
    "vkCmdSetLineStippleKHR",
    "vkCmdSetLineWidth",
    "vkCmdSetLogicOpEXT",
    "vkCmdSetLogicOpEnableEXT",
    "vkCmdSetPatchControlPointsEXT",
    "vkCmdSetPerformanceMarkerINTEL",
    "vkCmdSetPerformanceOverrideINTEL",
    "vkCmdSetPerformanceStreamMarkerINTEL",
    "vkCmdSetPolygonModeEXT",
    "vkCmdSetPrimitiveRestartEnable",
    "vkCmdSetPrimitiveRestartEnableEXT",
    "vkCmdSetPrimitiveTopology",
    "vkCmdSetPrimitiveTopologyEXT",
    "vkCmdSetProvokingVertexModeEXT",
    "vkCmdSetRasterizationSamplesEXT",
    "vkCmdSetRasterizationStreamEXT",
    "vkCmdSetRasterizerDiscardEnable",
    "vkCmdSetRasterizerDiscardEnableEXT",
    "vkCmdSetRayTracingPipelineStackSizeKHR",
    "vkCmdSetRenderingAttachmentLocations",
    "vkCmdSetRenderingAttachmentLocationsKHR",
    "vkCmdSetRenderingInputAttachmentIndices",
    "vkCmdSetRenderingInputAttachmentIndicesKHR",
    "vkCmdSetRepresentativeFragmentTestEnableNV",
    "vkCmdSetSampleLocationsEXT",
    "vkCmdSetSampleLocationsEnableEXT",
    "vkCmdSetSampleMaskEXT",
    "vkCmdSetScissor",
    "vkCmdSetScissorWithCount",
    "vkCmdSetScissorWithCountEXT",
    "vkCmdSetShadingRateImageEnableNV",
    "vkCmdSetStencilCompareMask",
    "vkCmdSetStencilOp",
    "vkCmdSetStencilOpEXT",
    "vkCmdSetStencilReference",
    "vkCmdSetStencilTestEnable",
    "vkCmdSetStencilTestEnableEXT",
    "vkCmdSetStencilWriteMask",
    "vkCmdSetTessellationDomainOriginEXT",
    "vkCmdSetVertexInputEXT",
    "vkCmdSetViewport",
    "vkCmdSetViewportShadingRatePaletteNV",
    "vkCmdSetViewportSwizzleNV",
    "vkCmdSetViewportWScalingEnableNV",
    "vkCmdSetViewportWScalingNV",
    "vkCmdSetViewportWithCount",
    "vkCmdSetViewportWithCountEXT",
    "vkCmdSubpassShadingHUAWEI",
    "vkCmdTraceRaysIndirect2KHR",
    "vkCmdTraceRaysIndirectKHR",
    "vkCmdTraceRaysKHR",
    "vkCmdTraceRaysNV",
    "vkCmdUpdateBuffer",
    "vkCmdUpdatePipelineIndirectBufferNV",
    "vkCmdWaitEvents",
    "vkCmdWaitEvents2",
    "vkCmdWaitEvents2KHR",
    "vkCmdWriteAccelerationStructuresPropertiesKHR",
    "vkCmdWriteAccelerationStructuresPropertiesNV",
    "vkCmdWriteBufferMarker2AMD",
    "vkCmdWriteBufferMarkerAMD",
    "vkCmdWriteMicromapsPropertiesEXT",
    "vkCmdWriteTimestamp",
    "vkCmdWriteTimestamp2",
    "vkCmdWriteTimestamp2KHR",
    "vkCompileDeferredNV",
    "vkConvertCooperativeVectorMatrixNV",
    "vkCopyAccelerationStructureKHR",
    "vkCopyAccelerationStructureToMemoryKHR",
    "vkCopyImageToImage",
    "vkCopyImageToImageEXT",
    "vkCopyImageToMemory",
    "vkCopyImageToMemoryEXT",
    "vkCopyMemoryToAccelerationStructureKHR",
    "vkCopyMemoryToImage",
    "vkCopyMemoryToImageEXT",
    "vkCopyMemoryToMicromapEXT",
    "vkCopyMicromapEXT",
    "vkCopyMicromapToMemoryEXT",
    "vkCreateAccelerationStructureKHR",
    "vkCreateAccelerationStructureNV",
    "vkCreateBuffer",
    "vkCreateBufferCollectionFUCHSIA",
    "vkCreateBufferView",
    "vkCreateCommandPool",
    "vkCreateComputePipelines",
    "vkCreateCuFunctionNVX",
    "vkCreateCuModuleNVX",
    "vkCreateCudaFunctionNV",
    "vkCreateCudaModuleNV",
    "vkCreateDataGraphPipelineSessionARM",
    "vkCreateDataGraphPipelinesARM",
    "vkCreateDeferredOperationKHR",
    "vkCreateDescriptorPool",
    "vkCreateDescriptorSetLayout",
    "vkCreateDescriptorUpdateTemplate",
    "vkCreateDescriptorUpdateTemplateKHR",
    "vkCreateEvent",
    "vkCreateExecutionGraphPipelinesAMDX",
    "vkCreateExternalComputeQueueNV",
    "vkCreateFence",
    "vkCreateFramebuffer",
    "vkCreateGraphicsPipelines",
    "vkCreateImage",
    "vkCreateImageView",
    "vkCreateIndirectCommandsLayoutEXT",
    "vkCreateIndirectCommandsLayoutNV",
    "vkCreateIndirectExecutionSetEXT",
    "vkCreateMicromapEXT",
    "vkCreateOpticalFlowSessionNV",
    "vkCreatePipelineBinariesKHR",
    "vkCreatePipelineCache",
    "vkCreatePipelineLayout",
    "vkCreatePrivateDataSlot",
    "vkCreatePrivateDataSlotEXT",
    "vkCreateQueryPool",
    "vkCreateRayTracingPipelinesKHR",
    "vkCreateRayTracingPipelinesNV",
    "vkCreateRenderPass",
    "vkCreateRenderPass2",
    "vkCreateRenderPass2KHR",
    "vkCreateSampler",
    "vkCreateSamplerYcbcrConversion",
    "vkCreateSamplerYcbcrConversionKHR",
    "vkCreateSemaphore",
    "vkCreateShaderModule",
    "vkCreateShadersEXT",
    "vkCreateSharedSwapchainsKHR",
    "vkCreateSwapchainKHR",
    "vkCreateTensorARM",
    "vkCreateTensorViewARM",
    "vkCreateValidationCacheEXT",
    "vkCreateVideoSessionKHR",
    "vkCreateVideoSessionParametersKHR",
    "vkDebugMarkerSetObjectNameEXT",
    "vkDebugMarkerSetObjectTagEXT",
    "vkDeferredOperationJoinKHR",
    "vkDestroyAccelerationStructureKHR",
    "vkDestroyAccelerationStructureNV",
    "vkDestroyBuffer",
    "vkDestroyBufferCollectionFUCHSIA",
    "vkDestroyBufferView",
    "vkDestroyCommandPool",
    "vkDestroyCuFunctionNVX",
    "vkDestroyCuModuleNVX",
    "vkDestroyCudaFunctionNV",
    "vkDestroyCudaModuleNV",
    "vkDestroyDataGraphPipelineSessionARM",
    "vkDestroyDeferredOperationKHR",
    "vkDestroyDescriptorPool",
    "vkDestroyDescriptorSetLayout",
    "vkDestroyDescriptorUpdateTemplate",
    "vkDestroyDescriptorUpdateTemplateKHR",
    "vkDestroyDevice",
    "vkDestroyEvent",
    "vkDestroyExternalComputeQueueNV",
    "vkDestroyFence",
    "vkDestroyFramebuffer",
    "vkDestroyImage",
    "vkDestroyImageView",
    "vkDestroyIndirectCommandsLayoutEXT",
    "vkDestroyIndirectCommandsLayoutNV",
    "vkDestroyIndirectExecutionSetEXT",
    "vkDestroyMicromapEXT",
    "vkDestroyOpticalFlowSessionNV",
    "vkDestroyPipeline",
    "vkDestroyPipelineBinaryKHR",
    "vkDestroyPipelineCache",
    "vkDestroyPipelineLayout",
    "vkDestroyPrivateDataSlot",
    "vkDestroyPrivateDataSlotEXT",
    "vkDestroyQueryPool",
    "vkDestroyRenderPass",
    "vkDestroySampler",
    "vkDestroySamplerYcbcrConversion",
    "vkDestroySamplerYcbcrConversionKHR",
    "vkDestroySemaphore",
    "vkDestroyShaderEXT",
    "vkDestroyShaderModule",
    "vkDestroySwapchainKHR",
    "vkDestroyTensorARM",
    "vkDestroyTensorViewARM",
    "vkDestroyValidationCacheEXT",
    "vkDestroyVideoSessionKHR",
    "vkDestroyVideoSessionParametersKHR",
    "vkDeviceWaitIdle",
    "vkDisplayPowerControlEXT",
    "vkEndCommandBuffer",
    "vkExportMetalObjectsEXT",
    "vkFlushMappedMemoryRanges",
    "vkFreeCommandBuffers",
    "vkFreeDescriptorSets",
    "vkFreeMemory",
    "vkGetAccelerationStructureBuildSizesKHR",
    "vkGetAccelerationStructureDeviceAddressKHR",
    "vkGetAccelerationStructureHandleNV",
    "vkGetAccelerationStructureMemoryRequirementsNV",
    "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT",
    "vkGetAndroidHardwareBufferPropertiesANDROID",
    "vkGetBufferCollectionPropertiesFUCHSIA",
    "vkGetBufferDeviceAddress",
    "vkGetBufferDeviceAddressEXT",
    "vkGetBufferDeviceAddressKHR",
    "vkGetBufferMemoryRequirements",
    "vkGetBufferMemoryRequirements2",
    "vkGetBufferMemoryRequirements2KHR",
    "vkGetBufferOpaqueCaptureAddress",
    "vkGetBufferOpaqueCaptureAddressKHR",
    "vkGetBufferOpaqueCaptureDescriptorDataEXT",
    "vkGetCalibratedTimestampsEXT",
    "vkGetCalibratedTimestampsKHR",
    "vkGetClusterAccelerationStructureBuildSizesNV",
    "vkGetCudaModuleCacheNV",
    "vkGetDataGraphPipelineAvailablePropertiesARM",
    "vkGetDataGraphPipelinePropertiesARM",
    "vkGetDataGraphPipelineSessionBindPointRequirementsARM",
    "vkGetDataGraphPipelineSessionMemoryRequirementsARM",
    "vkGetDeferredOperationMaxConcurrencyKHR",
    "vkGetDeferredOperationResultKHR",
    "vkGetDescriptorEXT",
    "vkGetDescriptorSetHostMappingVALVE",
    "vkGetDescriptorSetLayoutBindingOffsetEXT",
    "vkGetDescriptorSetLayoutHostMappingInfoVALVE",
    "vkGetDescriptorSetLayoutSizeEXT",
    "vkGetDescriptorSetLayoutSupport",
    "vkGetDescriptorSetLayoutSupportKHR",
    "vkGetDeviceAccelerationStructureCompatibilityKHR",
    "vkGetDeviceBufferMemoryRequirements",
    "vkGetDeviceBufferMemoryRequirementsKHR",
    "vkGetDeviceFaultInfoEXT",
    "vkGetDeviceGroupPeerMemoryFeatures",
    "vkGetDeviceGroupPeerMemoryFeaturesKHR",
    "vkGetDeviceGroupPresentCapabilitiesKHR",
    "vkGetDeviceGroupSurfacePresentModes2EXT",
    "vkGetDeviceGroupSurfacePresentModesKHR",
    "vkGetDeviceImageMemoryRequirements",
    "vkGetDeviceImageMemoryRequirementsKHR",
    "vkGetDeviceImageSparseMemoryRequirements",
    "vkGetDeviceImageSparseMemoryRequirementsKHR",
    "vkGetDeviceImageSubresourceLayout",
    "vkGetDeviceImageSubresourceLayoutKHR",
    "vkGetDeviceMemoryCommitment",
    "vkGetDeviceMemoryOpaqueCaptureAddress",
    "vkGetDeviceMemoryOpaqueCaptureAddressKHR",
    "vkGetDeviceMicromapCompatibilityEXT",
    "vkGetDeviceProcAddr",
    "vkGetDeviceQueue",
    "vkGetDeviceQueue2",
    "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI",
    "vkGetDeviceTensorMemoryRequirementsARM",
    "vkGetDynamicRenderingTilePropertiesQCOM",
    "vkGetEncodedVideoSessionParametersKHR",
    "vkGetEventStatus",
    "vkGetExecutionGraphPipelineNodeIndexAMDX",
    "vkGetExecutionGraphPipelineScratchSizeAMDX",
    "vkGetFenceFdKHR",
    "vkGetFenceStatus",
    "vkGetFenceWin32HandleKHR",
    "vkGetFramebufferTilePropertiesQCOM",
    "vkGetGeneratedCommandsMemoryRequirementsEXT",
    "vkGetGeneratedCommandsMemoryRequirementsNV",
    "vkGetImageDrmFormatModifierPropertiesEXT",
    "vkGetImageMemoryRequirements",
    "vkGetImageMemoryRequirements2",
    "vkGetImageMemoryRequirements2KHR",
    "vkGetImageOpaqueCaptureDescriptorDataEXT",
    "vkGetImageSparseMemoryRequirements",
    "vkGetImageSparseMemoryRequirements2",
    "vkGetImageSparseMemoryRequirements2KHR",
    "vkGetImageSubresourceLayout",
    "vkGetImageSubresourceLayout2",
    "vkGetImageSubresourceLayout2EXT",
    "vkGetImageSubresourceLayout2KHR",
    "vkGetImageViewAddressNVX",
    "vkGetImageViewHandle64NVX",
    "vkGetImageViewHandleNVX",
    "vkGetImageViewOpaqueCaptureDescriptorDataEXT",
    "vkGetLatencyTimingsNV",
    "vkGetMemoryAndroidHardwareBufferANDROID",
    "vkGetMemoryFdKHR",
    "vkGetMemoryFdPropertiesKHR",
    "vkGetMemoryHostPointerPropertiesEXT",
    "vkGetMemoryMetalHandleEXT",
    "vkGetMemoryMetalHandlePropertiesEXT",
    "vkGetMemoryRemoteAddressNV",
    "vkGetMemoryWin32HandleKHR",
    "vkGetMemoryWin32HandleNV",
    "vkGetMemoryWin32HandlePropertiesKHR",
    "vkGetMemoryZirconHandleFUCHSIA",
    "vkGetMemoryZirconHandlePropertiesFUCHSIA",
    "vkGetMicromapBuildSizesEXT",
    "vkGetPartitionedAccelerationStructuresBuildSizesNV",
    "vkGetPastPresentationTimingGOOGLE",
    "vkGetPerformanceParameterINTEL",
    "vkGetPipelineBinaryDataKHR",
    "vkGetPipelineCacheData",
    "vkGetPipelineExecutableInternalRepresentationsKHR",
    "vkGetPipelineExecutablePropertiesKHR",
    "vkGetPipelineExecutableStatisticsKHR",
    "vkGetPipelineIndirectDeviceAddressNV",
    "vkGetPipelineIndirectMemoryRequirementsNV",
    "vkGetPipelineKeyKHR",
    "vkGetPipelinePropertiesEXT",
    "vkGetPrivateData",
    "vkGetPrivateDataEXT",
    "vkGetQueryPoolResults",
    "vkGetQueueCheckpointData2NV",
    "vkGetQueueCheckpointDataNV",
    "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR",
    "vkGetRayTracingShaderGroupHandlesKHR",
    "vkGetRayTracingShaderGroupHandlesNV",
    "vkGetRayTracingShaderGroupStackSizeKHR",
    "vkGetRefreshCycleDurationGOOGLE",
    "vkGetRenderAreaGranularity",
    "vkGetRenderingAreaGranularity",
    "vkGetRenderingAreaGranularityKHR",
    "vkGetSamplerOpaqueCaptureDescriptorDataEXT",
    "vkGetScreenBufferPropertiesQNX",
    "vkGetSemaphoreCounterValue",
    "vkGetSemaphoreCounterValueKHR",
    "vkGetSemaphoreFdKHR",
    "vkGetSemaphoreWin32HandleKHR",
    "vkGetSemaphoreZirconHandleFUCHSIA",
    "vkGetShaderBinaryDataEXT",
    "vkGetShaderInfoAMD",
    "vkGetShaderModuleCreateInfoIdentifierEXT",
    "vkGetShaderModuleIdentifierEXT",
    "vkGetSwapchainCounterEXT",
    "vkGetSwapchainImagesKHR",
    "vkGetSwapchainStatusKHR",
    "vkGetTensorMemoryRequirementsARM",
    "vkGetTensorOpaqueCaptureDescriptorDataARM",
    "vkGetTensorViewOpaqueCaptureDescriptorDataARM",
    "vkGetValidationCacheDataEXT",
    "vkGetVideoSessionMemoryRequirementsKHR",
    "vkImportFenceFdKHR",
    "vkImportFenceWin32HandleKHR",
    "vkImportSemaphoreFdKHR",
    "vkImportSemaphoreWin32HandleKHR",
    "vkImportSemaphoreZirconHandleFUCHSIA",
    "vkInitializePerformanceApiINTEL",
    "vkInvalidateMappedMemoryRanges",
    "vkLatencySleepNV",
    "vkMapMemory",
    "vkMapMemory2",
    "vkMapMemory2KHR",
    "vkMergePipelineCaches",
    "vkMergeValidationCachesEXT",
    "vkQueueBeginDebugUtilsLabelEXT",
    "vkQueueBindSparse",
    "vkQueueEndDebugUtilsLabelEXT",
    "vkQueueInsertDebugUtilsLabelEXT",
    "vkQueueNotifyOutOfBandNV",
    "vkQueuePresentKHR",
    "vkQueueSetPerformanceConfigurationINTEL",
    "vkQueueSubmit",
    "vkQueueSubmit2",
    "vkQueueSubmit2KHR",
    "vkQueueWaitIdle",
    "vkRegisterDeviceEventEXT",
    "vkRegisterDisplayEventEXT",
    "vkReleaseCapturedPipelineDataKHR",
    "vkReleaseFullScreenExclusiveModeEXT",
    "vkReleasePerformanceConfigurationINTEL",
    "vkReleaseProfilingLockKHR",
    "vkReleaseSwapchainImagesEXT",
    "vkResetCommandBuffer",
    "vkResetCommandPool",
    "vkResetDescriptorPool",
    "vkResetEvent",
    "vkResetFences",
    "vkResetQueryPool",
    "vkResetQueryPoolEXT",
    "vkSetBufferCollectionBufferConstraintsFUCHSIA",
    "vkSetBufferCollectionImageConstraintsFUCHSIA",
    "vkSetDebugUtilsObjectNameEXT",
    "vkSetDebugUtilsObjectTagEXT",
    "vkSetDeviceMemoryPriorityEXT",
    "vkSetEvent",
    "vkSetHdrMetadataEXT",
    "vkSetLatencyMarkerNV",
    "vkSetLatencySleepModeNV",
    "vkSetLocalDimmingAMD",
    "vkSetPrivateData",
    "vkSetPrivateDataEXT",
    "vkSignalSemaphore",
    "vkSignalSemaphoreKHR",
    "vkTransitionImageLayout",
    "vkTransitionImageLayoutEXT",
    "vkTrimCommandPool",
    "vkTrimCommandPoolKHR",
    "vkUninitializePerformanceApiINTEL",
    "vkUnmapMemory",
    "vkUnmapMemory2",
    "vkUnmapMemory2KHR",
    "vkUpdateDescriptorSetWithTemplate",
    "vkUpdateDescriptorSetWithTemplateKHR",
    "vkUpdateDescriptorSets",
    "vkUpdateIndirectExecutionSetPipelineEXT",
    "vkUpdateIndirectExecutionSetShaderEXT",
    "vkUpdateVideoSessionParametersKHR",
    "vkWaitForFences",
    "vkWaitForPresent2KHR",
    "vkWaitForPresentKHR",
    "vkWaitSemaphores",
    "vkWaitSemaphoresKHR",
    "vkWriteAccelerationStructuresPropertiesKHR",
    "vkWriteMicromapsPropertiesEXT",
};

static int glad_vulkan_is_device_function(const char *name) {
    /* Exists as a workaround for:
     * https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/issues/2323
     *
     * `vkGetDeviceProcAddr` does not return NULL for non-device functions.
     */
    int i;
    int length = sizeof(DEVICE_FUNCTIONS) / sizeof(DEVICE_FUNCTIONS[0]);

    for (i=0; i < length; ++i) {
        if (strcmp(DEVICE_FUNCTIONS[i], name) == 0) {
            return 1;
        }
    }

    return 0;
}

struct _glad_vulkan_userptr {
    void *vk_handle;
    VkInstance vk_instance;
    VkDevice vk_device;
    PFN_vkGetInstanceProcAddr get_instance_proc_addr;
    PFN_vkGetDeviceProcAddr get_device_proc_addr;
};

static GLADapiproc glad_vulkan_get_proc(void *vuserptr, const char *name) {
    struct _glad_vulkan_userptr userptr = *(struct _glad_vulkan_userptr*) vuserptr;
    PFN_vkVoidFunction result = NULL;

    if (userptr.vk_device != NULL && glad_vulkan_is_device_function(name)) {
        result = userptr.get_device_proc_addr(userptr.vk_device, name);
    }

    if (result == NULL && userptr.vk_instance != NULL) {
        result = userptr.get_instance_proc_addr(userptr.vk_instance, name);
    }

    if(result == NULL) {
        result = (PFN_vkVoidFunction) glad_dlsym_handle(userptr.vk_handle, name);
    }

    return (GLADapiproc) result;
}



static void* glad_vulkan_dlopen_handle(GladVulkanContext *context) {
    static const char *NAMES[] = {
#if GLAD_PLATFORM_APPLE
        "libvulkan.1.dylib",
#elif GLAD_PLATFORM_WIN32
        "vulkan-1.dll",
        "vulkan.dll",
#else
        "libvulkan.so.1",
        "libvulkan.so",
#endif
    };

    if (context->glad_loader_handle == NULL) {
        context->glad_loader_handle = glad_get_dlopen_handle(NAMES, sizeof(NAMES) / sizeof(NAMES[0]));
    }

    return context->glad_loader_handle;
}

static struct _glad_vulkan_userptr glad_vulkan_build_userptr(void *handle, VkInstance instance, VkDevice device) {
    struct _glad_vulkan_userptr userptr;
    userptr.vk_handle = handle;
    userptr.vk_instance = instance;
    userptr.vk_device = device;
    userptr.get_instance_proc_addr = (PFN_vkGetInstanceProcAddr) glad_dlsym_handle(handle, "vkGetInstanceProcAddr");
    userptr.get_device_proc_addr = (PFN_vkGetDeviceProcAddr) glad_dlsym_handle(handle, "vkGetDeviceProcAddr");
    return userptr;
}

int gladLoaderLoadVulkanContext(GladVulkanContext *context, VkInstance instance, VkPhysicalDevice physical_device, VkDevice device) {
    int version = 0;
    void *handle = NULL;
    int did_load = 0;
    struct _glad_vulkan_userptr userptr;

    did_load = context->glad_loader_handle == NULL;
    handle = glad_vulkan_dlopen_handle(context);
    if (handle != NULL) {
        userptr = glad_vulkan_build_userptr(handle, instance, device);

        if (userptr.get_instance_proc_addr != NULL && userptr.get_device_proc_addr != NULL) {
            version = gladLoadVulkanContextUserPtr(context, physical_device, glad_vulkan_get_proc, &userptr);
        }

        if (!version && did_load) {
            gladLoaderUnloadVulkanContext(context);
        }
    }

    return version;
}



void gladLoaderUnloadVulkanContext(GladVulkanContext *context) {
    if (context->glad_loader_handle != NULL) {
        glad_close_dlopen_handle(context->glad_loader_handle);
        context->glad_loader_handle = NULL;
    }
}

#endif /* GLAD_VULKAN */

#ifdef __cplusplus
}
#endif
