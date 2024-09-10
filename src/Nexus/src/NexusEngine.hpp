#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Audio/AudioDevice.hpp"
#include "Nexus-Core/EntryPoint.hpp"
#include "Nexus-Core/Events/Event.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Graphics/Framebuffer.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Mesh.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Input/Input.hpp"
#include "Nexus-Core/Logging/Log.hpp"
#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/Renderer.hpp"
#include "Nexus-Core/Runtime.hpp"
#include "Nexus-Core/Runtime/Camera.hpp"
#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/Vertex.hpp"
#include "Nexus-Core/Window.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/// @brief A struct that is used to track the allocations of the engine
struct AllocationTracker
{
	/// @brief The total amount of bytes allocated throughout the lifetime of the
	/// program
	uint32_t TotalAllocated = 0;

	/// @brief The total amount of bytes released throughout the lifetime of the
	/// program
	uint32_t TotalFreed = 0;

	/// @brief A method that returns the currently used memory by the application
	/// in bytes
	/// @return The amount of bytes allocated
	uint32_t GetCurrentUsage()
	{
		return TotalAllocated - TotalFreed;
	}
};

/// @brief A static allocation tracker that is used to track the new and delete
/// keywords
static AllocationTracker s_AllocationTracker;

/// @brief A method that returns the current application memory usage in bytes
/// @return The total amount of bytes currently allocated
uint32_t GetCurrentMemoryUsage()
{
	return s_AllocationTracker.GetCurrentUsage();
}

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