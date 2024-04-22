#pragma once

namespace Nexus::Graphics
{
    enum class PixelFormat
    {
        None = 0,
        B8_G8_R8_A8_UNorm,
        B8_G8_R8_A8_UNorm_SRGB,
        BC1_Rgb_UNorm,
        BC1_Rgb_UNorm_SRGB,
        BC1_Rgba_UNorm,
        BC1_Rgba_UNorm_SRGB,
        BC2_UNorm,
        BC2_UNorm_SRGB,
        BC3_UNorm,
        BC3_UNorm_SRGB,
        BC4_SNorm,
        BC4_UNorm,
        BC5_SNorm,
        BC5_UNorm,
        BC7_UNorm,
        BC7_UNorm_SRGB,
        D24_UNorm_S8_UInt,
        D32_Float_S8_UInt,
        R10_G10_B10_A2_UInt,
        R10_G10_B10_A2_UNorm,
        R11_G11_B10_Float,
        R16_Float,
        R16_G16_B16_A16_Float,
        R16_G16_B16_A16_SInt,
        R16_G16_B16_A16_SNorm,
        R16_G16_B16_A16_UInt,
        R16_G16_B16_A16_UNorm,
        R16_G16_Float,
        R16_G16_SInt,
        R16_G16_SNorm,
        R16_G16_UInt,
        R16_G16_UNorm,
        R16_SInt,
        R16_SNorm,
        R16_UInt,
        R16_UNorm,
        R32_Float,
        R32_G32_B32_A32_Float,
        R32_G32_B32_A32_SInt,
        R32_G32_B32_A32_UInt,
        R32_G32_Float,
        R32_G32_SInt,
        R32_G32_UInt,
        R32_SInt,
        R32_UInt,
        R8_G8_B8_A8_SInt,
        R8_G8_B8_A8_SNorm,
        R8_G8_B8_A8_UInt,
        R8_G8_B8_A8_UNorm,
        R8_G8_B8_A8_UNorm_SRGB,
        R8_G8_SInt,
        R8_G8_SNorm,
        S8_G8_UInt,
        R8_G8_UInt,
        R8_G8_UNorm,
        R8_SInt,
        R8_SNorm,
        R8_UInt,
        R8_UNorm,
    };

