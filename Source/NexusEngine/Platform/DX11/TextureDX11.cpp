#if defined(WIN32)

#include "TextureDX11.hpp"
#include "Core/Logging/Log.hpp"

namespace Nexus::Graphics
{
    TextureDX11::TextureDX11(Microsoft::WRL::ComPtr<ID3D11Device> device, const TextureSpecification &spec) : Texture(spec)
    {
        int imagePitch = spec.Width * 4;

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = spec.Width;
        desc.Height = spec.Height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA subresourceData;
        subresourceData.pSysMem = spec.Data;
        subresourceData.SysMemPitch = imagePitch;

        HRESULT hr = device->CreateTexture2D(
            &desc,
            &subresourceData,
            &m_Texture);

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create texture: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
        ZeroMemory(&rvDesc, sizeof(rvDesc));
        rvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        rvDesc.Texture2D.MipLevels = desc.MipLevels;
        rvDesc.Texture2D.MostDetailedMip = 0;
        hr = device->CreateShaderResourceView(m_Texture.Get(), &rvDesc, m_ResourceView.GetAddressOf());

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create resource view: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }

        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxAnisotropy = D3D11_FLOAT32_MAX;

        hr = device->CreateSamplerState(&samplerDesc, &m_SamplerState);
    }

    TextureDX11::~TextureDX11()
    {
    }

    void *TextureDX11::GetHandle()
    {
        return m_ResourceView.Get();
    }
}

#endif