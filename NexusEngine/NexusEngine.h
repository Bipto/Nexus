#pragma once

#include "Core/nxpch.h"

#include "Core/Runtime.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Point.h"
#include "Core/Vertex.h"
#include "Core/Memory.h"

#include "Core/Events/Event.h"
#include "Core/Events/EventHandler.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Texture.h"
#include "Core/Graphics/TextureFormat.h"
#include "Core/Graphics/Framebuffer.h"
#include "Core/Graphics/DepthFormat.h"

#include "Core/Renderer.h"
#include "Core/Logging/Log.h"
#include "Core/Input/Input.h"

#include "Runtime/Camera.h"
#include "Runtime/Project.h"
#include "Runtime/Scene.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define NX_LOG(message) Nexus::GetCoreLogger()->LogInfo(message)
#define NX_WARNING(message) Nexus::GetCoreLogger()->LogWarning(message)
#define NX_ERROR(message) Nexus::GetCoreLogger()->LogError(message)

#define NX_IS_KEY_PRESSED(keycode) Nexus::GetApplication()->GetCoreInput()->IsKeyPressed(keycode)
#define NX_SCROLL_Y() Nexus::GetApplication()->GetCoreInput()->GetMouseScrollY()

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