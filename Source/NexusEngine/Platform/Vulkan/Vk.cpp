#include "Vk.hpp"

VkFormat GetVkFormatFromNexusFormat(Nexus::Graphics::TextureFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::TextureFormat::RGBA8:
        return VK_FORMAT_R8G8B8_UINT;

    default:
        throw std::runtime_error("Failed to find a valid format");
        break;
    }
}