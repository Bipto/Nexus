#include "ResourceSetD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "TextureD3D12.hpp"
#include "BufferD3D12.hpp"
#include "SamplerD3D12.hpp"

#include "Nexus/Utils/Utils.hpp"

namespace Nexus::Graphics
{
    ResourceSetD3D12::ResourceSetD3D12(const ResourceSetSpecification &spec, GraphicsDeviceD3D12 *device)
        : ResourceSet(spec)
    {
        m_Device = device;
        auto d3d12Device = m_Device->GetDevice();

        uint32_t textureCount = spec.SampledImages.size();
        uint32_t constantBufferCount = spec.UniformBuffers.size();
        uint32_t textureConstantBufferCount = spec.SampledImages.size() + spec.UniformBuffers.size();

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
                const auto &textureInfo = spec.SampledImages.at(i);
                const uint32_t slot = ResourceSet::GetLinearDescriptorSlot(textureInfo.Set, textureInfo.Binding);

                m_SamplerCPUDescriptors[slot] = cpuLocation;
                m_SamplerGPUDescriptors[slot] = gpuLocation;

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
                for (int i = 0; i < spec.SampledImages.size(); i++)
                {
                    const auto &textureInfo = spec.SampledImages.at(i);
                    const uint32_t slot = ResourceSet::GetLinearDescriptorSlot(textureInfo.Set, textureInfo.Binding);

                    m_TextureCPUDescriptors[slot] = cpuLocation;
                    m_TextureGPUDescriptors[slot] = gpuLocation;

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
            }

            // create constant buffer handles
            {
                for (int i = 0; i < spec.UniformBuffers.size(); i++)
                {
                    const auto &constantBufferInfo = spec.UniformBuffers.at(i);
                    const uint32_t slot = ResourceSet::GetLinearDescriptorSlot(constantBufferInfo.Set, constantBufferInfo.Binding);

                    m_ConstantBufferCPUDescriptors[slot] = cpuLocation;
                    m_ConstantBufferGPUDescriptors[slot] = gpuLocation;

                    cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
            }
        }
    }

    void ResourceSetD3D12::WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, const std::string &name)
    {
        const BindingInfo &info = m_UniformBufferBindingInfos.at(name);
        const uint32_t index = GetLinearDescriptorSlot(info.Set, info.Binding);
        auto d3d12Device = m_Device->GetDevice();
        Ref<UniformBufferD3D12> d3d12UniformBuffer = std::dynamic_pointer_cast<UniformBufferD3D12>(uniformBuffer);

        D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
        desc.BufferLocation = d3d12UniformBuffer->GetHandle()->GetGPUVirtualAddress();

        // convert the size of the constant buffer to be 256 byte aligned
        desc.SizeInBytes = (d3d12UniformBuffer->GetDescription().Size + 255) & ~255;

        d3d12Device->CreateConstantBufferView(
            &desc,
            m_ConstantBufferCPUDescriptors.at(index));
    }

    void ResourceSetD3D12::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
    {
        const auto d3d12Device = m_Device->GetDevice();
        // write texture
        {
            Ref<TextureD3D12> d3d12Texture = std::dynamic_pointer_cast<TextureD3D12>(texture);

            const BindingInfo &info = m_TextureBindingInfos.at(name);
            const uint32_t index = GetLinearDescriptorSlot(info.Set, info.Binding);

            D3D12_SHADER_RESOURCE_VIEW_DESC srv;
            srv.Format = d3d12Texture->GetFormat();
            srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srv.Texture2D.MipLevels = 1;
            srv.Texture2D.MostDetailedMip = 0;
            srv.Texture2D.PlaneSlice = 0;
            srv.Texture2D.ResourceMinLODClamp = 0.0f;

            D3D12_CPU_DESCRIPTOR_HANDLE textureHandle = m_TextureCPUDescriptors.at(index);
            auto resourceHandle = d3d12Texture->GetD3D12ResourceHandle();
            d3d12Device->CreateShaderResourceView(resourceHandle.Get(),
                                                  &srv,
                                                  textureHandle);
        }

        // write sampler
        {
            const BindingInfo &info = m_SamplerBindingInfos.at(name);
            const uint32_t index = GetLinearDescriptorSlot(info.Set, info.Binding);
            auto d3d12Device = m_Device->GetDevice();
            Ref<SamplerD3D12> d3d12Sampler = std::dynamic_pointer_cast<SamplerD3D12>(sampler);

            const auto &spec = d3d12Sampler->GetSamplerSpecification();

            const glm::vec4 color = Nexus::Utils::ColorFromBorderColor(spec.BorderColor);

            D3D12_SAMPLER_DESC sd;
            sd.Filter = d3d12Sampler->GetFilter();
            sd.AddressU = d3d12Sampler->GetAddressModeU();
            sd.AddressV = d3d12Sampler->GetAddressModeV();
            sd.AddressW = d3d12Sampler->GetAddressModeW();
            sd.MipLODBias = spec.LODBias;
            sd.MaxAnisotropy = spec.MaximumAnisotropy;
            sd.ComparisonFunc = d3d12Sampler->GetComparisonFunc();
            sd.BorderColor[0] = color.r;
            sd.BorderColor[1] = color.g;
            sd.BorderColor[2] = color.b;
            sd.BorderColor[3] = color.a;
            sd.MinLOD = spec.MinimumLOD;
            sd.MaxLOD = spec.MaximumLOD;

            D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle = m_SamplerCPUDescriptors.at(index);
            d3d12Device->CreateSampler(
                &sd,
                samplerHandle);
        }
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

    const D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetSamplerStartHandle() const
    {
        return m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    const D3D12_GPU_DESCRIPTOR_HANDLE ResourceSetD3D12::GetTextureConstantBufferStartHandle() const
    {
        return m_TextureConstantBufferDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }
}

#endif