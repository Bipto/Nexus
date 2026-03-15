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
	/// @brief A binding of a meshlet pipeline to the command list and the resources to use with it
	struct MeshletState
	{
		/// @brief The meshlet pipeline to bind to the command list
		IMeshletPipeline *MeshletPipeline = nullptr;

		/// @brief The resources to bind to the command list
		ResourceSetBinding Resources = {};

		/// @brief The framebuffer to bind to the command list
		IFramebuffer *Framebuffer = nullptr;

		/// @brief The viewports to bind to the command list
		std::array<Viewport, MaxViewports> Viewports = {};

		/// @brief The scissors to bind to the command list
		std::array<Scissor, MaxScissors> Scissors = {};
	};
}	 // namespace Nexus::Graphics