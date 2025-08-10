#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "Nexus-Core/Graphics/ResourceSet.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"
	#include "SamplerD3D12.hpp"

namespace Nexus::Graphics
{
	class ResourceSetD3D12 : public ResourceSet
	{
	  public:
		ResourceSetD3D12(Ref<Pipeline> pipeline, GraphicsDeviceD3D12 *device);
		virtual void WriteStorageBuffer(StorageBufferView storageBuffer, const std::string &name) override;
		virtual void WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name) override;
		virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) override;
		virtual void WriteStorageImage(StorageImageView view, const std::string &name) override;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSamplerDescriptorHeap();
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetTextureConstantBufferDescriptorHeap();

		const std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> &GetDescriptorTables() const;
		const std::vector<ID3D12DescriptorHeap *>	   &GetDescriptorHeaps() const;

	  private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap	  = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRV_UAV_CBV_DescriptorHeap = nullptr;

		std::vector<ID3D12DescriptorHeap *> m_DescriptorHeapArray = {};

		std::map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> m_SamplerDescriptorHandles	   = {};
		std::map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> m_SRV_UAV_CBV_DescriptorHandles = {};

		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_DescriptorTables = {};
		GraphicsDeviceD3D12 *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif