#include "FramebufferD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "D3D12Utils.hpp"
#include "TextureD3D12.hpp"

namespace Nexus::Graphics
{
FramebufferD3D12::FramebufferD3D12(const FramebufferSpecification &spec, GraphicsDeviceD3D12 *device) : Framebuffer(spec), m_Device(device)
{
    if (spec.Width != 0 && spec.Height != 0)
    {
        Recreate();
    }
}

FramebufferD3D12::~FramebufferD3D12()
{
    Flush();
}

void FramebufferD3D12::SetFramebufferSpecification(const FramebufferSpecification &spec)
{
}

Ref<Texture2D> FramebufferD3D12::GetColorTexture(uint32_t index)
{
    return m_ColorAttachments.at(index);
}

Ref<Texture2D> FramebufferD3D12::GetDepthTexture()
{
    return m_DepthAttachment;
}

Ref<Texture2D_D3D12> FramebufferD3D12::GetD3D12ColorTexture(uint32_t index)
{
    return m_ColorAttachments.at(index);
}

Ref<Texture2D_D3D12> FramebufferD3D12::GetD3D12DepthTexture()
{
    return m_DepthAttachment;
}

const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &FramebufferD3D12::GetColorAttachmentCPUHandles()
{
    return m_ColorAttachmentCPUHandles;
}

D3D12_CPU_DESCRIPTOR_HANDLE FramebufferD3D12::GetDepthAttachmentCPUHandle()
{
    return m_DepthAttachmentCPUHandle;
}

void FramebufferD3D12::Recreate()
{
    auto d3d12Device = m_Device->GetDevice();

    CreateAttachments();
    CreateRTVs();
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

void FramebufferD3D12::CreateAttachments()
{
    for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
    {
        const auto &colorAttachmentSpec = m_Specification.ColorAttachmentSpecification.Attachments.at(i);

        if (colorAttachmentSpec.TextureFormat == PixelFormat::None)
        {
            NX_ASSERT(0, "Pixel format cannot be PixelFormat::None for a color attachment");
        }

        Nexus::Graphics::Texture2DSpecification spec;
        spec.Width = m_Specification.Width;
        spec.Height = m_Specification.Height;
        spec.Format = colorAttachmentSpec.TextureFormat;
        spec.Samples = m_Specification.Samples;
        spec.Usage = {TextureUsage::Sampled, TextureUsage::RenderTarget};
        auto texture = std::dynamic_pointer_cast<Texture2D_D3D12>(m_Device->CreateTexture2D(spec));
        m_ColorAttachments.push_back(texture);
    }

    if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
    {
        Nexus::Graphics::Texture2DSpecification spec;
        spec.Width = m_Specification.Width;
        spec.Height = m_Specification.Height;
        spec.Format = m_Specification.DepthAttachmentSpecification.DepthFormat;
        spec.Samples = m_Specification.Samples;
        spec.Usage = {TextureUsage::DepthStencil};
        m_DepthAttachment = std::dynamic_pointer_cast<Texture2D_D3D12>(m_Device->CreateTexture2D(spec));
    }
}

void FramebufferD3D12::CreateRTVs()
{
    auto d3d12Device = m_Device->GetDevice();

    // create descriptor heaps
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

    // retrieve descriptor handles
    auto cpuHandle = m_ColorDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    for (uint32_t i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
    {
        auto texture = m_ColorAttachments.at(i);
        m_ColorAttachmentCPUHandles.push_back(cpuHandle);

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
        rtvDesc.Format = D3D12::GetD3D12PixelFormat(m_Specification.ColorAttachmentSpecification.Attachments[i].TextureFormat, false);

        if (m_Specification.Samples != SampleCount::SampleCount1)
        {
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
        }
        else
        {
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        }

        rtvDesc.Texture2D.MipSlice = 0;
        rtvDesc.Texture2D.PlaneSlice = 0;

        auto resourceHandle = texture->GetD3D12ResourceHandle();
        d3d12Device->CreateRenderTargetView(resourceHandle.Get(), &rtvDesc, cpuHandle);

        auto incrementSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        cpuHandle.ptr += incrementSize;
    }

    m_DepthAttachmentCPUHandle = m_DepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
    {
        auto texture = m_DepthAttachment;

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Format = D3D12::GetD3D12PixelFormat(m_Specification.DepthAttachmentSpecification.DepthFormat, true);

        if (m_Specification.Samples != SampleCount::SampleCount1)
        {
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
        }
        else
        {
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        }

        dsvDesc.Texture2D.MipSlice = 0;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

        auto resourceHandle = texture->GetD3D12ResourceHandle();
        d3d12Device->CreateDepthStencilView(resourceHandle.Get(), &dsvDesc, m_DepthAttachmentCPUHandle);
    }
}
} // namespace Nexus::Graphics

#endif