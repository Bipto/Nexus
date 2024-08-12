#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    enum class ImageLayout
    {
        Undefined = 0,
        General,
        Colour,
        DepthRead,
        DepthReadWrite,
        ShaderRead,
        ResolveSource,
        ResolveDestination,
        Present
    };

    inline std::string ImageLayoutToString(ImageLayout layout)
    {
        switch (layout)
        {
        case ImageLayout::Undefined:
            return "ImageLayout::Undefined";
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
        case ImageLayout::Present:
            return "ImageLayout::Present";
        default:
            throw std::runtime_error("Failed to find a valid image layout");
        }
    }
}