#pragma once

#include "Core/nxpch.hpp"

#include "Core/Runtime.hpp"
#include "Core/Application.hpp"
#include "Core/Window.hpp"
#include "Core/Point.hpp"
#include "Core/Vertex.hpp"
#include "Core/Memory.hpp"

#include "Core/Events/Event.hpp"
#include "Core/Events/EventHandler.hpp"

#include "Core/Graphics/GraphicsDevice.hpp"
#include "Core/Graphics/Texture.hpp"
#include "Core/Graphics/TextureFormat.hpp"
#include "Core/Graphics/Framebuffer.hpp"
#include "Core/Graphics/DepthFormat.hpp"

#include "Core/Audio/AudioDevice.hpp"

#include "Core/Renderer.hpp"
#include "Core/Logging/Log.hpp"
#include "Core/Input/Input.hpp"

#include "Core/EntryPoint.hpp"

#include "Runtime/Camera.hpp"
#include "Runtime/Project.hpp"
#include "Runtime/Scene.hpp"

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