#pragma once

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

	struct GraphicsAPICreateInfo
	{
		GraphicsAPI API	  = GraphicsAPI::OpenGL;
		bool		Debug = false;
	};

}	 // namespace Nexus::Graphics
