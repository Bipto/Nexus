#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    enum class ImageLayout
    {
        General = 0,
        Colour,
        DepthRead,
        DepthReadWrite,
        ShaderRead,
        ResolveSource,
        ResolveDestination,
        CopySource,
        CopyDestination,
        Present
    };

    inline std::string ImageLayoutToString(ImageLayout layout)
    {
        switch (layout)
        {
        case ImageLayout::General:
            return "ImageLayout::General";
        case ImageLayout::Colour:
            return "ImageLayout::Colour";
        case ImageLayout::DepthRead:
            return "ImageLayout::DepthRead";
        case ImageLayout::DepthReadWrite:
            return "ImageLayout::DepthReadWrite";
        case ImageLayout::ShaderRead:
            return "ImageLayout::ShaderRead";
        case ImageLayout::ResolveSource:
            return "ImageLayout::ResolveSource";
        case ImageLayout::ResolveDestination:
            return "ImageLayout::ResolveDestination";
        case ImageLayout::CopySource:
            return "ImageLayout::CopySource";
        case ImageLayout::CopyDestination:
            return "ImageLayout::CopyDestination";
        case ImageLayout::Present:
            return "ImageLayout::Present";
        default:
            throw std::runtime_error("Failed to find a valid image layout");
        }
    }
}