#include "ResourceSetD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "DeviceBufferD3D12.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"
	#include "SamplerD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "PipelineD3D12.hpp"

namespace Nexus::Graphics
{
	ResourceSetD3D12::ResourceSetD3D12(Ref<Pipeline> pipeline, GraphicsDeviceD3D12 *device) : ResourceSet(pipeline), m_Device(device)
	{
		Ref<PipelineD3D12>						  pipelineD3D12		   = std::dynamic_pointer_cast<PipelineD3D12>(pipeline);
		const Nexus::D3D12::DescriptorHandleInfo &descriptorHandleInfo = pipelineD3D12->GetDescriptorHandleInfo();
		Microsoft::WRL::ComPtr<ID3D12Device9>	  d3dDevice			   = m_Device->GetDevice();
		m_CombinedImageSamplerMap									   = descriptorHandleInfo.CombinedImageSamplerMap;

		// create sampler heap
		if (descriptorHandleInfo.SamplerHeapCount > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
			samplerHeapDesc.NumDescriptors			   = descriptorHandleInfo.SamplerHeapCount;
			samplerHeapDesc.Type					   = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			samplerHeapDesc.Flags					   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			samplerHeapDesc.NodeMask				   = 0;

			d3dDevice->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_SamplerDescriptorHeap));

