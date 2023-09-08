#include "Vk.hpp"

#if defined(NX_PLATFORM_VULKAN)

VkFormat GetVkTextureFormatFromNexusFormat(Nexus::Graphics::TextureFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::TextureFormat::RGBA8:
        return VK_FORMAT_R8G8B8A8_UNORM;

    default:
        throw std::runtime_error("Failed to find a valid format");
        break;
    }
}

VkFormat GetVkDepthFormatFromNexusFormat(Nexus::Graphics::DepthFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::DepthFormat::DEPTH24STENCIL8:
        return VK_FORMAT_D24_UNORM_S8_UINT;

    default:
        throw std::runtime_error("Failed to find a valid format");
        break;
    }
}

VkFormat GetShaderDataType(Nexus::Graphics::ShaderDataType type)
{
    switch (type)
    {
    case Nexus::Graphics::ShaderDataType::Float:
        return VK_FORMAT_R32_SFLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float2:
        return VK_FORMAT_R32G32_SFLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float3:
        return VK_FORMAT_R32G32B32_SFLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case Nexus::Graphics::ShaderDataType::Int:
        return VK_FORMAT_R32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int2:
        return VK_FORMAT_R32G32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int3:
        return VK_FORMAT_R32G32B32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int4:
        return VK_FORMAT_R32G32B32A32_SINT;
        break;
    default:
        throw std::runtime_error("An invalid data type was entered");
        break;
    }
}

#endif