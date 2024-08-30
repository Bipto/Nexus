#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Structures.hpp"

namespace Nexus::Graphics
{
/// @brief An enum representing a method of sampling a texture
enum SamplerState
{
    /// @brief Pixels will be linearly interpolated and pixels out of range will be clamped to the nearest value in the texture
    LinearClamp,

    /// @brief Pixels will be linearly interpolated and pixels out of range will be wrapped around into the texture
    LinearWrap,

    /// @brief Pixels will be clamped to the nearest value and pixels out of range will be clamped to the nearest value in the texture
    PointClamp,

    /// @brief Pixels will be clamped to the nearest value and pixels out of range will be wrapped around into the texture
    PointWrap
};

enum class BorderColor
{
    TransparentBlack,
    OpaqueBlack,
    OpaqueWhite
};

enum class SamplerAddressMode
{
    Border,
    Clamp,
    Mirror,
    MirrorOnce,
    Wrap
};

enum class SamplerFilter
{
    Anisotropic,

    MinPoint_MagPoint_MipPoint,
    MinPoint_MagPoint_MipLinear,
    MinPoint_MagLinear_MipPoint,
    MinPoint_MagLinear_MipLinear,

    MinLinear_MagPoint_MipPoint,
    MinLinear_MagPoint_MipLinear,
    MinLinear_MagLinear_MipPoint,
    MinLinear_MagLinear_MipLinear
};

struct SamplerSpecification
{
    SamplerAddressMode AddressModeU = SamplerAddressMode::Wrap;
    SamplerAddressMode AddressModeV = SamplerAddressMode::Wrap;
    SamplerAddressMode AddressModeW = SamplerAddressMode::Wrap;
    SamplerFilter SampleFilter = SamplerFilter::MinLinear_MagLinear_MipLinear;
    ComparisonFunction SamplerComparisonFunction = ComparisonFunction::Never;
    uint32_t MaximumAnisotropy = 1;
    uint32_t MinimumLOD = 0;
    uint32_t MaximumLOD = std::numeric_limits<uint32_t>::max();
    int32_t LODBias = 1;
    BorderColor TextureBorderColor = BorderColor::OpaqueBlack;
};
} // namespace Nexus::Graphics