#pragma once

#include "RHI/DeviceBuffer.hpp"
#include "RHI/Framebuffer.hpp"
#include "RHI/Pipeline.hpp"
#include "RHI/ResourceSet.hpp"
#include "RHI/ResourceSetBinding.hpp"
#include "RHI/Scissor.hpp"
#include "RHI/Viewport.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace Nexus::Graphics
{
    /// @brief A binding of a meshlet pipeline to the command list and the resources
    /// to use with it
    struct RayTracingState
    {
        /// @brief The ray tracing pipeline to bind to the command list
        IRayTracingPipeline *RayTracingPipeline = nullptr;

        /// @brief The resources to bind to the command list
        ResourceSetBinding Resources = {};
    };
} // namespace Nexus::Graphics