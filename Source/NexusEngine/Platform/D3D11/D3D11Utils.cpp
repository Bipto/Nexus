#include "D3D11Utils.hpp"

DXGI_FORMAT GetD3D11TextureFormat(Nexus::Graphics::TextureFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::TextureFormat::RGBA8:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Nexus::Graphics::TextureFormat::RedInteger:
        return DXGI_FORMAT_R8_UNORM;
    default:
        throw std::runtime_error("Failed to find a valid format");
    }
}

DXGI_FORMAT GetD3D11DepthFormat(Nexus::Graphics::DepthFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::DepthFormat::DEPTH24STENCIL8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;

    default:
        throw std::runtime_error("Failed to find a valid format");
    }
}