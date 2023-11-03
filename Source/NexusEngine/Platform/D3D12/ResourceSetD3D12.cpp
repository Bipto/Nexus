#include "ResourceSetD3D12.hpp"

#include "TextureD3D12.hpp"

namespace Nexus::Graphics
{
    ResourceSetD3D12::ResourceSetD3D12(const ResourceSetSpecification &spec, GraphicsDeviceD3D12 *device)
        : ResourceSet(spec)
    {
        m_Device = device;
        auto d3d12Device = m_Device->GetDevice();

        if (spec.TextureBindings.size() > 0)
        {
            D3D12_DESCRIPTOR_HEAP_DESC samplerDesc;
            samplerDesc.NumDescriptors = spec.TextureBindings.size();
            samplerDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            samplerDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            samplerDesc.NodeMask = 0;

            d3d12Device->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&m_SamplerDescriptorHeap));

            auto cpuLocation = m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto gpuLocation = m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

            for (int i = 0; i < spec.TextureBindings.size(); i++)
            {
                m_SamplerCPUDescriptors.push_back(cpuLocation);
                m_SamplerGPUDescriptors.push_back(gpuLocation);

                cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
            }
        }

        // d3d12 requires textures to be uploaded into CBV_SRV_UAV descriptor
        uint32_t descriptorCount = spec.TextureBindings.size() + spec.UniformResourceBindings.size();
        if (descriptorCount > 0)
        {
            D3D12_DESCRIPTOR_HEAP_DESC constantBufferDesc;
            constantBufferDesc.NumDescriptors = descriptorCount;
            constantBufferDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            constantBufferDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            constantBufferDesc.NodeMask = 0;

            d3d12Device->CreateDescriptorHeap(&constantBufferDesc, IID_PPV_ARGS(&m_ConstantBufferTextureDescriptorHeap));

            auto cpuLocation = m_ConstantBufferTextureDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto gpuLocation = m_ConstantBufferTextureDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

            // retrieve constant buffer handles
            for (int i = 0; i < spec.UniformResourceBindings.size(); i++)
            {
                m_ConstantBufferCPUDescriptors.push_back(cpuLocation);
                m_ConstantBufferGPUDescriptors.push_back(gpuLocation);

                cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }

            // retrieve texture handles
            for (int i = 0; i < spec.TextureBindings.size(); i++)
            {
                m_TextureCPUDescriptors.push_back(cpuLocation);
                m_TextureGPUDescriptors.push_back(gpuLocation);

                cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
        }
    }

    void ResourceSetD3D12::WriteTexture(Texture *texture, uint32_t binding)
    {
        auto d3d12Device = m_Device->GetDevice();
        TextureD3D12 *d3d12Texture = (TextureD3D12 *)texture;

        D3D12_SHADER_RESOURCE_VIEW_DESC srv;
        srv.Format = d3d12Texture->GetFormat();
        srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv.Texture2D.MipLevels = 1;
        srv.Texture2D.MostDetailedMip = 0;
        srv.Texture2D.PlaneSlice = 0;
        srv.Texture2D.ResourceMinLODClamp = 0.0f;

        d3d12Device->CreateShaderResourceView(d3d12Texture->GetD3D12ResourceHandle(),
                                              &srv,
                                              m_TextureCPUDescriptors[binding]);

        D3D12_SAMPLER_DESC sd;
        sd.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sd.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sd.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sd.MipLODBias = 0;
        sd.MaxAnisotropy = 1;
        sd.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        sd.BorderColor[0] = 0.0f;
        sd.BorderColor[1] = 0.0f;
        sd.BorderColor[2] = 0.0f;
        sd.BorderColor[3] = 0.0f;
        sd.MinLOD = 0.0f;
        sd.MaxLOD = D3D12_FLOAT32_MAX;

        d3d12Device->CreateSampler(
            &sd,
            m_SamplerCPUDescriptors[binding]);
    }

    void ResourceSetD3D12::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t binding)
    {
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetSamplerGPUStartHandle()
    {
        return m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    ID3D12DescriptorHeap *ResourceSetD3D12::GetSamplerDescriptorHeap()
    {
        return m_SamplerDescriptorHeap.Get();
    }

    ID3D12DescriptorHeap *ResourceSetD3D12::GetConstantBufferDescriptorHeap()
    {
        return m_ConstantBufferTextureDescriptorHeap.Get();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetConstantBufferGPUStartHandle()
    {
        return m_ConstantBufferTextureDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }
}