			for (const auto &[name, offset] : descriptorHandleInfo.SamplerIndexes)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				descriptorHandle.ptr += d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) * offset;
				m_SamplerDescriptorHandles[name] = descriptorHandle;
			}
		}

		// create SRV UAV CBV heap
		if (descriptorHandleInfo.SRV_UAV_CBV_HeapCount > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC srvUavCbvHeapDesc = {};
			srvUavCbvHeapDesc.NumDescriptors			 = descriptorHandleInfo.SRV_UAV_CBV_HeapCount;
			srvUavCbvHeapDesc.Type						 = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvUavCbvHeapDesc.Flags						 = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			srvUavCbvHeapDesc.NodeMask					 = 0;

			d3dDevice->CreateDescriptorHeap(&srvUavCbvHeapDesc, IID_PPV_ARGS(&m_SRV_UAV_CBV_DescriptorHeap));

			for (const auto &[name, offset] : descriptorHandleInfo.NonSamplerIndexes)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_SRV_UAV_CBV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				descriptorHandle.ptr += d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * offset;
				m_SRV_UAV_CBV_DescriptorHandles[name] = descriptorHandle;
			}
		}

		// create descriptor table handles
		for (const D3D12::DescriptorTableInfo &descriptorTableInfo : descriptorHandleInfo.DescriptorTables)
		{
			switch (descriptorTableInfo.Source)
			{
				case D3D12::DescriptorHandleSource::Sampler:
				{
					D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle = m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
					descriptorHandle.ptr +=
						(d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) * descriptorTableInfo.Offset);
					m_DescriptorTables.push_back(descriptorHandle);

					break;
				}
				case D3D12::DescriptorHandleSource::SRV_UAV_CBV:
				{
					D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle = m_SRV_UAV_CBV_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
					descriptorHandle.ptr +=
						(d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * descriptorTableInfo.Offset);
					m_DescriptorTables.push_back(descriptorHandle);
					break;
				}
				default: throw std::runtime_error("Failed to find a valid descriptor handle source");
			}
		}

		if (m_SamplerDescriptorHeap)
		{
			m_DescriptorHeapArray.push_back(m_SamplerDescriptorHeap.Get());
		}

		if (m_SRV_UAV_CBV_DescriptorHeap)
		{
			m_DescriptorHeapArray.push_back(m_SRV_UAV_CBV_DescriptorHeap.Get());
		}
	}

	void ResourceSetD3D12::WriteStorageBuffer(StorageBufferView storageBuffer, const std::string &name)
	{
		auto			   d3d12Device = m_Device->GetDevice();
		if (Ref<DeviceBufferD3D12> d3d12StorageBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(storageBuffer.BufferHandle))
		{
			NX_ASSERT(d3d12StorageBuffer->CheckUsage(Graphics::BufferUsage::Storage), "Attempting to bind a buffer that is not a storage buffer");

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension					 = D3D12_UAV_DIMENSION_BUFFER;

			// storage buffers are accessed in 4 byte chunks
			uavDesc.Format							 = DXGI_FORMAT_R32_TYPELESS;

			size_t bufferOffset = storageBuffer.Offset;
			if (bufferOffset > 0)
			{
				bufferOffset = Utils::AlignTo<size_t>(bufferOffset, 4);
			}
			uavDesc.Buffer.FirstElement = bufferOffset;

			size_t bufferSize = storageBuffer.SizeInBytes;

			// convert into multiples of 4
			if (bufferSize > 0)
			{
				bufferSize /= 4;
			}
			uavDesc.Buffer.NumElements = bufferSize;

			// access as a raw buffer (no stride as it is assumed that the buffer is some multiple of 4 bytes)
			uavDesc.Buffer.StructureByteStride		 = 0;
			uavDesc.Buffer.CounterOffsetInBytes		 = 0;
			uavDesc.Buffer.Flags					 = D3D12_BUFFER_UAV_FLAG_RAW;

			auto						resourceHandle = d3d12StorageBuffer->GetHandle();
			D3D12_CPU_DESCRIPTOR_HANDLE uavHandle	   = m_SRV_UAV_CBV_DescriptorHandles.at(name);
			d3d12Device->CreateUnorderedAccessView(resourceHandle.Get(), nullptr, &uavDesc, uavHandle);

			m_BoundStorageBuffers[name] = storageBuffer;
		}
	}

	void ResourceSetD3D12::WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name)
	{
		auto d3d12Device = m_Device->GetDevice();
		if (Ref<DeviceBufferD3D12> d3d12UniformBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(uniformBuffer.BufferHandle))
		{
			NX_ASSERT(d3d12UniformBuffer->CheckUsage(Graphics::BufferUsage::Uniform), "Attempting to bind a buffer that is not a uniform buffer");

			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = d3d12UniformBuffer->GetHandle()->GetGPUVirtualAddress() + uniformBuffer.Offset;

			// constant buffers are accessed in 256 byte chunks
			size_t bufferViewSize = Utils::AlignTo<size_t>(uniformBuffer.Size, 256);
			desc.SizeInBytes	  = bufferViewSize;

			d3d12Device->CreateConstantBufferView(&desc, m_SRV_UAV_CBV_DescriptorHandles.at(name));

			m_BoundUniformBuffers[name] = uniformBuffer;
		}
	}

	void ResourceSetD3D12::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
	{
		const auto d3d12Device = m_Device->GetDevice();
		// write texture
		{
			Ref<TextureD3D12> d3d12Texture = std::dynamic_pointer_cast<TextureD3D12>(texture);

			D3D12_SHADER_RESOURCE_VIEW_DESC srv = D3D12::CreateTextureSrvView(texture->GetDescription());

			D3D12_CPU_DESCRIPTOR_HANDLE textureHandle  = m_SRV_UAV_CBV_DescriptorHandles.at(name);
			auto						resourceHandle = d3d12Texture->GetHandle();
			d3d12Device->CreateShaderResourceView(resourceHandle.Get(), &srv, textureHandle);
		}

		// write sampler
		{
			// find the relevant sampler for the given texture
			std::string samplerName = m_CombinedImageSamplerMap.at(name);

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

			D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle = m_SamplerDescriptorHandles.at(samplerName);
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
			Ref<TextureD3D12> d3d12Texture = std::dynamic_pointer_cast<TextureD3D12>(view.TextureHandle);

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = D3D12::CreateTextureUavView(view);

			D3D12_CPU_DESCRIPTOR_HANDLE uavHandle	   = m_SRV_UAV_CBV_DescriptorHandles.at(name);
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
		return m_SRV_UAV_CBV_DescriptorHeap;
	}

	const std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> &ResourceSetD3D12::GetDescriptorTables() const
	{
		return m_DescriptorTables;
	}

	const std::vector<ID3D12DescriptorHeap *> &ResourceSetD3D12::GetDescriptorHeaps() const
	{
		return m_DescriptorHeapArray;
	}
}	 // namespace Nexus::Graphics

#endif