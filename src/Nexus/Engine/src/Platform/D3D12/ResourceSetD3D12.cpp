#include "ResourceSetD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "DeviceBufferD3D12.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"
	#include "PipelineD3D12.hpp"
	#include "SamplerD3D12.hpp"
	#include "TexelBufferD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TextureViewD3D12.hpp"

namespace Nexus::Graphics
{
	ResourceSetD3D12::ResourceSetD3D12(Ref<Pipeline> pipeline, GraphicsDeviceD3D12 *device) : IResourceSet(pipeline), m_Device(device)
	{
		Ref<PipelineD3D12> pipelineD3D12				= std::dynamic_pointer_cast<PipelineD3D12>(pipeline);
		m_RootSignatureBindingLocations					= pipelineD3D12->GetRootSignatureBindingLocations();
		m_DescriptorHandleInfo							= pipelineD3D12->GetDescriptorHandleInfo();
		Microsoft::WRL::ComPtr<ID3D12Device9> d3dDevice = m_Device->GetD3D12Device();

		// create sampler heap
		if (m_DescriptorHandleInfo.SamplerHeapCount > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
			samplerHeapDesc.NumDescriptors			   = m_DescriptorHandleInfo.SamplerHeapCount;
			samplerHeapDesc.Type					   = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			samplerHeapDesc.Flags					   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			samplerHeapDesc.NodeMask				   = 0;

			d3dDevice->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_SamplerDescriptorHeap));

