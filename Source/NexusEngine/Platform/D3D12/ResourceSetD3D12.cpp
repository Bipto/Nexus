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

        uint32_t textureCount = 0;
        uint32_t textureConstantBufferCount = 0;
        uint32_t cpuSlotLocation = 0;

        for (const auto &binding : spec.Resources)
        {
            if (binding.Type == ResourceType::CombinedImageSampler)
            {
                textureCount++;
                textureConstantBufferCount++;
            }
            else if (binding.Type == ResourceType::UniformBuffer)
            {
                textureConstantBufferCount++;
            }
            else
            {
                throw std::runtime_error("Failed to find a valid resource");
            }
        }

        if (textureCount > 0)
        {
            m_HasSamplerHeap = true;

            D3D12_DESCRIPTOR_HEAP_DESC samplerDesc;
            samplerDesc.NumDescriptors = textureCount;
            samplerDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            samplerDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            samplerDesc.NodeMask = 0;

            d3d12Device->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&m_SamplerDescriptorHeap));

            auto cpuLocation = m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto gpuLocation = m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

            for (const auto &resource : spec.Resources)
            {
                if (resource.Type == ResourceType::CombinedImageSampler)
                {
                    uint32_t binding = ResourceSet::GetLinearDescriptorSlot(resource.Set, resource.Binding);

                    m_SamplerCPUDescriptors[binding] = cpuLocation;
                    m_SamplerGPUDescriptors[binding] = gpuLocation;
                    m_CPUDescriptorSlots[binding] = cpuSlotLocation;

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                    cpuSlotLocation++;
                }
            }
        }

        if (textureConstantBufferCount > 0)
        {
            m_HasConstantBufferTextureHeap = true;

            D3D12_DESCRIPTOR_HEAP_DESC textureConstantBufferDesc;
            textureConstantBufferDesc.NumDescriptors = textureConstantBufferCount;
            textureConstantBufferDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            textureConstantBufferDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            textureConstantBufferDesc.NodeMask = 0;

            d3d12Device->CreateDescriptorHeap(&textureConstantBufferDesc, IID_PPV_ARGS(&m_TextureConstantBufferDescriptorHeap));

            auto cpuLocation = m_TextureConstantBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto gpuLocation = m_TextureConstantBufferDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

            for (const auto &resource : spec.Resources)
            {
                if (resource.Type == ResourceType::CombinedImageSampler)
                {
                    uint32_t binding = ResourceSet::GetLinearDescriptorSlot(resource.Set, resource.Binding);

                    m_TextureCPUDescriptors[binding] = cpuLocation;
                    m_TextureGPUDescriptors[binding] = gpuLocation;
                    m_CPUDescriptorSlots[binding] = cpuSlotLocation;

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    cpuSlotLocation++;
                }
                else if (resource.Type == ResourceType::UniformBuffer)
                {
                    uint32_t binding = ResourceSet::GetLinearDescriptorSlot(resource.Set, resource.Binding);

                    m_ConstantBufferCPUDescriptors[binding] = cpuLocation;
                    m_ConstantBufferGPUDescriptors[binding] = gpuLocation;
                    m_CPUDescriptorSlots[binding] = cpuSlotLocation;

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    cpuSlotLocation++;
                }
            }
        }
    }

    void ResourceSetD3D12::WriteTexture(Texture *texture, uint32_t set, uint32_t binding)
    {
        auto d3d12Device = m_Device->GetDevice();
        TextureD3D12 *d3d12Texture = (TextureD3D12 *)texture;
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(set, binding);

        D3D12_SHADER_RESOURCE_VIEW_DESC srv;
        srv.Format = d3d12Texture->GetFormat();
        srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv.Texture2D.MipLevels = 1;
        srv.Texture2D.MostDetailedMip = 0;
        srv.Texture2D.PlaneSlice = 0;
        srv.Texture2D.ResourceMinLODClamp = 0.0f;

        D3D12_CPU_DESCRIPTOR_HANDLE textureHandle = m_TextureCPUDescriptors.at(slot);

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

        D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle = m_SamplerCPUDescriptors.at(slot);

        d3d12Device->CreateSampler(
            &sd,
            samplerHandle);
    }

    void ResourceSetD3D12::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding)
    {
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(set, binding);

        auto d3d12Device = m_Device->GetDevice();
        UniformBufferD3D12 *d3d12UniformBuffer = (UniformBufferD3D12 *)uniformBuffer;

        D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
        desc.BufferLocation = d3d12UniformBuffer->GetHandle()->GetGPUVirtualAddress();

        // convert the size of the constant buffer to be 256 byte aligned
        desc.SizeInBytes = (d3d12UniformBuffer->GetDescription().Size + 255) & ~255;

        d3d12Device->CreateConstantBufferView(
            &desc,
            m_ConstantBufferCPUDescriptors[slot]);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetSamplerGPUStartHandle()
    {
        return m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetConstantBufferTextureGPUStartHandle()
    {
        return m_TextureConstantBufferDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    ID3D12DescriptorHeap *ResourceSetD3D12::GetSamplerDescriptorHeap()
    {
        return m_SamplerDescriptorHeap.Get();
    }

    ID3D12DescriptorHeap *ResourceSetD3D12::GetTextureConstantBufferDescriptorHeap()
    {
        return m_TextureConstantBufferDescriptorHeap.Get();
    }

    const D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetConstantBufferDescriptor(uint32_t slot)
    {
        return m_ConstantBufferGPUDescriptors.at(slot);
    }

    const D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetTextureDescriptor(uint32_t slot)
    {
        return m_TextureGPUDescriptors.at(slot);
    }

    const D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetSamplerDescriptor(uint32_t slot)
    {
        return m_SamplerGPUDescriptors.at(slot);
    }

    uint32_t ResourceSetD3D12::GetFirstSamplerIndex() const
    {
        uint32_t firstIndex = UINT32_MAX;

        for (const auto &pair : m_SamplerCPUDescriptors)
        {
            if (pair.first < firstIndex)
            {
                firstIndex = pair.first;
            }
        }

        return firstIndex;
    }

    uint32_t ResourceSetD3D12::GetFirstConstantBufferTextureIndex() const
    {
        uint32_t firstIndex = UINT32_MAX;

        for (const auto &pair : m_ConstantBufferCPUDescriptors)
        {
            if (pair.first < firstIndex)
            {
                firstIndex = pair.first;
            }
        }

        for (const auto &pair : m_TextureCPUDescriptors)
        {
            if (pair.first < firstIndex)
            {
                firstIndex = pair.first;
            }
        }

        return firstIndex;
    }

    bool ResourceSetD3D12::HasConstantBufferTextureHeap() const
    {
        return m_HasConstantBufferTextureHeap;
    }

    bool ResourceSetD3D12::HasSamplerHeap() const
    {
        return m_HasSamplerHeap;
    }
}

#endif