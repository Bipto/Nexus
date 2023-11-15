#include "D3D12Utils.hpp"

DXGI_FORMAT GetD3D12TextureFormat(Nexus::Graphics::TextureFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::TextureFormat::RGBA8:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
}

DXGI_FORMAT GetD3D12DepthFormat(Nexus::Graphics::DepthFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::DepthFormat::DEPTH24STENCIL8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    }
}
