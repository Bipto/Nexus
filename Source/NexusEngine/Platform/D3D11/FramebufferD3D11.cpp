#if defined(NX_PLATFORM_D3D11)

#include "FramebufferD3D11.hpp"
#include "Nexus/Logging/Log.hpp"

#include "D3D11Utils.hpp"

namespace Nexus::Graphics
{
#if defined(NX_PLATFORM_D3D11)
    FramebufferD3D11::FramebufferD3D11(ID3D11Device *&device, RenderPass *renderPass)
        : Framebuffer(renderPass)
    {
        m_Device = device;
        Recreate();
    }

    FramebufferD3D11::~FramebufferD3D11()
    {
        DeleteTextures();
    }

    void FramebufferD3D11::Recreate()
    {
        CreateTextures();
    }

    void *FramebufferD3D11::GetColorAttachment(int index)
    {
        return m_ColorRenderTargets[index].ShaderResourceView;
    }

    const FramebufferSpecification FramebufferD3D11::GetFramebufferSpecification()
    {
        return m_Specification;
    }

    void FramebufferD3D11::SetFramebufferSpecification(const FramebufferSpecification &spec)
    {
        m_Specification = spec;
        Recreate();
    }

    void FramebufferD3D11::CreateTextures()
    {
        DeleteTextures();

        // create color textures
        {
            for (auto colorAttachments : m_Specification.ColorAttachmentSpecification.Attachments)
            {
                FramebufferColorRenderTarget target;

                D3D11_TEXTURE2D_DESC textureDesc;
                ZeroMemory(&textureDesc, sizeof(textureDesc));
                textureDesc.Width = m_Specification.Width;
                textureDesc.Height = m_Specification.Height;
                textureDesc.MipLevels = 1;
                textureDesc.ArraySize = 1;
                textureDesc.Format = GetD3D11TextureFormat(colorAttachments.TextureFormat);
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
                D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
                shaderResourceViewDesc.Format = textureDesc.Format;
                shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
                shaderResourceViewDesc.Texture2D.MipLevels = 1;
                hr = m_Device->CreateShaderResourceView(target.Texture, &shaderResourceViewDesc, &target.ShaderResourceView);

                if (FAILED(hr))
                {
                    _com_error error(hr);
                    std::string output = std::string("Failed to create framebuffer shader resource view: ") + error.ErrorMessage();
                    NX_ERROR(output);
                }

                m_ColorRenderTargets.push_back(target);
            }
        }

        if (m_Specification.DepthAttachmentSpecification.DepthFormat == DepthFormat::None)
            return;

        // create depth target if requested
        {
            FramebufferDepthRenderTarget depthTarget;

            D3D11_TEXTURE2D_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.Width = m_Specification.Width;
            depthStencilDesc.Height = m_Specification.Height;
            depthStencilDesc.MipLevels = 1;
            depthStencilDesc.ArraySize = 1;
            depthStencilDesc.Format = GetD3D11DepthFormat(m_Specification.DepthAttachmentSpecification.DepthFormat);
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

            hr = m_Device->CreateDepthStencilView(depthTarget.Texture, NULL, &depthTarget.DepthStencilView);

            if (FAILED(hr))
            {
                _com_error error(hr);
                std::string output = std::string("Failed to create framebuffer depth stencil view: ") + error.ErrorMessage();
                NX_ERROR(output);
            }

            m_DepthTarget = depthTarget;
        }
    }

    void FramebufferD3D11::DeleteTextures()
    {
        for (int i = 0; i < m_ColorRenderTargets.size(); i++)
        {
            m_ColorRenderTargets[i].RenderTargetView->Release();
            m_ColorRenderTargets[i].ShaderResourceView->Release();
            m_ColorRenderTargets[i].Texture->Release();
        }

        m_ColorRenderTargets.clear();

        if (m_Specification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None)
        {
            m_DepthTarget.Texture->Release();
            m_DepthTarget.DepthStencilView->Release();
        }
    }

#endif
}

#endif