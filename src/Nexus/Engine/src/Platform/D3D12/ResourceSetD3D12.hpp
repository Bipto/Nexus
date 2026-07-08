#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "Nexus-Core/nxpch.hpp"
#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"
#include "RHI/ResourceSet.hpp"
#include "SamplerD3D12.hpp"

namespace Nexus::Graphics
{
    class ResourceSetD3D12 final : public IResourceSet
    {
      public:
        ResourceSetD3D12(PipelineHandle pipeline, GraphicsDeviceD3D12 *device);
        void Flush() final;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSamplerDescriptorHeap();
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetTextureConstantBufferDescriptorHeap();

        const std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> &GetDescriptorTables() const;
        const std::vector<ID3D12DescriptorHeap *> &GetDescriptorHeaps() const;

        void SetPushConstants(
            const std::string &name, const void *data, size_t offset, size_t size, bool isGraphics,
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList
        );

        void Bind(
            bool isGraphics, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
            const std::map<std::string, std::vector<uint32_t>> &dynamicOffsets
        );

      private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRV_UAV_CBV_DescriptorHeap = nullptr;

        std::vector<ID3D12DescriptorHeap *> m_DescriptorHeapArray = {};

        std::map<std::string, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>> m_SamplerDescriptorHandles = {};
        std::map<std::string, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>> m_SRV_UAV_CBV_DescriptorHandles = {};

        D3D12::DescriptorHandleInfo m_DescriptorHandleInfo = {};
        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_DescriptorTables = {};

        GraphicsDeviceD3D12 *m_Device = nullptr;

        D3D12::RootSignatureBindingLocations m_RootSignatureBindingLocations = {};

        std::map<std::string, uint32_t> m_PushConstants = {};
    };
} // namespace Nexus::Graphics

#endif