			for (const auto &[name, offsets] : m_DescriptorHandleInfo.SamplerIndexes)
			{
				for (size_t i = 0; i < offsets.size(); i++)
				{
					uint32_t					offset			 = offsets.at(i);
					D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
					descriptorHandle.ptr += d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) * offset;
					m_SamplerDescriptorHandles[name].push_back(descriptorHandle);
				}
			}
		}

		// create SRV UAV CBV heap
		if (m_DescriptorHandleInfo.SRV_UAV_CBV_HeapCount > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC srvUavCbvHeapDesc = {};
			srvUavCbvHeapDesc.NumDescriptors			 = m_DescriptorHandleInfo.SRV_UAV_CBV_HeapCount;
			srvUavCbvHeapDesc.Type						 = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvUavCbvHeapDesc.Flags						 = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			srvUavCbvHeapDesc.NodeMask					 = 0;

			d3dDevice->CreateDescriptorHeap(&srvUavCbvHeapDesc, IID_PPV_ARGS(&m_SRV_UAV_CBV_DescriptorHeap));

			for (const auto &[name, offsets] : m_DescriptorHandleInfo.NonSamplerIndexes)
			{
				for (size_t i = 0; i < offsets.size(); i++)
				{
					uint32_t					offset			 = offsets.at(i);
					D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_SRV_UAV_CBV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
					descriptorHandle.ptr += d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * offset;
					m_SRV_UAV_CBV_DescriptorHandles[name].push_back(descriptorHandle);
				}
			}
		}

		// create descriptor table handles
		for (const D3D12::DescriptorTableInfo &descriptorTableInfo : m_DescriptorHandleInfo.DescriptorTables)
		{
			switch (descriptorTableInfo.Source)
			{
				case D3D12::DescriptorHandleSource::ISampler:
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

		const ResourceSetDescription &resourceSetDesc = pipeline->GetResourceSetDescription();
		for (const auto &descriptor : resourceSetDesc.Descriptors)
		{
			if (descriptor.Type == ResourceDescriptorType::PushConstants)
			{
				const std::map<std::string, ShaderResource> &shaderResources = pipeline->GetRequiredShaderResources();
				if (shaderResources.contains(descriptor.Name))
				{
					const ShaderResource &resource	 = shaderResources.at(descriptor.Name);
					m_PushConstants[descriptor.Name] = resource.Binding;
				}
			}
		}
	}

	static void CreateConstantBufferView(Microsoft::WRL::ComPtr<ID3D12Device9> device,
										 Ref<DeviceBufferD3D12>				   buffer,
										 size_t								   offset,
										 size_t								   sizeInBytes,
										 D3D12_CPU_DESCRIPTOR_HANDLE		   cpuHandle)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource2> bufferHandle = buffer->GetHandle();

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation					 = bufferHandle->GetGPUVirtualAddress() + offset;

		// constant buffers are accessed in 256 byte chunks
		size_t bufferViewSize = Utils::AlignUp<size_t>(sizeInBytes, 256);
		desc.SizeInBytes	  = bufferViewSize;

		device->CreateConstantBufferView(&desc, cpuHandle);
	}

	static void CreateSrvBufferView(Microsoft::WRL::ComPtr<ID3D12Device9>	device,
									Microsoft::WRL::ComPtr<ID3D12Resource2> buffer,
									size_t									offset,
									size_t									sizeInBytes,
									size_t									strideInBytes,
									D3D12_CPU_DESCRIPTOR_HANDLE				cpuHandle,
									bool									byteAddress,
									DXGI_FORMAT								format)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_BUFFER;

		if (byteAddress)
		{
			srvDesc.Format					   = format;
			srvDesc.Buffer.FirstElement		   = Utils::AlignDown<size_t>(offset, 4);
			srvDesc.Buffer.NumElements		   = sizeInBytes / 4;
			srvDesc.Buffer.StructureByteStride = 0;
			srvDesc.Buffer.Flags			   = D3D12_BUFFER_SRV_FLAG_RAW;
		}
		else
		{
			size_t stride					   = strideInBytes;
			srvDesc.Format					   = format;
			srvDesc.Buffer.FirstElement		   = (offset / stride);
			srvDesc.Buffer.NumElements		   = (sizeInBytes / stride);
			srvDesc.Buffer.StructureByteStride = stride;
			srvDesc.Buffer.Flags			   = D3D12_BUFFER_SRV_FLAG_NONE;
		}

		device->CreateShaderResourceView(buffer.Get(), &srvDesc, cpuHandle);
	}

	static void CreateUavBufferView(Microsoft::WRL::ComPtr<ID3D12Device9>	device,
									Microsoft::WRL::ComPtr<ID3D12Resource2> buffer,
									size_t									offset,
									size_t									sizeInBytes,
									size_t									strideInBytes,
									D3D12_CPU_DESCRIPTOR_HANDLE				cpuHandle,
									bool									byteAddress,
									DXGI_FORMAT								format)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension					 = D3D12_UAV_DIMENSION_BUFFER;

		if (byteAddress)
		{
			uavDesc.Format					   = DXGI_FORMAT_R32_TYPELESS;
			uavDesc.Buffer.FirstElement		   = Utils::AlignDown<size_t>(offset, 4);
			uavDesc.Buffer.NumElements		   = sizeInBytes / 4;
			uavDesc.Buffer.StructureByteStride = 0;
			uavDesc.Buffer.Flags			   = D3D12_BUFFER_UAV_FLAG_RAW;
		}
		else
		{
			size_t stride					   = strideInBytes;
			uavDesc.Format					   = format;
			uavDesc.Buffer.FirstElement		   = (offset / stride);
			uavDesc.Buffer.NumElements		   = (sizeInBytes / stride);
			uavDesc.Buffer.StructureByteStride = stride;
			uavDesc.Buffer.Flags			   = D3D12_BUFFER_UAV_FLAG_NONE;
		}

		device->CreateUnorderedAccessView(buffer.Get(), nullptr, &uavDesc, cpuHandle);
	}

	static void CreateSrvTextureView(Microsoft::WRL::ComPtr<ID3D12Device9> device,
									 const TextureViewD3D12				  *textureView,
									 D3D12_CPU_DESCRIPTOR_HANDLE		   cpuHandle)
	{
		Ref<TextureD3D12> texture = std::dynamic_pointer_cast<TextureD3D12>(textureView->GetTexture());

		D3D12_SHADER_RESOURCE_VIEW_DESC srv = D3D12::CreateTextureSrvView(textureView->GetDescription());

		auto resourceHandle = texture->GetHandle();
		device->CreateShaderResourceView(resourceHandle.Get(), &srv, cpuHandle);
	}

	static void CreateSampler(Microsoft::WRL::ComPtr<ID3D12Device9> device, const SamplerD3D12 *sampler, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		const Graphics::SamplerDescription &desc = sampler->GetSamplerDescription();

		const glm::vec4 color = Nexus::Utils::ColourFromBorderColor(desc.TextureBorderColor);

		D3D12_SAMPLER_DESC sd = {};
		sd.Filter			  = sampler->GetFilter();
		sd.AddressU			  = sampler->GetAddressModeU();
		sd.AddressV			  = sampler->GetAddressModeV();
		sd.AddressW			  = sampler->GetAddressModeW();
		sd.MipLODBias		  = desc.LODBias;
		sd.MaxAnisotropy	  = desc.MaximumAnisotropy;
		sd.ComparisonFunc	  = sampler->GetComparisonFunc();
		sd.BorderColor[0]	  = color.r;
		sd.BorderColor[1]	  = color.g;
		sd.BorderColor[2]	  = color.b;
		sd.BorderColor[3]	  = color.a;
		sd.MinLOD			  = desc.MinimumLOD;
		sd.MaxLOD			  = desc.MaximumLOD;

		device->CreateSampler(&sd, cpuHandle);
	}

	void ResourceSetD3D12::Flush()
	{
		Microsoft::WRL::ComPtr<ID3D12Device9> device = m_Device->GetD3D12Device();

		// uniform buffers
		for (const auto &[name, views] : m_QueuedResources.UniformBuffers)
		{
			const auto &descriptorHandles = m_SRV_UAV_CBV_DescriptorHandles.at(name);

			for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
			{
				const auto &view = views[arrayIndex];

				if (Ref<DeviceBufferD3D12> buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(view.BufferHandle))
				{
					CreateConstantBufferView(device, buffer, view.Offset, view.Size, descriptorHandles.at(arrayIndex));
					m_BoundResources.UniformBuffers[name][arrayIndex] = view;
				}
			}
		}

		// dynamic uniform buffers
		for (const auto &[name, views] : m_QueuedResources.DynamicUniformBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
			{
				const auto &view = views[arrayIndex];
				if (Ref<DeviceBufferD3D12> buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(view.BufferHandle))
				{
					m_BoundResources.DynamicUniformBuffers[name][arrayIndex] = view;
				}
			}
		}

		// inline uniform block
		for (const auto &[name, inlineData] : m_QueuedResources.InlineUniformBlocks) { m_BoundResources.InlineUniformBlocks[name] = inlineData; }

		// storage buffers
		for (const auto &[name, views] : m_QueuedResources.StorageBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
			{
				const auto &view = views[arrayIndex];
				if (Ref<DeviceBufferD3D12> buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(view.BufferHandle))
				{
					StorageResourceAccess access	  = m_DescriptorHandleInfo.StorageBuffers.at(name);
					bool				  readonly	  = false;
					bool				  byteAddress = false;
					D3D12::GetShaderAccessModifiers(access, readonly, byteAddress);
					const auto &descriptorHandles = m_SRV_UAV_CBV_DescriptorHandles.at(name);

					if (readonly)
					{
						CreateSrvBufferView(device,
											buffer->GetHandle(),
											view.Offset,
											view.SizeInBytes,
											buffer->GetStrideInBytes(),
											descriptorHandles.at(arrayIndex),
											byteAddress,
											DXGI_FORMAT_UNKNOWN);
					}
					else
					{
						CreateUavBufferView(device,
											buffer->GetHandle(),
											view.Offset,
											view.SizeInBytes,
											buffer->GetStrideInBytes(),
											descriptorHandles.at(arrayIndex),
											byteAddress,
											DXGI_FORMAT_UNKNOWN);
					}

					m_BoundResources.StorageBuffers[name][arrayIndex] = view;
				}
			}
		}

		// dynamic storage buffers
		for (const auto &[name, views] : m_QueuedResources.DynamicStorageBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
			{
				const auto &view = views[arrayIndex];
				if (Ref<DeviceBufferD3D12> buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(view.BufferHandle))
				{
					m_BoundResources.DynamicStorageBuffers[name][arrayIndex] = view;
				}
			}
		}

		// storage images
		for (const auto &[name, storageImages] : m_QueuedResources.StorageImages)
		{
			const auto &descriptorHandles = m_SRV_UAV_CBV_DescriptorHandles.at(name);

			for (size_t arrayIndex = 0; arrayIndex < storageImages.size(); arrayIndex++)
			{
				const auto &storageImage = storageImages[arrayIndex];
				if (Ref<TextureD3D12> texture = std::dynamic_pointer_cast<TextureD3D12>(storageImage.TextureHandle))
				{
					D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = D3D12::CreateTextureUavView(storageImage);

					D3D12_CPU_DESCRIPTOR_HANDLE uavHandle	   = descriptorHandles.at(arrayIndex);
					auto						resourceHandle = texture->GetHandle();
					device->CreateUnorderedAccessView(resourceHandle.Get(), nullptr, &uavDesc, uavHandle);

					m_BoundResources.StorageImages[name][arrayIndex] = storageImage;
				}
			}
		}

		// combined image samplers
		for (const auto &[name, combinedImageSamplers] : m_QueuedResources.CombinedImageSamplers)
		{
			const auto &srv_crb_uavDescriptorHandles = m_SRV_UAV_CBV_DescriptorHandles.at(name);

			// find the relevant sampler for the given texture
			std::string samplerName				 = m_DescriptorHandleInfo.CombinedImageSamplerMap.at(name);
			const auto &samplerDescriptorHandles = m_SamplerDescriptorHandles.at(samplerName);

			for (size_t arrayIndex = 0; arrayIndex < combinedImageSamplers.size(); arrayIndex++)
			{
				auto combinedImageSampler = combinedImageSamplers[arrayIndex];

				const TextureViewD3D12 *textureView = dynamic_cast<const TextureViewD3D12 *>(combinedImageSampler.ImageTexture.GetResource());
				SamplerD3D12		   *sampler		= dynamic_cast<SamplerD3D12 *>(combinedImageSampler.ImageSampler.GetResource());
				if (textureView && sampler)
				{
					D3D12_CPU_DESCRIPTOR_HANDLE textureHandle = srv_crb_uavDescriptorHandles.at(arrayIndex);
					CreateSrvTextureView(device, textureView, textureHandle);

					D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle = samplerDescriptorHandles.at(arrayIndex);
					CreateSampler(device, sampler, samplerHandle);

					m_BoundResources.CombinedImageSamplers[name][arrayIndex] = combinedImageSampler;
				}
			}
		}

		// sampled images
		for (const auto &[name, sampledImages] : m_QueuedResources.SampledImages)
		{
			const auto &srv_crb_uavDescriptorHandles = m_SRV_UAV_CBV_DescriptorHandles.at(name);

			for (size_t arrayIndex = 0; arrayIndex < sampledImages.size(); arrayIndex++)
			{
				const auto &sampledImage = sampledImages[arrayIndex];

				if (const TextureViewD3D12 *textureView = dynamic_cast<const TextureViewD3D12 *>(sampledImage.GetResource()))
				{
					D3D12_CPU_DESCRIPTOR_HANDLE textureHandle = srv_crb_uavDescriptorHandles.at(arrayIndex);
					CreateSrvTextureView(device, textureView, textureHandle);

					m_BoundResources.SampledImages[name][arrayIndex] = sampledImage;
				}
			}
		}

		// samplers
		for (const auto &[name, samplers] : m_QueuedResources.Samplers)
		{
			const auto &samplerDescriptorHandles = m_SamplerDescriptorHandles.at(name);

			for (size_t arrayIndex = 0; arrayIndex < samplers.size(); arrayIndex++)
			{
				const auto &sampler = samplers[arrayIndex];

				if (const SamplerD3D12 *samplerD3D12 = dynamic_cast<const SamplerD3D12 *>(sampler.GetResource()))
				{
					D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle = samplerDescriptorHandles.at(arrayIndex);
					CreateSampler(device, samplerD3D12, samplerHandle);

					m_BoundResources.Samplers[name][arrayIndex] = sampler;
				}
			}
		}

		// uniform texel buffers
		for (const auto &[name, texelBuffers] : m_QueuedResources.UniformTexelBuffers)
		{
			const auto &srv_crb_uavDescriptorHandles = m_SRV_UAV_CBV_DescriptorHandles.at(name);

			for (size_t arrayIndex = 0; arrayIndex < texelBuffers.size(); arrayIndex++)
			{
				const auto &texelBuffer = texelBuffers[arrayIndex];
				if (Ref<TexelBufferD3D12> texelBufferD3D12 = std::dynamic_pointer_cast<TexelBufferD3D12>(texelBuffer))
				{
					Ref<DeviceBufferD3D12> buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(texelBufferD3D12->GetDescription().Buffer);

					D3D12_CPU_DESCRIPTOR_HANDLE textureHandle = srv_crb_uavDescriptorHandles.at(arrayIndex);
					DXGI_FORMAT					pixelFormat	  = D3D12::GetD3D12PixelFormat(texelBuffer->GetDescription().Format);

					CreateSrvBufferView(device,
										buffer->GetHandle(),
										0,
										buffer->GetSizeInBytes(),
										buffer->GetStrideInBytes(),
										textureHandle,
										false,
										pixelFormat);

					m_BoundResources.UniformTexelBuffers[name][arrayIndex] = texelBuffer;
				}
			}
		}

		// storage texel buffers
		for (const auto &[name, texelBuffers] : m_QueuedResources.StorageTexelBuffers)
		{
			const auto &srv_crb_uavDescriptorHandles = m_SRV_UAV_CBV_DescriptorHandles.at(name);

			for (size_t arrayIndex = 0; arrayIndex < texelBuffers.size(); arrayIndex++)
			{
				const auto &texelBuffer = texelBuffers[arrayIndex];
				if (Ref<TexelBufferD3D12> texelBufferD3D12 = std::dynamic_pointer_cast<TexelBufferD3D12>(texelBuffer))
				{
					Ref<DeviceBufferD3D12> buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(texelBufferD3D12->GetDescription().Buffer);

					D3D12_CPU_DESCRIPTOR_HANDLE textureHandle = srv_crb_uavDescriptorHandles.at(arrayIndex);
					DXGI_FORMAT					pixelFormat	  = D3D12::GetD3D12PixelFormat(texelBuffer->GetDescription().Format);

					CreateUavBufferView(device,
										buffer->GetHandle(),
										0,
										buffer->GetSizeInBytes(),
										buffer->GetStrideInBytes(),
										textureHandle,
										false,
										pixelFormat);

					m_BoundResources.UniformTexelBuffers[name][arrayIndex] = texelBuffer;
				}
			}
		}

		// reset the resource queue
		m_QueuedResources.Reset();
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

	void ResourceSetD3D12::SetPushConstants(const std::string								 &name,
											const void										 *data,
											size_t											  offset,
											size_t											  size,
											bool											  isGraphics,
											Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
	{
		if (m_PushConstants.contains(name))
		{
			uint32_t rootParameterIndex	  = m_PushConstants.at(name);
			uint32_t offset32BitConstants = offset / 4;
			uint32_t count32BitConstants  = size / 4;

			if (isGraphics)
			{
				commandList->SetGraphicsRoot32BitConstants(rootParameterIndex, count32BitConstants, data, offset32BitConstants);
			}
			else
			{
				commandList->SetComputeRoot32BitConstants(rootParameterIndex, count32BitConstants, data, offset32BitConstants);
			}
		}
	}

	void ResourceSetD3D12::Bind(bool												isGraphics,
								Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	commandList,
								const std::map<std::string, std::vector<uint32_t>> &dynamicOffsets)
	{
		commandList->SetDescriptorHeaps(m_DescriptorHeapArray.size(), m_DescriptorHeapArray.data());

		Microsoft::WRL::ComPtr<ID3D12Device9> device = m_Device->GetD3D12Device();

		D3D12_GPU_DESCRIPTOR_HANDLE samplerHeapStartDescriptorHandle = {};
		uint32_t					samplerIncrementSize			 = 0;
		if (m_SamplerDescriptorHeap)
		{
			samplerHeapStartDescriptorHandle = m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			samplerIncrementSize			 = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		}

		D3D12_GPU_DESCRIPTOR_HANDLE cbv_srv_uavHeapStartDescriptorHandle = {};
		uint32_t					srv_uav_cbvIncrementSize			 = 0;
		if (m_SRV_UAV_CBV_DescriptorHeap)
		{
			cbv_srv_uavHeapStartDescriptorHandle = m_SRV_UAV_CBV_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			srv_uav_cbvIncrementSize			 = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		// bind descriptor tables
		for (const auto &descriptorTable : m_RootSignatureBindingLocations.HeapBindings)
		{
			if (descriptorTable.ParameterType == D3D12::RootParameterType::CBV_SRV_UAV_HeapRange)
			{
				D3D12_GPU_DESCRIPTOR_HANDLE tableHandle = cbv_srv_uavHeapStartDescriptorHandle;
				tableHandle.ptr += descriptorTable.DescriptorOffset * srv_uav_cbvIncrementSize;
				if (isGraphics)
				{
					commandList->SetGraphicsRootDescriptorTable(descriptorTable.RootParameterIndex, tableHandle);
				}
				else
				{
					commandList->SetComputeRootDescriptorTable(descriptorTable.RootParameterIndex, tableHandle);
				}
			}
			else if (descriptorTable.ParameterType == D3D12::RootParameterType::SamplerHeapRange)
			{
				D3D12_GPU_DESCRIPTOR_HANDLE tableHandle = samplerHeapStartDescriptorHandle;
				tableHandle.ptr += descriptorTable.DescriptorOffset * samplerIncrementSize;

				if (isGraphics)
				{
					commandList->SetGraphicsRootDescriptorTable(descriptorTable.RootParameterIndex, tableHandle);
				}
				else
				{
					commandList->SetComputeRootDescriptorTable(descriptorTable.RootParameterIndex, tableHandle);
				}
			}
			else
			{
				throw std::runtime_error("Invalid heap type supplied");
			}
		}

		for (const auto &[name, rootBinding] : m_RootSignatureBindingLocations.DynamicResources)
		{
			if (rootBinding.ParameterType == D3D12::RootParameterType::RootCBV)
			{
				if (m_BoundResources.DynamicUniformBuffers.contains(name))
				{
					const std::vector<UniformBufferView> &dynamicUniformBuffers = m_BoundResources.DynamicUniformBuffers.at(name);
					for (size_t uniformBufferIndex = 0; uniformBufferIndex < dynamicUniformBuffers.size(); uniformBufferIndex++)
					{
						const UniformBufferView &uboView	   = dynamicUniformBuffers.at(uniformBufferIndex);
						Ref<DeviceBufferD3D12>	 uniformBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(uboView.BufferHandle);

						uint32_t offset = 0;
						if (dynamicOffsets.contains(name))
						{
							const std::vector<uint32_t> &bufferOffsets = dynamicOffsets.at(name);
							if (uniformBufferIndex > bufferOffsets.size())
							{
								offset = bufferOffsets.at(uniformBufferIndex);
							}
						}

						// retrieve the GPU address and offset it by the requested dynamic offset
						D3D12_GPU_VIRTUAL_ADDRESS bufferAddress = uniformBuffer->GetHandle()->GetGPUVirtualAddress();
						bufferAddress += offset;

						if (isGraphics)
						{
							commandList->SetGraphicsRootConstantBufferView(rootBinding.RootParameterIndex + uniformBufferIndex, bufferAddress);
						}
						else
						{
							commandList->SetComputeRootConstantBufferView(rootBinding.RootParameterIndex + uniformBufferIndex, bufferAddress);
						}
					}
				}
			}
			else if (rootBinding.ParameterType == D3D12::RootParameterType::RootSRV)
			{
				if (m_BoundResources.DynamicUniformBuffers.contains(name))
				{
					const std::vector<StorageBufferView> &dynamicStorageBuffers = m_BoundResources.DynamicStorageBuffers.at(name);
					for (size_t storageBufferIndex = 0; storageBufferIndex < dynamicStorageBuffers.size(); storageBufferIndex++)
					{
						const StorageBufferView &sboView	   = dynamicStorageBuffers.at(storageBufferIndex);
						Ref<DeviceBufferD3D12>	 storageBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(sboView.BufferHandle);

						uint32_t offset = 0;
						if (dynamicOffsets.contains(name))
						{
							const std::vector<uint32_t> &bufferOffsets = dynamicOffsets.at(name);
							if (storageBufferIndex > bufferOffsets.size())
							{
								offset = bufferOffsets.at(storageBufferIndex);
							}
						}

						// retrieve the GPU address and offset it by the requested dynamic offset
						D3D12_GPU_VIRTUAL_ADDRESS bufferAddress = storageBuffer->GetHandle()->GetGPUVirtualAddress();
						bufferAddress += offset;

						if (isGraphics)
						{
							commandList->SetGraphicsRootShaderResourceView(rootBinding.RootParameterIndex + storageBufferIndex, bufferAddress);
						}
						else
						{
							commandList->SetComputeRootShaderResourceView(rootBinding.RootParameterIndex + storageBufferIndex, bufferAddress);
						}
					}
				}
			}
			else if (rootBinding.ParameterType == D3D12::RootParameterType::RootUAV)
			{
				if (m_BoundResources.DynamicUniformBuffers.contains(name))
				{
					const std::vector<StorageBufferView> &dynamicStorageBuffers = m_BoundResources.DynamicStorageBuffers.at(name);
					for (size_t storageBufferIndex = 0; storageBufferIndex < dynamicStorageBuffers.size(); storageBufferIndex++)
					{
						const StorageBufferView &sboView	   = dynamicStorageBuffers.at(storageBufferIndex);
						Ref<DeviceBufferD3D12>	 storageBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(sboView.BufferHandle);

						uint32_t offset = 0;
						if (dynamicOffsets.contains(name))
						{
							const std::vector<uint32_t> &bufferOffsets = dynamicOffsets.at(name);
							if (storageBufferIndex > bufferOffsets.size())
							{
								offset = bufferOffsets.at(storageBufferIndex);
							}
						}

						// retrieve the GPU address and offset it by the requested dynamic offset
						D3D12_GPU_VIRTUAL_ADDRESS bufferAddress = storageBuffer->GetHandle()->GetGPUVirtualAddress();
						bufferAddress += offset;

						if (isGraphics)
						{
							commandList->SetGraphicsRootUnorderedAccessView(rootBinding.RootParameterIndex + storageBufferIndex, bufferAddress);
						}
						else
						{
							commandList->SetGraphicsRootUnorderedAccessView(rootBinding.RootParameterIndex + storageBufferIndex, bufferAddress);
						}
					}
				}
			}
		}
	}
}	 // namespace Nexus::Graphics
#endif