#pragma once

#include "RHI/ResourceSet.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace Nexus::Graphics
{
    /// @brief Struct representing a set of resources bound to the current pipeline
    /// and any offsets required for dynamic resources
    struct ResourceSetBinding
    {
        /// @brief The resources to bind to the pipeline
        IResourceSet *ResourceSet = nullptr;

        /// @brief The offsets to use for dynamic resources in the resource set
        std::map<std::string, std::vector<uint32_t>> DynamicOffsets = {};
    };
} // namespace Nexus::Graphics