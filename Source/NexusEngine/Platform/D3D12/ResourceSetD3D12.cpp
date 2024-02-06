#include "ResourceSetD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "TextureD3D12.hpp"
#include "BufferD3D12.hpp"

namespace Nexus::Graphics
{
    ResourceSetD3D12::ResourceSetD3D12(const ResourceSetSpecification &spec, GraphicsDeviceD3D12 *device)
        : ResourceSet(spec)
    {
        m_Device = device;
        auto d3d12Device = m_Device->GetDevice();

        uint32_t textureCount = spec.Textures.size();
        uint32_t constantBufferCount = spec.UniformBuffers.size();
        uint32_t textureConstantBufferCount = spec.Textures.size() + spec.UniformBuffers.size();

        if (textureCount > 0)
        {
            D3D12_DESCRIPTOR_HEAP_DESC samplerDesc;
            samplerDesc.NumDescriptors = textureCount;
            samplerDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            samplerDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            samplerDesc.NodeMask = 0;

            d3d12Device->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&m_SamplerDescriptorHeap));

            auto cpuLocation = m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto gpuLocation = m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

            for (int i = 0; i < textureCount; i++)
            {
                m_SamplerCPUDescriptors.push_back(cpuLocation);
                m_SamplerGPUDescriptors.push_back(gpuLocation);

                cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
            }
        }

        if (textureConstantBufferCount > 0)
        {
            D3D12_DESCRIPTOR_HEAP_DESC constantBufferTextureDesc;
            constantBufferTextureDesc.NumDescriptors = textureConstantBufferCount;
            constantBufferTextureDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            constantBufferTextureDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            constantBufferTextureDesc.NodeMask = 0;

            d3d12Device->CreateDescriptorHeap(&constantBufferTextureDesc, IID_PPV_ARGS(&m_TextureConstantBufferDescriptorHeap));

            auto cpuLocation = m_TextureConstantBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto gpuLocation = m_TextureConstantBufferDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

            // create texture handles
            {
                for (int i = 0; i < spec.Textures.size(); i++)
                {
                    m_TextureCPUDescriptors.push_back(cpuLocation);
                    m_TextureGPUDescriptors.push_back(gpuLocation);

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
            }

            // create constant buffer handles
            {
                for (int i = 0; i < spec.UniformBuffers.size(); i++)
                {
                    m_ConstantBufferCPUDescriptors.push_back(cpuLocation);
                    m_ConstantBufferGPUDescriptors.push_back(gpuLocation);

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
            }
        }
    }

    void ResourceSetD3D12::WriteTexture(Texture *texture, uint32_t set, uint32_t binding)
    {
        const auto d3d12Device = m_Device->GetDevice();
        TextureD3D12 *d3d12Texture = (TextureD3D12 *)texture;
        // uint32_t slot = ResourceSet::GetTextureSlot(set, binding);

        const uint32_t index = GetLinearDescriptorSlot(set, binding);
        const uint32_t newBinding = m_TextureBindings.at(index);

        D3D12_SHADER_RESOURCE_VIEW_DESC srv;
        srv.Format = d3d12Texture->GetFormat();
        srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv.Texture2D.MipLevels = 1;
        srv.Texture2D.MostDetailedMip = 0;
        srv.Texture2D.PlaneSlice = 0;
        srv.Texture2D.ResourceMinLODClamp = 0.0f;

        D3D12_CPU_DESCRIPTOR_HANDLE textureHandle = m_TextureCPUDescriptors.at(newBinding);

        d3d12Device->CreateShaderResourceView(d3d12Texture->GetD3D12ResourceHandle(),
                                              &srv,
                                              textureHandle);

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

        D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle = m_SamplerCPUDescriptors.at(newBinding);

        d3d12Device->CreateSampler(
            &sd,
            samplerHandle);
    }

    void ResourceSetD3D12::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding)
    {
        /* uint32_t slot = ResourceSet::GetUniformBufferSlot(set, binding);
        uint32_t newBinding = m_UniformBufferBindings.at(slot); */

        const uint32_t index = GetLinearDescriptorSlot(set, binding);
        const uint32_t newBinding = m_UniformBufferBindings.at(index);

        auto d3d12Device = m_Device->GetDevice();
        UniformBufferD3D12 *d3d12UniformBuffer = (UniformBufferD3D12 *)uniformBuffer;

        D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
        desc.BufferLocation = d3d12UniformBuffer->GetHandle()->GetGPUVirtualAddress();

        // convert the size of the constant buffer to be 256 byte aligned
        desc.SizeInBytes = (d3d12UniformBuffer->GetDescription().Size + 255) & ~255;

        d3d12Device->CreateConstantBufferView(
            &desc,
            m_ConstantBufferCPUDescriptors[newBinding]);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetSamplerGPUStartHandle()
    {
        return m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetConstantBufferTextureGPUStartHandle()
    {
        return m_TextureConstantBufferDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetTextureGPUStartHandle()
    {
        return m_TextureGPUDescriptors[0];
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetConstantBufferGPUStartHandle()
    {
        return m_ConstantBufferGPUDescriptors[0];
    }

    ID3D12DescriptorHeap *ResourceSetD3D12::GetSamplerDescriptorHeap()
    {
        return m_SamplerDescriptorHeap.Get();
    }

    ID3D12DescriptorHeap *ResourceSetD3D12::GetTextureConstantBufferDescriptorHeap()
    {
        return m_TextureConstantBufferDescriptorHeap.Get();
    }

    bool ResourceSetD3D12::HasConstantBufferTextureHeap() const
    {
        return m_TextureConstantBufferDescriptorHeap;
    }

    bool ResourceSetD3D12::HasSamplerHeap() const
    {
        return m_SamplerDescriptorHeap;
    }

    bool ResourceSetD3D12::HasConstantBuffers() const
    {
        return m_Specification.UniformBuffers.size() > 0;
    }
}

#endif