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
        CreateTextures();
    }

    int FramebufferDX11::GetColorTextureCount()
    {
        return m_ColorRenderTargets.size();
    }

    bool FramebufferDX11::HasColorTexture()
    {
        return m_ColorRenderTargets.size() > 0;
    }

    bool FramebufferDX11::HasDepthTexture()
    {
        return m_FramebufferSpecification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None;
    }

    void* FramebufferDX11::GetColorAttachment(int index)
    {
        return m_ColorRenderTargets[index].ShaderResourceView;
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

    void FramebufferDX11::CreateTextures()
    {
        DeleteTextures();

        //create color textures
        {
            m_ColorRenderTargets.clear();

            for (auto colorAttachments : m_FramebufferSpecification.ColorAttachmentSpecification.Attachments)
            {
                FramebufferColorRenderTarget target;

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
                HRESULT hr = m_Device->CreateTexture2D(&textureDesc, NULL, &target.Texture);

                if (FAILED(hr))
                {
                    _com_error error(hr);
                    std::string output = std::string("Failed to created framebuffer texture: ") + error.ErrorMessage();
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
                hr = m_Device->CreateRenderTargetView(target.Texture, &renderTargetViewDesc, &target.RenderTargetView);

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
                hr = m_Device->CreateShaderResourceView(target.Texture,  &shaderResourceViewDesc, &target.ShaderResourceView);

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

                m_ColorRenderTargets.push_back(target);
            }
        }

        //create depth target if requested
        {
            FramebufferDepthRenderTarget depthTarget;

            D3D11_TEXTURE2D_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.Width = m_FramebufferSpecification.Width;
            depthStencilDesc.Height = m_FramebufferSpecification.Height;
            depthStencilDesc.MipLevels = 1;
            depthStencilDesc.ArraySize = 1;
            depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilDesc.SampleDesc.Count = 1;
            depthStencilDesc.SampleDesc.Quality = 0;
            depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
            depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            depthStencilDesc.CPUAccessFlags = 0;
            depthStencilDesc.MiscFlags = 0;

            HRESULT hr = m_Device->CreateTexture2D(&depthStencilDesc, NULL, &depthTarget.Texture);

            if (FAILED(hr))
            {
                _com_error error(hr);
                std::string output = std::string("Failed to create framebuffer depth texture: ") + error.ErrorMessage();
                NX_ERROR(output);
            }
            else
            {
                NX_LOG("Framebuffer depth texture created successfully");
            }

            hr = m_Device->CreateDepthStencilView(depthTarget.Texture, NULL, &depthTarget.DepthStencilView);

            if (FAILED(hr))
            {
                _com_error error(hr);
                std::string output = std::string("Failed to create framebuffer depth stencil view: ") + error.ErrorMessage();
                NX_ERROR(output);
            }
            else
            {
                NX_LOG("Framebuffer depth stencil view created successfully");
            }
        }        
    }

    void FramebufferDX11::DeleteTextures()
    {
        for (auto colorAttachment : m_ColorRenderTargets)
        {
            colorAttachment.RenderTargetView->Release();
            colorAttachment.RenderTargetView = nullptr;

            colorAttachment.ShaderResourceView->Release();
            colorAttachment.ShaderResourceView = nullptr;
            
            colorAttachment.Texture->Release();
            colorAttachment.Texture = nullptr;
        }

        if (HasDepthTexture())
        {
            m_DepthTarget.DepthStencilView->Release();
            m_DepthTarget.DepthStencilView = nullptr;

            m_DepthTarget.Texture->Release();
            m_DepthTarget.Texture = nullptr;
        }

        m_ColorRenderTargets.clear();
    }
}