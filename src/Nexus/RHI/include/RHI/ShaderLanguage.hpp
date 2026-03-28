#pragma once

#include <cstdint>

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

		/// @brief Value representing a SPIR-V Vulkan compatible binary shader
		Vulkan_SPIRV,

		/// @brief Value representing a SPIR-V OpenGL compatible binary shader
		OpenGL_SPIRV
	};

	/// @brief A structure representing the specific version of a shader language that should be used as input/targetted for output of the compilation
	/// (e.g. GLSL 450 would be 4.5 in this format)
	struct ShaderVersion
	{
		uint16_t Major = 0;
		uint16_t Minor = 0;
	};

	struct ShaderProfile
	{
		ShaderLanguage Language = {};
		ShaderVersion  Version	= {};
	};

	inline bool IsBinaryShaderFormat(ShaderLanguage language)
	{
		switch (language)
		{
			case ShaderLanguage::Vulkan_SPIRV:
			case ShaderLanguage::OpenGL_SPIRV: return true;
			default: return false;
		}
	}
}	 // namespace Nexus::Graphics