#pragma once

#include "Nexus/nxpch.hpp"

#include "Nexus/Runtime.hpp"
#include "Nexus/Application.hpp"
#include "Nexus/Window.hpp"
#include "Nexus/Point.hpp"
#include "Nexus/Vertex.hpp"
#include "Nexus/Types.hpp"

#include "Nexus/Events/Event.hpp"
#include "Nexus/Events/EventHandler.hpp"

#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "Nexus/Graphics/Texture.hpp"
#include "Nexus/Graphics/Framebuffer.hpp"
#include "Nexus/Graphics/Mesh.hpp"
#include "Nexus/Graphics/MeshFactory.hpp"

#include "Nexus/Audio/AudioDevice.hpp"

#include "Nexus/Renderer.hpp"
#include "Nexus/Logging/Log.hpp"
#include "Nexus/Input/Input.hpp"

#include "Nexus/EntryPoint.hpp"

#include "Nexus/Runtime/Camera.hpp"
#include "Nexus/Runtime/Project.hpp"
#include "Nexus/Runtime/Scene.hpp"

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