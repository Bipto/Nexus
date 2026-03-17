#pragma once

#include <cstdint>

namespace Nexus::Graphics
{
	/// @brief An enum class that represents an available graphics API backend
	enum class GraphicsAPI : uint8_t
	{
		/// @brief Graphics will be created using OpenGL
		OpenGL,

		/// @brief Graphics will be created using Direct3D12
		D3D12,

		/// @brief Graphics will be created using Vulkan
		Vulkan
	};

	/// @brief A structure containing the in-use graphics API and it's version
	struct GraphicsAPIInfo
	{
		/// @brief The graphics API that is in use
		GraphicsAPI API = {};

		/// @brief Integer representing the major version of the API
		uint32_t Major = {};

		/// @brief Integer representing the minor version of the API
		uint32_t Minor = {};
	};
}	 // namespace Nexus::Graphics