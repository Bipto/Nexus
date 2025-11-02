#include "ResourceSetD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "DeviceBufferD3D12.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"
	#include "PipelineD3D12.hpp"
	#include "SamplerD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TextureViewD3D12.hpp"

namespace Nexus::Graphics
{
	ResourceSetD3D12::ResourceSetD3D12(Ref<Pipeline> pipeline, GraphicsDeviceD3D12 *device) : IResourceSet(pipeline), m_Device(device)
	{
		Ref<PipelineD3D12> pipelineD3D12				= std::dynamic_pointer_cast<PipelineD3D12>(pipeline);
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

			for (const auto &[name, offset] : m_DescriptorHandleInfo.SamplerIndexes)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				descriptorHandle.ptr += d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) * offset;
				m_SamplerDescriptorHandles[name] = descriptorHandle;
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

			for (const auto &[name, offset] : m_DescriptorHandleInfo.NonSamplerIndexes)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_SRV_UAV_CBV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				descriptorHandle.ptr += d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * offset;
				m_SRV_UAV_CBV_DescriptorHandles[name] = descriptorHandle;
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
	}

	void ResourceSetD3D12::Flush()
	{
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