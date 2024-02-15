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
        virtual void WriteTexture(Texture *texture, const std::string &name) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, const std::string &name) override;

        D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerGPUStartHandle();
        D3D12_GPU_DESCRIPTOR_HANDLE GetConstantBufferTextureGPUStartHandle();
        D3D12_GPU_DESCRIPTOR_HANDLE GetTextureGPUStartHandle();
        D3D12_GPU_DESCRIPTOR_HANDLE GetConstantBufferGPUStartHandle();

        ID3D12DescriptorHeap *GetSamplerDescriptorHeap();
        ID3D12DescriptorHeap *GetTextureConstantBufferDescriptorHeap();

        bool HasConstantBufferTextureHeap() const;
        bool HasSamplerHeap() const;
        bool HasConstantBuffers() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_TextureConstantBufferDescriptorHeap = nullptr;

        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_SamplerCPUDescriptors;
        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_SamplerGPUDescriptors;
        std::vector<uint32_t> m_SamplerCPUDescriptorSlots;

        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_ConstantBufferCPUDescriptors;
        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_ConstantBufferGPUDescriptors;
        std::vector<uint32_t> m_ConstantBufferCPUDescriptorsSlots;

        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_TextureCPUDescriptors;
        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_TextureGPUDescriptors;
        std::vector<uint32_t> m_TextureCPUDescriptorSlots;

        GraphicsDeviceD3D12 *m_Device = nullptr;
    };
}

#endif