    static size_t GetPixelFormatSizeInBytes(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::B8_G8_R8_A8_UNorm:
        case PixelFormat::B8_G8_R8_A8_UNorm_SRGB:
            return 32;
        case PixelFormat::BC1_Rgb_UNorm:
        case PixelFormat::BC1_Rgb_UNorm_SRGB:
        case PixelFormat::BC1_Rgba_UNorm:
        case PixelFormat::BC1_Rgba_UNorm_SRGB:
        case PixelFormat::BC2_UNorm:
        case PixelFormat::BC2_UNorm_SRGB:
        case PixelFormat::BC3_UNorm:
        case PixelFormat::BC3_UNorm_SRGB:
        case PixelFormat::BC4_SNorm:
        case PixelFormat::BC4_UNorm:
        case PixelFormat::BC5_SNorm:
        case PixelFormat::BC5_UNorm:
        case PixelFormat::BC7_UNorm:
        case PixelFormat::BC7_UNorm_SRGB:
            throw std::runtime_error("Getting a size in bytes is not supported on compressed formats");

        case PixelFormat::D24_UNorm_S8_UInt:
            return 32;
        case PixelFormat::D32_Float_S8_UInt:
            return 40;
        case PixelFormat::R10_G10_B10_A2_UInt:
        case PixelFormat::R10_G10_B10_A2_UNorm:
        case PixelFormat::R11_G11_B10_Float:
            return 32;
        case PixelFormat::R16_Float:
            return 16;
        case PixelFormat::R16_G16_B16_A16_Float:
        case PixelFormat::R16_G16_B16_A16_SInt:
        case PixelFormat::R16_G16_B16_A16_SNorm:
        case PixelFormat::R16_G16_B16_A16_UInt:
        case PixelFormat::R16_G16_B16_A16_UNorm:
            return 64;
        case PixelFormat::R16_G16_Float:
        case PixelFormat::R16_G16_SInt:
        case PixelFormat::R16_G16_SNorm:
        case PixelFormat::R16_G16_UInt:
        case PixelFormat::R16_G16_UNorm:
            return 32;
        case PixelFormat::R16_SInt:
        case PixelFormat::R16_SNorm:
        case PixelFormat::R16_UInt:
        case PixelFormat::R16_UNorm:
            return 16;
        case PixelFormat::R32_Float:
            return 32;
        case PixelFormat::R32_G32_B32_A32_Float:
        case PixelFormat::R32_G32_B32_A32_SInt:
        case PixelFormat::R32_G32_B32_A32_UInt:
            return 128;
        case PixelFormat::R32_G32_Float:
        case PixelFormat::R32_G32_SInt:
        case PixelFormat::R32_G32_UInt:
            return 64;
        case PixelFormat::R32_SInt:
        case PixelFormat::R32_UInt:
            return 32;
        case PixelFormat::R8_G8_B8_A8_SInt:
        case PixelFormat::R8_G8_B8_A8_SNorm:
        case PixelFormat::R8_G8_B8_A8_UInt:
        case PixelFormat::R8_G8_B8_A8_UNorm:
        case PixelFormat::R8_G8_B8_A8_UNorm_SRGB:
            return 32;
        case PixelFormat::R8_G8_SInt:
        case PixelFormat::R8_G8_SNorm:
        case PixelFormat::S8_G8_UInt:
        case PixelFormat::R8_G8_UInt:
        case PixelFormat::R8_G8_UNorm:
            return 16;
        case PixelFormat::R8_SInt:
        case PixelFormat::R8_SNorm:
        case PixelFormat::R8_UInt:
        case PixelFormat::R8_UNorm:
            return 8;

        case PixelFormat::None:
            throw std::runtime_error("Invalid pixel format selected");
        }
    }

    static size_t GetPixelFormatNumberOfChannels(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::B8_G8_R8_A8_UNorm:
        case PixelFormat::B8_G8_R8_A8_UNorm_SRGB:
            return 4;
        case PixelFormat::BC1_Rgb_UNorm:
        case PixelFormat::BC1_Rgb_UNorm_SRGB:
        case PixelFormat::BC1_Rgba_UNorm:
        case PixelFormat::BC1_Rgba_UNorm_SRGB:
        case PixelFormat::BC2_UNorm:
        case PixelFormat::BC2_UNorm_SRGB:
        case PixelFormat::BC3_UNorm:
        case PixelFormat::BC3_UNorm_SRGB:
        case PixelFormat::BC4_SNorm:
        case PixelFormat::BC4_UNorm:
        case PixelFormat::BC5_SNorm:
        case PixelFormat::BC5_UNorm:
        case PixelFormat::BC7_UNorm:
        case PixelFormat::BC7_UNorm_SRGB:
            throw std::runtime_error("Getting the number of channels is not supported on compressed formats");

        case PixelFormat::D24_UNorm_S8_UInt:
            return 2;
        case PixelFormat::D32_Float_S8_UInt:
            return 2;
        case PixelFormat::R10_G10_B10_A2_UInt:
        case PixelFormat::R10_G10_B10_A2_UNorm:
        case PixelFormat::R11_G11_B10_Float:
            return 4;
        case PixelFormat::R16_Float:
            return 1;
        case PixelFormat::R16_G16_B16_A16_Float:
        case PixelFormat::R16_G16_B16_A16_SInt:
        case PixelFormat::R16_G16_B16_A16_SNorm:
        case PixelFormat::R16_G16_B16_A16_UInt:
        case PixelFormat::R16_G16_B16_A16_UNorm:
            return 4;
        case PixelFormat::R16_G16_Float:
        case PixelFormat::R16_G16_SInt:
        case PixelFormat::R16_G16_SNorm:
        case PixelFormat::R16_G16_UInt:
        case PixelFormat::R16_G16_UNorm:
            return 2;
        case PixelFormat::R16_SInt:
        case PixelFormat::R16_SNorm:
        case PixelFormat::R16_UInt:
        case PixelFormat::R16_UNorm:
            return 1;
        case PixelFormat::R32_Float:
            return 1;
        case PixelFormat::R32_G32_B32_A32_Float:
        case PixelFormat::R32_G32_B32_A32_SInt:
        case PixelFormat::R32_G32_B32_A32_UInt:
            return 4;
        case PixelFormat::R32_G32_Float:
        case PixelFormat::R32_G32_SInt:
        case PixelFormat::R32_G32_UInt:
            return 2;
        case PixelFormat::R32_SInt:
        case PixelFormat::R32_UInt:
            return 2;
        case PixelFormat::R8_G8_B8_A8_SInt:
        case PixelFormat::R8_G8_B8_A8_SNorm:
        case PixelFormat::R8_G8_B8_A8_UInt:
        case PixelFormat::R8_G8_B8_A8_UNorm:
        case PixelFormat::R8_G8_B8_A8_UNorm_SRGB:
            return 4;
        case PixelFormat::R8_G8_SInt:
        case PixelFormat::R8_G8_SNorm:
        case PixelFormat::S8_G8_UInt:
        case PixelFormat::R8_G8_UInt:
        case PixelFormat::R8_G8_UNorm:
            return 2;
        case PixelFormat::R8_SInt:
        case PixelFormat::R8_SNorm:
        case PixelFormat::R8_UInt:
        case PixelFormat::R8_UNorm:
            return 1;

        case PixelFormat::None:
            throw std::runtime_error("Invalid pixel format selected");
        }
    }
}