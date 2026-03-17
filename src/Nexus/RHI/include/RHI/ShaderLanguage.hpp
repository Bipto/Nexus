#pragma once

namespace Nexus::Graphics
{
	/// @brief An enum representing different shader languages
	enum class ShaderLanguage
	{
		/// @brief Value representing the GLSL shader language
		GLSL,

		/// @brief Value representing the GLSLES shader language
		GLSLES,

		/// @brief Value representing the HLSL shader language
		HLSL,

		/// @brief Value representing a SPIR-V binary shader
		Vulkan_SPIRV,

		OpenGL_SPIRV
	};
}	 // namespace Nexus::Graphics