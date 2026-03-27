#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "RHI/APIVersion.hpp"
#include "RHI/GraphicsAPICreateInfo.hpp"
#include "RHI/ShaderLanguage.hpp"
#include "RHI/ShaderReflectionData.hpp"
#include "RHI/ShaderResources.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief A structure representing the specific version of a shader language that should be used as input/targetted for output of the compilation
	/// (e.g. GLSL 450 would be 4.5 in this format)
	struct ShaderTargetVersion
	{
		uint16_t Major = 0;
		uint16_t Minor = 0;
	};

	enum class ShaderOptimisationLevel
	{
		None,
		Performance,
		Size
	};

	struct ShaderDefine
	{
		std::string				   Name	 = {};
		std::optional<std::string> Value = {};
	};

	struct ShaderSourceInput
	{
		std::string_view		   SourceText	= {};
		std::span<const std::byte> SourceBinary = {};
	};

	struct ShaderCompilationInputDescription
	{
		Graphics::ShaderLanguage  ShaderLanguage = {};
		ShaderTargetVersion		  ShaderVersion	 = {};
		std::string				  EntryPoint	 = "main";
		std::vector<ShaderDefine> Defines		 = {};
	};

	struct ShaderCompilationOutputDescription
	{
		Graphics::ShaderLanguage ShaderLanguage	   = {};
		ShaderTargetVersion		 ShaderVersion	   = {};
		ShaderOptimisationLevel	 OptimisationLevel = ShaderOptimisationLevel::Performance;
		bool					 Debug			   = false;
	};

	struct ShaderCompilationResult
	{
		std::string					   OutputText	  = {};
		std::vector<std::byte>		   OutputBinary	  = {};
		Graphics::ShaderReflectionData ReflectionData = {};
		std::vector<std::string>	   Warnings		  = {};
	};

	struct ShaderCompilationOptions
	{
		ShaderSourceInput				   SourceInput			 = {};
		ShaderCompilationInputDescription  InputCompilationDesc	 = {};
		ShaderCompilationOutputDescription OutputCompilationDesc = {};
		Graphics::GraphicsAPIInfo		   Environment			 = {};
		Graphics::ShaderStage			   Stage				 = {};
		std::string						   DebugName			 = {};
	};

	class NX_API IShaderCompiler
	{
	  public:
		virtual ~IShaderCompiler() = default;

		virtual std::expected<ShaderCompilationResult, std::string> Compile(const ShaderCompilationOptions &options) const = 0;
	};
}	 // namespace Nexus