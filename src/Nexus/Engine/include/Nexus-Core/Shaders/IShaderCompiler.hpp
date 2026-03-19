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

	struct ShaderCompilationOptions
	{
		std::string				  EntryPoint		= "main";
		Graphics::GraphicsAPIInfo TargetEnvironment = {};
		Graphics::ShaderStage	  Stage				= {};
		std::vector<ShaderDefine> Defines			= {};
		Graphics::ShaderLanguage  InputLanguage		= {};
		ShaderTargetVersion		  InputVersion		= {};
		Graphics::ShaderLanguage  OutputLanguage	= {};
		ShaderTargetVersion		  OutputVersion		= {};
		ShaderOptimisationLevel	  OptimisationLevel = ShaderOptimisationLevel::Performance;
		bool					  Debug				= false;
	};

	struct ShaderCompilationResult
	{
		std::vector<std::byte> OutputSource = {};
	};

	class NX_API IShaderCompiler
	{
	  public:
		virtual ~IShaderCompiler() = default;

		virtual std::expected<ShaderCompilationResult, std::string> CompileShader(std::string_view				  shaderSource,
																				  std::string_view				  shaderName,
																				  const ShaderCompilationOptions &options) const = 0;
	};
}	 // namespace Nexus