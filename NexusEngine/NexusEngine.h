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

/// @brief A struct that is used to track the allocations of the engine
struct AllocationTracker
{
    /// @brief The total amount of bytes allocated throughout the lifetime of the program
    uint32_t TotalAllocated = 0;

    /// @brief The total amount of bytes released throughout the lifetime of the program
    uint32_t TotalFreed = 0;

    /// @brief A method that returns the currently used memory by the application in bytes
    /// @return The amount of bytes allocated
    uint32_t GetCurrentUsage() { return TotalAllocated - TotalFreed; }
};

/// @brief A static allocation tracker that is used to track the new and delete keywords
static AllocationTracker s_AllocationTracker;

/// @brief A method that returns the current application memory usage in bytes
/// @return The total amount of bytes currently allocated
uint32_t GetCurrentMemoryUsage() { return s_AllocationTracker.GetCurrentUsage(); }

// custom memory allocator
void *operator new(size_t size)
{
    s_AllocationTracker.TotalAllocated += size;
    return malloc(size);
}

// custom memory deallocator
void operator delete(void *memory, size_t size)
{
    s_AllocationTracker.TotalFreed += size;
    free(memory);
}