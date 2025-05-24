#include "ResourceSetD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "DeviceBufferD3D12.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"
	#include "SamplerD3D12.hpp"
	#include "TextureD3D12.hpp"

namespace Nexus::Graphics
{
	ResourceSetD3D12::ResourceSetD3D12(const ResourceSetSpecification &spec, GraphicsDeviceD3D12 *device) : ResourceSet(spec)
	{
		m_Device		 = device;
		auto d3d12Device = m_Device->GetDevice();

		uint32_t samplerCount		 = spec.SampledImages.size();
		uint32_t constantBufferCount = spec.UniformBuffers.size();
		uint32_t storageImageCount	 = spec.StorageImages.size();
		uint32_t viewCount			 = spec.SampledImages.size() + spec.UniformBuffers.size() + spec.StorageImages.size();

		if (samplerCount > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC samplerDesc;
			samplerDesc.NumDescriptors = samplerCount;
			samplerDesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			samplerDesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			samplerDesc.NodeMask	   = 0;

			d3d12Device->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&m_SamplerDescriptorHeap));

			auto cpuLocation = m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			auto gpuLocation = m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

			for (size_t i = 0; i < samplerCount; i++)
			{
				const auto	  &textureInfo = spec.SampledImages.at(i);
				const uint32_t slot		   = ResourceSet::GetLinearDescriptorSlot(textureInfo.Set, textureInfo.Binding);

				m_SamplerCPUDescriptors[slot] = cpuLocation;
				m_SamplerGPUDescriptors[slot] = gpuLocation;

				cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
				gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			}
		}

		if (viewCount > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC constantBufferTextureDesc;
			constantBufferTextureDesc.NumDescriptors = viewCount;
			constantBufferTextureDesc.Type			 = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			constantBufferTextureDesc.Flags			 = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			constantBufferTextureDesc.NodeMask		 = 0;

			d3d12Device->CreateDescriptorHeap(&constantBufferTextureDesc, IID_PPV_ARGS(&m_TextureConstantBufferDescriptorHeap));

			auto cpuLocation = m_TextureConstantBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			auto gpuLocation = m_TextureConstantBufferDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

			// create texture handles
			{
				for (size_t i = 0; i < spec.SampledImages.size(); i++)
				{
					const auto	  &textureInfo = spec.SampledImages.at(i);
					const uint32_t slot		   = ResourceSet::GetLinearDescriptorSlot(textureInfo.Set, textureInfo.Binding);

					m_TextureCPUDescriptors[slot] = cpuLocation;
					m_TextureGPUDescriptors[slot] = gpuLocation;

					cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}

			// create constant buffer handles
			{
				for (size_t i = 0; i < spec.UniformBuffers.size(); i++)
				{
					const auto	  &constantBufferInfo = spec.UniformBuffers.at(i);
					const uint32_t slot				  = ResourceSet::GetLinearDescriptorSlot(constantBufferInfo.Set, constantBufferInfo.Binding);

					m_ConstantBufferCPUDescriptors[slot] = cpuLocation;
					m_ConstantBufferGPUDescriptors[slot] = gpuLocation;

					cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}

			// create storage image handles
			{
				for (size_t i = 0; i < spec.StorageImages.size(); i++)
				{
					const auto	  &storageImageInfo = spec.StorageImages.at(i);
					const uint32_t slot				= ResourceSet::GetLinearDescriptorSlot(storageImageInfo.Set, storageImageInfo.Binding);

					m_StorageImageCPUDescriptors[slot] = cpuLocation;
					m_StorageImageGPUDescriptors[slot] = gpuLocation;

					cpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					gpuLocation.ptr += d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}
		}
	}

	void ResourceSetD3D12::WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name)
	{
		const BindingInfo &info		   = m_UniformBufferBindingInfos.at(name);
		const uint32_t	   index	   = GetLinearDescriptorSlot(info.Set, info.Binding);
		auto			   d3d12Device = m_Device->GetDevice();
		if (Ref<DeviceBufferD3D12> d3d12UniformBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(uniformBuffer.BufferHandle))
		{
			NX_ASSERT(d3d12UniformBuffer->CheckUsage(Graphics::BufferUsage::Uniform), "Attempting to bind a buffer that is not a uniform buffer");

			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = d3d12UniformBuffer->GetHandle()->GetGPUVirtualAddress() + uniformBuffer.Offset;
			desc.SizeInBytes	= d3d12UniformBuffer->GetBufferSizeInBytes() - uniformBuffer.Offset;

			d3d12Device->CreateConstantBufferView(&desc, m_ConstantBufferCPUDescriptors.at(index));

			m_BoundUniformBuffers[name] = uniformBuffer;
		}
	}

	void ResourceSetD3D12::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
	{
		const auto d3d12Device = m_Device->GetDevice();
		// write texture
		{
			Ref<TextureD3D12> d3d12Texture = std::dynamic_pointer_cast<TextureD3D12>(texture);

			const BindingInfo &info	 = m_CombinedImageSamplerBindingInfos.at(name);
			const uint32_t	   index = GetLinearDescriptorSlot(info.Set, info.Binding);

			D3D12_SHADER_RESOURCE_VIEW_DESC srv = D3D12::CreateTextureSrvView(texture->GetSpecification());

			D3D12_CPU_DESCRIPTOR_HANDLE textureHandle  = m_TextureCPUDescriptors.at(index);
			auto						resourceHandle = d3d12Texture->GetHandle();
			d3d12Device->CreateShaderResourceView(resourceHandle.Get(), &srv, textureHandle);
		}

		// write sampler
		{
			const BindingInfo &info			= m_CombinedImageSamplerBindingInfos.at(name);
			const uint32_t	   index		= GetLinearDescriptorSlot(info.Set, info.Binding);
			auto			   d3d12Device	= m_Device->GetDevice();
			Ref<SamplerD3D12>  d3d12Sampler = std::dynamic_pointer_cast<SamplerD3D12>(sampler);

			const auto &spec = d3d12Sampler->GetSamplerSpecification();

			const glm::vec4 color = Nexus::Utils::ColorFromBorderColor(spec.TextureBorderColor);

			D3D12_SAMPLER_DESC sd;
			sd.Filter		  = d3d12Sampler->GetFilter();
			sd.AddressU		  = d3d12Sampler->GetAddressModeU();
			sd.AddressV		  = d3d12Sampler->GetAddressModeV();
			sd.AddressW		  = d3d12Sampler->GetAddressModeW();
			sd.MipLODBias	  = spec.LODBias;
			sd.MaxAnisotropy  = spec.MaximumAnisotropy;
			sd.ComparisonFunc = d3d12Sampler->GetComparisonFunc();
			sd.BorderColor[0] = color.r;
			sd.BorderColor[1] = color.g;
			sd.BorderColor[2] = color.b;
			sd.BorderColor[3] = color.a;
			sd.MinLOD		  = spec.MinimumLOD;
			sd.MaxLOD		  = spec.MaximumLOD;

			D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle = m_SamplerCPUDescriptors.at(index);
			d3d12Device->CreateSampler(&sd, samplerHandle);
		}

		CombinedImageSampler ciSampler {};
		ciSampler.ImageTexture			   = texture;
		ciSampler.ImageSampler			   = sampler;
		m_BoundCombinedImageSamplers[name] = ciSampler;
	}

	void ResourceSetD3D12::WriteStorageImage(StorageImageView view, const std::string &name)
	{
		const auto d3d12Device = m_Device->GetDevice();

		// write storage image
		{
			const BindingInfo &info			= m_StorageImageBindingInfos.at(name);
			Ref<TextureD3D12>  d3d12Texture = std::dynamic_pointer_cast<TextureD3D12>(view.TextureHandle);
			const uint32_t	   index		= GetLinearDescriptorSlot(info.Set, info.Binding);

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = D3D12::CreateTextureUavView(view);

			D3D12_CPU_DESCRIPTOR_HANDLE uavHandle	   = m_StorageImageCPUDescriptors.at(index);
			auto						resourceHandle = d3d12Texture->GetHandle();
			d3d12Device->CreateUnorderedAccessView(resourceHandle.Get(), nullptr, &uavDesc, uavHandle);
		}

		m_BoundStorageImages[name] = view;
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ResourceSetD3D12::GetSamplerDescriptorHeap()
	{
		return m_SamplerDescriptorHeap;
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ResourceSetD3D12::GetTextureConstantBufferDescriptorHeap()
	{
		return m_TextureConstantBufferDescriptorHeap;
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
}	 // namespace Nexus::Graphics

#endif