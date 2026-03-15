#pragma once

#include "RHI/Pipeline.hpp"
#include "RHI/ResourceSet.hpp"
#include "RHI/ResourceSetBinding.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace Nexus::Graphics
{
	/// @brief A binding of a compute pipeline to the command list and the resources to use with it
	struct ComputeState
	{
		/// @brief The compute pipeline to bind to the command list
		IComputePipeline *ComputePipeline = nullptr;

		/// @brief The resources to bind to the command list
		ResourceSetBinding Resources = {};
	};
}	 // namespace Nexus::Graphics