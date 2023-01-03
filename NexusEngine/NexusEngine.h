#pragma once

#include "Core/Runtime.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Size.h"
#include "Core/Vertex.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Swapchain.h"
#include "Core/Renderer.h"

#include "Core/Logging/Log.h"

#include <iostream>

#define NX_LOG(message) Nexus::GetCoreLogger()->LogInfo(message)
#define NX_WARNING(message) Nexus::GetCoreLogger()->LogWarning(message);
#define NX_ERROR(message) Nexus::GetCoreLogger()->LogError(message);

struct AllocationTracker
{
    uint32_t TotalAllocated = 0;
    uint32_t TotalFreed = 0;

    uint32_t GetCurrentUsage(){return TotalAllocated - TotalFreed;}
};

static AllocationTracker s_AllocationTracker;

uint32_t GetCurrentMemoryUsage(){return s_AllocationTracker.GetCurrentUsage();}

//custom memory allocator
void* operator new(size_t size)
{
    s_AllocationTracker.TotalAllocated += size;
    return malloc(size);
}

//custom memory deallocator
void operator delete(void* memory, size_t size)
{
    s_AllocationTracker.TotalFreed += size;
    free(memory);
}