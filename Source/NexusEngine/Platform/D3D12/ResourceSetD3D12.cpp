#include "ResourceSetD3D12.hpp"

#include "TextureD3D12.hpp"
#include "BufferD3D12.hpp"

namespace Nexus::Graphics
{
    ResourceSetD3D12::ResourceSetD3D12(const ResourceSetSpecification &spec, GraphicsDeviceD3D12 *device)
        : ResourceSet(spec)
    {
        m_Device = device;
        auto d3d12Device = m_Device->GetDevice();
        uint32_t textureCount = spec.TextureBindings.size();
        uint32_t uniformBufferCount = spec.UniformResourceBindings.size();
        uint32_t textureConstantBufferCount = spec.TextureBindings.size() + spec.UniformResourceBindings.size();

        if (textureCount > 0)
        {
            // create sampler descriptors
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

            // create texture descriptors
            {
                /* D3D12_DESCRIPTOR_HEAP_DESC textureDescriptorDesc;
                textureDescriptorDesc.NumDescriptors = textureCount;
                textureDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                textureDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                textureDescriptorDesc.NodeMask = 0;

                d3d12Device->CreateDescriptorHeap(&textureDescriptorDesc, IID_PPV_ARGS(&m_TextureDescriptorHeap));

                auto cpuLocation = m_TextureDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                auto gpuLocation = m_TextureDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

                // retrieve texture handles
                for (int i = 0; i < spec.TextureBindings.size(); i++)
                {
                    m_TextureCPUDescriptors.push_back(cpuLocation);
                    m_TextureGPUDescriptors.push_back(gpuLocation);

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                } */
            }
        }

        if (textureConstantBufferCount > 0)
        {
            D3D12_DESCRIPTOR_HEAP_DESC textureDescriptorDesc;
            textureDescriptorDesc.NumDescriptors = textureConstantBufferCount;
            textureDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            textureDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            textureDescriptorDesc.NodeMask = 0;

            d3d12Device->CreateDescriptorHeap(&textureDescriptorDesc, IID_PPV_ARGS(&m_TextureConstantBufferDescriptorHeap));

            auto cpuLocation = m_TextureConstantBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto gpuLocation = m_TextureConstantBufferDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

            // textures
            {
                // retrieve texture handles
                for (int i = 0; i < spec.TextureBindings.size(); i++)
                {
                    m_TextureCPUDescriptors.push_back(cpuLocation);
                    m_TextureGPUDescriptors.push_back(gpuLocation);

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
            }

            // constant buffers
            {
                for (int i = 0; i < spec.UniformResourceBindings.size(); i++)
                {
                    m_ConstantBufferCPUDescriptors.push_back(cpuLocation);
                    m_ConstantBufferGPUDescriptors.push_back(gpuLocation);

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
            }
        }

        /* if (uniformBufferCount > 0)
        {
            D3D12_DESCRIPTOR_HEAP_DESC constantBufferDescriptorDesc;
            constantBufferDescriptorDesc.NumDescriptors = uniformBufferCount;
            constantBufferDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            constantBufferDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            constantBufferDescriptorDesc.NodeMask = 0;

            d3d12Device->CreateDescriptorHeap(&constantBufferDescriptorDesc, IID_PPV_ARGS(&m_ConstantBufferDescriptorHeap));

            auto cpuLocation = m_ConstantBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            auto gpuLocation = m_ConstantBufferDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

            // retrieve constant buffer handles
            for (int i = 0; i < spec.UniformResourceBindings.size(); i++)
            {
                m_ConstantBufferCPUDescriptors.push_back(cpuLocation);
                m_ConstantBufferGPUDescriptors.push_back(gpuLocation);

                cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
        } */
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
        auto d3d12Device = m_Device->GetDevice();
        UniformBufferD3D12 *d3d12UniformBuffer = (UniformBufferD3D12 *)uniformBuffer;

        D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
        desc.BufferLocation = d3d12UniformBuffer->GetHandle()->GetGPUVirtualAddress();

        // convert the size of the constant buffer to be 256 byte aligned
        desc.SizeInBytes = (d3d12UniformBuffer->GetDescription().Size + 255) & ~255;

        d3d12Device->CreateConstantBufferView(
            &desc,
            m_ConstantBufferCPUDescriptors[binding]);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetSamplerGPUStartHandle()
    {
        return m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetTextureGPUStartHandle()
    {
        return m_TextureGPUDescriptors[0];
    }

    ID3D12DescriptorHeap *ResourceSetD3D12::GetSamplerDescriptorHeap()
    {
        return m_SamplerDescriptorHeap.Get();
    }

    ID3D12DescriptorHeap *ResourceSetD3D12::GetTextureConstantBufferDescriptorHeap()
    {
        return m_TextureConstantBufferDescriptorHeap.Get();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetConstantBufferGPUStartHandle()
    {
        return m_ConstantBufferGPUDescriptors[0];
    }
}