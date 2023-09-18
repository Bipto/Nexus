#if defined(WIN32)

#include "FramebufferDX11.hpp"
#include "Nexus/Logging/Log.hpp"

namespace Nexus::Graphics
{
#if defined(WIN32)
    FramebufferDX11::FramebufferDX11(Microsoft::WRL::ComPtr<ID3D11Device> device, Ref<RenderPass> renderPass)
        : Framebuffer(renderPass)
    {
        m_Device = device;
        Recreate();
    }

    FramebufferDX11::~FramebufferDX11()
    {
        DeleteTextures();
    }

    void FramebufferDX11::Recreate()
    {
        CreateTextures();
    }

    void *FramebufferDX11::GetColorAttachment(int index)
    {
        return m_ColorRenderTargets[index].ShaderResourceView.Get();
    }

    const FramebufferSpecification FramebufferDX11::GetFramebufferSpecification()
    {
        return m_Specification;
    }

    void FramebufferDX11::SetFramebufferSpecification(const FramebufferSpecification &spec)
    {
        m_Specification = spec;
        Recreate();
    }

    void FramebufferDX11::CreateTextures()
    {
        DeleteTextures();

        // create color textures
        {
            m_ColorRenderTargets.clear();

            for (auto colorAttachments : m_Specification.ColorAttachmentSpecification.Attachments)
            {
                FramebufferColorRenderTarget target;

                D3D11_TEXTURE2D_DESC textureDesc;
                ZeroMemory(&textureDesc, sizeof(textureDesc));
                textureDesc.Width = m_Specification.Width;
                textureDesc.Height = m_Specification.Height;
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

                D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                renderTargetViewDesc.Format = textureDesc.Format;
                renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                renderTargetViewDesc.Texture2D.MipSlice = 0;
                hr = m_Device->CreateRenderTargetView(target.Texture.Get(), &renderTargetViewDesc, target.RenderTargetView.GetAddressOf());

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
                hr = m_Device->CreateShaderResourceView(target.Texture.Get(), &shaderResourceViewDesc, target.ShaderResourceView.GetAddressOf());

                if (FAILED(hr))
                {
                    _com_error error(hr);
                    std::string output = std::string("Failed to create framebuffer shader resource view: ") + error.ErrorMessage();
                    NX_ERROR(output);
                }

                m_ColorRenderTargets.push_back(target);
            }
        }

        // create depth target if requested
        {
            FramebufferDepthRenderTarget depthTarget;

            D3D11_TEXTURE2D_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.Width = m_Specification.Width;
            depthStencilDesc.Height = m_Specification.Height;
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

            hr = m_Device->CreateDepthStencilView(depthTarget.Texture.Get(), NULL, depthTarget.DepthStencilView.GetAddressOf());

            if (FAILED(hr))
            {
                _com_error error(hr);
                std::string output = std::string("Failed to create framebuffer depth stencil view: ") + error.ErrorMessage();
                NX_ERROR(output);
            }

            m_DepthTarget = depthTarget;
        }
    }

    void FramebufferDX11::DeleteTextures()
    {
    }

#endif
}

#endif