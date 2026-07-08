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

#include "Nexus-Core/Resources/IResourceLoader.hpp"
#include "Nexus-Core/Shaders/IShaderCompiler.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
    class NX_API SPIRVCrossShaderCompiler final : public IShaderCompiler
    {
      public:
        std::expected<ShaderCompilationResult, std::string> Compile(
            const ShaderCompilationOptions &options
        ) const final;
    };
} // namespace Nexus