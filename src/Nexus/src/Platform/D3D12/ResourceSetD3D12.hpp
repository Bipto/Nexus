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
		ResourceSetD3D12(const ResourceSetSpecification &spec, GraphicsDeviceD3D12 *device);
		virtual void WriteStorageBuffer(StorageBufferView storageBuffer, const std::string &name) override;
		virtual void WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name) override;
		virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) override;
		virtual void WriteStorageImage(StorageImageView view, const std::string &name) override;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSamplerDescriptorHeap();
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetTextureConstantBufferDescriptorHeap();

		bool HasConstantBufferTextureHeap() const;
		bool HasSamplerHeap() const;
		bool HasConstantBuffers() const;

		const D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerStartHandle() const;
		const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureConstantBufferStartHandle() const;
		const D3D12_GPU_DESCRIPTOR_HANDLE GetStorageBufferStartHandle() const;

		void EnumerateStorageBuffers(std::function<void(uint32_t, StorageBufferView)>);

	  private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap			   = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_TextureConstantBufferDescriptorHeap = nullptr;

		std::map<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_SamplerCPUDescriptors;
		std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_SamplerGPUDescriptors;

		std::map<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_ConstantBufferCPUDescriptors;
		std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_ConstantBufferGPUDescriptors;

		std::map<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_TextureCPUDescriptors;
		std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_TextureGPUDescriptors;

		std::map<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_StorageImageCPUDescriptors;
		std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_StorageImageGPUDescriptors;

		std::map<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_StorageBufferCPUDescriptors;
		std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_StorageBufferGPUDescriptors;

		D3D12_GPU_DESCRIPTOR_HANDLE m_StorageBufferDescriptorStartHandle;

		GraphicsDeviceD3D12 *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif