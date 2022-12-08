#pragma once

#include "Core/Runtime.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Size.h"
#include "Core/Vertex.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Swapchain.h"

#include "Core/Logging/Log.h"

#define NX_LOG(message) Nexus::GetCoreLogger()->LogInfo(message)
#define NX_WARNING(message) Nexus::GetCoreLogger()->LogWarning(message);
#define NX_ERROR(message) Nexus::GetCoreLogger()->LogError(message);