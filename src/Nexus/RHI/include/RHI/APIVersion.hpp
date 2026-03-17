#pragma once

#include <cstdint>

namespace Nexus::Graphics
{
	/// @brief An enum class that represents an available graphics API backend
	enum class GraphicsAPI
	{
		/// @brief Graphics will be created using OpenGL
		OpenGL,

		/// @brief Graphics will be created using Direct3D12
		D3D12,

		/// @brief Graphics will be created using Vulkan
		Vulkan
	};

	struct GraphicsAPIInfo
	{
		GraphicsAPI API	  = {};
		uint32_t	Major = {};
		uint32_t	Minor = {};
	};
}	 // namespace Nexus::Graphics