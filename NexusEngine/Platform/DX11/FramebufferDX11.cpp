#include "FramebufferDX11.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    FramebufferDX11::FramebufferDX11(ID3D11Device* device, const Nexus::FramebufferSpecification &spec)
    {
        m_Device = device;
        this->m_FramebufferSpecification = spec;
        Resize();
    }

    FramebufferDX11::~FramebufferDX11()
    {
    }

    void FramebufferDX11::Resize()
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(textureDesc));
        textureDesc.Width = m_FramebufferSpecification.Width;
        textureDesc.Height = m_FramebufferSpecification.Height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;
        HRESULT hr = m_Device->CreateTexture2D(&textureDesc, NULL, &m_RenderTargetTextureMap);

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string output = std::string("Failed to create framebuffer texture: ") + error.ErrorMessage();
            NX_ERROR(output);
        }
        else
        {
            NX_LOG("Framebuffer texture created successfully");
        }

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;
        hr = m_Device->CreateRenderTargetView(m_RenderTargetTextureMap, &renderTargetViewDesc, &m_RenderTargetViewMap);

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string output = std::string("Failed to create framebuffer render target view: ") + error.ErrorMessage();
            NX_ERROR(output);
        }
        else
        {
            NX_LOG("Framebuffer render target view created successfully");
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;
        hr = m_Device->CreateShaderResourceView(m_RenderTargetTextureMap, &shaderResourceViewDesc, &m_ShaderResourceView);
    
        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string output = std::string("Failed to create framebuffer shader resource view: ") + error.ErrorMessage();
            NX_ERROR(output);
        }
        else
        {
            NX_LOG("Framebuffer shader resource view created successfully");
        }
    }

    int FramebufferDX11::GetColorTextureCount()
    {
        return 0;
    }

    bool FramebufferDX11::HasColorTexture()
    {
        return m_RenderTargetTextureMap;
    }

    bool FramebufferDX11::HasDepthTexture()
    {
        return false;
    }

    void* FramebufferDX11::GetColorAttachment(int index)
    {
        return m_ShaderResourceView;
    }

    void* FramebufferDX11::GetDepthAttachment()
    {
        return 0;
    }

    const FramebufferSpecification FramebufferDX11::GetFramebufferSpecification()
    {
        return m_FramebufferSpecification;
    }

    void FramebufferDX11::SetFramebufferSpecification(const FramebufferSpecification &spec)
    {
        m_FramebufferSpecification = spec;
        Resize();
    }
}