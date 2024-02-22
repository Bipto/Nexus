#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "Nexus/Graphics/ResourceSet.hpp"
#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"

#include <unordered_map>

namespace Nexus::Graphics
{
    class ResourceSetD3D12 : public ResourceSet
    {
    public:
        ResourceSetD3D12(const ResourceSetSpecification &spec, GraphicsDeviceD3D12 *device);
        virtual void PerformResourceUpdate() override;
        virtual void WriteTexture(Texture *texture, const std::string &name) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, const std::string &name) override;
        virtual void WriteSampler(Sampler *sampler, const std::string &name) override;
        virtual void WriteCombinedImageSampler(Texture *texture, Sampler *sampler, const std::string &name) override;

        ID3D12DescriptorHeap *GetSamplerDescriptorHeap();
        ID3D12DescriptorHeap *GetTextureConstantBufferDescriptorHeap();

        bool HasConstantBufferTextureHeap() const;
        bool HasSamplerHeap() const;
        bool HasConstantBuffers() const;

        const D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerStartHandle() const;
        const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureConstantBufferStartHandle() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_TextureConstantBufferDescriptorHeap = nullptr;

        std::map<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_SamplerCPUDescriptors;
        std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_SamplerGPUDescriptors;

        std::map<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_ConstantBufferCPUDescriptors;
        std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_ConstantBufferGPUDescriptors;

        std::map<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_TextureCPUDescriptors;
        std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_TextureGPUDescriptors;

        GraphicsDeviceD3D12 *m_Device = nullptr;
    };
}

#endif