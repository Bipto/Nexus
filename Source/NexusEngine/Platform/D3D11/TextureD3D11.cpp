#if defined(WIN32)

#include "TextureD3D11.hpp"
#include "D3D11Utils.hpp"
#include "Nexus/Logging/Log.hpp"

namespace Nexus::Graphics
{
    TextureD3D11::TextureD3D11(ID3D11Device *device, ID3D11DeviceContext *context, const TextureSpecification &spec) : Texture(spec)
    {
        m_DeviceContext = context;

        m_RowPitch = spec.Width * spec.NumberOfChannels;
        m_TextureFormat = GetD3D11TextureFormat(spec.Format);

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = spec.Width;
        desc.Height = spec.Height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = m_TextureFormat;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;

        HRESULT hr = device->CreateTexture2D(
            &desc,
            nullptr,
            &m_Texture);

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create texture: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
        ZeroMemory(&rvDesc, sizeof(rvDesc));
        rvDesc.Format = m_TextureFormat;
        rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        rvDesc.Texture2D.MipLevels = desc.MipLevels;
        rvDesc.Texture2D.MostDetailedMip = 0;
        hr = device->CreateShaderResourceView(m_Texture, &rvDesc, &m_ResourceView);

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

    TextureD3D11::~TextureD3D11()
    {
        m_SamplerState->Release();
        m_ResourceView->Release();
        m_Texture->Release();
    }

    ResourceHandle TextureD3D11::GetHandle()
    {
        return (ResourceHandle)m_ResourceView;
    }

    void TextureD3D11::SetData(void *data, uint32_t size)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        m_DeviceContext->Map(m_Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        // memcpy(mappedResource.pData, data, size);

        BYTE *mappedData = (BYTE *)mappedResource.pData;
        BYTE *buffer = (BYTE *)data;

        D3D11_TEXTURE2D_DESC desc;
        m_Texture->GetDesc(&desc);

        for (uint32_t i = 0; i < desc.Height; ++i)
        {
            memcpy(mappedData, buffer, m_RowPitch);
            mappedData += mappedResource.RowPitch;
            buffer += m_RowPitch;
        }

        m_DeviceContext->Unmap(m_Texture, 0);
    }
}

#endif