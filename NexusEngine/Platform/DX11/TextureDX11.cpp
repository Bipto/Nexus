#include "TextureDX11.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    TextureDX11::TextureDX11(ID3D11Device* device, const char* filepath) : Texture(filepath)
    {
        int desiredChannels = 4;
        this->m_Data = stbi_load(filepath, &m_Width, &m_Height, &m_NumOfChannels, desiredChannels);

        int imagePitch = m_Width * 4;

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = m_Width;
        desc.Height = m_Height;
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
        subresourceData.pSysMem = m_Data;
        subresourceData.SysMemPitch = imagePitch;

        HRESULT hr = device->CreateTexture2D(
            &desc,
            &subresourceData,
            &m_Texture
        );

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create texture: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }
        else
        {
            NX_LOG("Texture created successfully");
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
        ZeroMemory(&rvDesc, sizeof(rvDesc));
        rvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
        else
        {
            NX_LOG("Resource view created successfully");
        }
    }

    TextureDX11::~TextureDX11()
    {
        m_Texture->Release();
        stbi_image_free(m_Data);
    }

    void TextureDX11::Bind(unsigned int slot)
    {

    }

    void* TextureDX11::GetHandle()
    {
        return m_ResourceView;
    }
}