#include "FramebufferD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Utils.hpp"

#include "D3D12Include.hpp"

namespace Nexus::Graphics
{
    FramebufferD3D12::FramebufferD3D12(const FramebufferSpecification &spec, GraphicsDeviceD3D12 *device)
        : Framebuffer(spec), m_Device(device)
    {
        Recreate();
    }

    FramebufferD3D12::~FramebufferD3D12()
    {
        Flush();
    }

    void FramebufferD3D12::SetFramebufferSpecification(const FramebufferSpecification &spec)
    {
    }

    void FramebufferD3D12::Recreate()
    {
        auto d3d12Device = m_Device->GetDevice();
        m_CurrentColorTextureStates.clear();

        D3D12_RESOURCE_STATES colourResourceState = D3D12_RESOURCE_STATE_COMMON;
        m_CurrentDepthState = D3D12_RESOURCE_STATE_DEPTH_READ;

        uint32_t samples = GetSampleCount(m_Specification.Samples);

        // create color textures
        for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
        {

            Microsoft::WRL::ComPtr<ID3D12Resource2> texture = nullptr;

            D3D12_HEAP_PROPERTIES heapProperties;
            heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProperties.CreationNodeMask = 0;
            heapProperties.VisibleNodeMask = 0;

            auto textureFormat = GetD3D12TextureFormat(m_Specification.ColorAttachmentSpecification.Attachments[i].TextureFormat);

            D3D12_RESOURCE_DESC resourceDesc;
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resourceDesc.Width = m_Specification.Width;
            resourceDesc.Height = m_Specification.Height;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = textureFormat;
            resourceDesc.SampleDesc.Count = samples;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

            d3d12Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, colourResourceState, nullptr, IID_PPV_ARGS(&texture));

            m_ColorTextures.push_back(texture);
            m_CurrentColorTextureStates.push_back(colourResourceState);
        }

        // create depth texture if required
        if (m_Specification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None)
        {
            auto depthFormat = GetD3D12DepthFormat(m_Specification.DepthAttachmentSpecification.DepthFormat);

            D3D12_HEAP_PROPERTIES heapProperties;
            heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProperties.CreationNodeMask = 0;
            heapProperties.VisibleNodeMask = 0;

            D3D12_RESOURCE_DESC resourceDesc;
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resourceDesc.Width = m_Specification.Width;
            resourceDesc.Height = m_Specification.Height;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = depthFormat;
            resourceDesc.SampleDesc.Count = samples;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            d3d12Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, m_CurrentDepthState, nullptr, IID_PPV_ARGS(&m_DepthTexture));
        }

        D3D12_DESCRIPTOR_HEAP_DESC colorDescriptorHeapDesc;
        colorDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        colorDescriptorHeapDesc.NumDescriptors = m_Specification.ColorAttachmentSpecification.Attachments.size();
        colorDescriptorHeapDesc.NodeMask = 0;
        colorDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        d3d12Device->CreateDescriptorHeap(&colorDescriptorHeapDesc, IID_PPV_ARGS(&m_ColorDescriptorHeap));

        D3D12_DESCRIPTOR_HEAP_DESC depthDescriptorHeapDesc;
        depthDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        depthDescriptorHeapDesc.NumDescriptors = 1;
        depthDescriptorHeapDesc.NodeMask = 0;
        depthDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        d3d12Device->CreateDescriptorHeap(&depthDescriptorHeapDesc, IID_PPV_ARGS(&m_DepthDescriptorHeap));

        // retrive descriptor handles
        for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
        {
            auto cpuHandle = m_ColorDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto incrementSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            cpuHandle.ptr += (incrementSize * i);

            m_ColorAttachmentCPUHandles.push_back(cpuHandle);

            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
            rtvDesc.Format = GetD3D12TextureFormat(m_Specification.ColorAttachmentSpecification.Attachments[i].TextureFormat);

            if (samples > 1)
            {
                rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            }

            rtvDesc.Texture2D.MipSlice = 0;
            rtvDesc.Texture2D.PlaneSlice = 0;

            d3d12Device->CreateRenderTargetView(m_ColorTextures[i].Get(), &rtvDesc, m_ColorAttachmentCPUHandles[i]);
        }

        m_DepthAttachmentCPUHandle = m_DepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

        if (m_Specification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None)
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
            dsvDesc.Format = GetD3D12DepthFormat(m_Specification.DepthAttachmentSpecification.DepthFormat);

            if (samples > 1)
            {
                dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            }

            dsvDesc.Texture2D.MipSlice = 0;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

            d3d12Device->CreateDepthStencilView(m_DepthTexture.Get(), &dsvDesc, m_DepthAttachmentCPUHandle);
        }
    }

    void FramebufferD3D12::Flush()
    {
        for (int i = 0; i < BUFFER_COUNT; i++)
        {
            m_Device->SignalAndWait();
        }
    }

    const FramebufferSpecification FramebufferD3D12::GetFramebufferSpecification()
    {
        return m_Specification;
    }

    void *FramebufferD3D12::GetColorAttachment(int index)
    {
        return (void *)m_ColorTextures[index].Get();
    }

    void *FramebufferD3D12::GetDepthAttachment()
    {
        return (void *)m_DepthTexture.Get();
    }

    const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &FramebufferD3D12::GetColorAttachmentHandles()
    {
        return m_ColorAttachmentCPUHandles;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE FramebufferD3D12::GetDepthAttachmentHandle()
    {
        return m_DepthAttachmentCPUHandle;
    }

    const std::vector<Microsoft::WRL::ComPtr<ID3D12Resource2>> FramebufferD3D12::GetColorTextures()
    {
        return m_ColorTextures;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource2> FramebufferD3D12::GetDepthTexture()
    {
        return m_DepthTexture;
    }

    DXGI_FORMAT FramebufferD3D12::GetColorAttachmentFormat(uint32_t index)
    {
        return GetD3D12TextureFormat(m_Specification.ColorAttachmentSpecification.Attachments[index].TextureFormat);
    }

    const std::vector<D3D12_RESOURCE_STATES> &FramebufferD3D12::GetCurrentColorTextureStates() const
    {
        return m_CurrentColorTextureStates;
    }

    const D3D12_RESOURCE_STATES FramebufferD3D12::GetCurrentDepthState() const
    {
        return m_CurrentDepthState;
    }

    void FramebufferD3D12::SetColorTextureState(D3D12_RESOURCE_STATES state, uint32_t index)
    {
        if (index > m_CurrentColorTextureStates.size())
        {
            return;
        }

        m_CurrentColorTextureStates[index] = state;
    }

    void FramebufferD3D12::SetDepthState(D3D12_RESOURCE_STATES state)
    {
        m_CurrentDepthState = state;
    }
}

#endif