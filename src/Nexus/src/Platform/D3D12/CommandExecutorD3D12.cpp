#if defined(NX_PLATFORM_D3D12)

	#include "CommandExecutorD3D12.hpp"

	#include "DeviceBufferD3D12.hpp"
	#include "FramebufferD3D12.hpp"
	#include "PipelineD3D12.hpp"
	#include "ResourceSetD3D12.hpp"
	#include "SwapchainD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TimingQueryD3D12.hpp"

	#include <WinPixEventRuntime/pix3.h>

namespace Nexus::Graphics
{
	CommandExecutorD3D12::CommandExecutorD3D12(Microsoft::WRL::ComPtr<ID3D12Device9> device) : m_Device(device)
	{
		m_PixModule = LoadLibrary("WinPixEventRuntime.dll");
		if (m_PixModule)
		{
			m_PIXBeginEvent = (PIXBeginEventFn)GetProcAddress(m_PixModule, "PIXBeginEventOnCommandList");
			m_PIXEndEvent	= (PIXEndEventFn)GetProcAddress(m_PixModule, "PIXEndEventOnCommandList");
			m_PIXSetMarker	= (PIXSetMarkerFn)GetProcAddress(m_PixModule, "PIXSetMarkerOnCommandList");
		}
		else
		{
			NX_WARNING("Failed to load PIX, some debugging functionality may not work correctly");
		}
	}

	CommandExecutorD3D12::~CommandExecutorD3D12()
	{
		FreeLibrary(m_PixModule);
		m_PixModule = NULL;
	}

	void CommandExecutorD3D12::ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device)
	{
		for (const auto &element : commands)
		{
			std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, element);
		}
	}

	void CommandExecutorD3D12::Reset()
	{
	}

	void CommandExecutorD3D12::SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList)
	{
		m_CommandList = commandList;
	}

	void CommandExecutorD3D12::ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (m_CurrentlyBoundPipeline.value()->GetType() == PipelineType::Graphics)
		{
			Ref<GraphicsPipelineD3D12> pipeline			 = std::dynamic_pointer_cast<GraphicsPipelineD3D12>(m_CurrentlyBoundPipeline.value());
			Ref<DeviceBufferD3D12>	   d3d12VertexBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.View.BufferHandle);
			const auto				  &bufferLayout		 = pipeline->GetPipelineDescription().Layouts.at(command.Slot);

			D3D12_VERTEX_BUFFER_VIEW bufferView;
			bufferView.BufferLocation = d3d12VertexBuffer->GetHandle()->GetGPUVirtualAddress() + command.View.Offset;
			bufferView.SizeInBytes	  = command.View.Size;
			bufferView.StrideInBytes  = pipeline->GetPipelineDescription().Layouts.at(command.Slot).GetStride();

			m_CommandList->IASetVertexBuffers(command.Slot, 1, &bufferView);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		Ref<DeviceBufferD3D12> d3d12IndexBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.View.BufferHandle);

		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		indexBufferView.BufferLocation = d3d12IndexBuffer->GetHandle()->GetGPUVirtualAddress() + command.View.Offset;
		indexBufferView.SizeInBytes	   = command.View.Size;
		indexBufferView.Format		   = D3D12::GetD3D12IndexBufferFormat(command.View.BufferFormat);

		m_CommandList->IASetIndexBuffer(&indexBufferView);
	}

	void CommandExecutorD3D12::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
	{
		Ref<Pipeline> pipeline = std::dynamic_pointer_cast<Pipeline>(command.lock());

		Ref<PipelineD3D12> d3d12Pipeline = std::dynamic_pointer_cast<PipelineD3D12>(pipeline);
		d3d12Pipeline->Bind(m_CommandList);
		m_CurrentlyBoundPipeline = pipeline;
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawDescription command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_CommandList->DrawInstanced(command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawIndexedDescription command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_CommandList->DrawIndexedInstanced(command.IndexCount,
											command.InstanceCount,
											command.IndexStart,
											command.VertexStart,
											command.InstanceStart);
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawIndirectDescription command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		NX_ASSERT(command.IndirectBuffer->CheckUsage(Graphics::BufferUsage::Indirect), "Buffer passed to DrawIndirect is not an indirect buffer");

		if (m_CurrentlyBoundPipeline.value()->GetType() == PipelineType::Graphics)
		{
			Ref<DeviceBufferD3D12>					indirectBuffer		 = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.IndirectBuffer);
			Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

			Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
				GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DRAW, command.Stride);

			m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawIndirectIndexedDescription command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		NX_ASSERT(command.IndirectBuffer->CheckUsage(Graphics::BufferUsage::Indirect), "Buffer passed to DrawIndirect is not an indirect buffer");

		if (m_CurrentlyBoundPipeline.value()->GetType() == PipelineType::Graphics)
		{
			Ref<DeviceBufferD3D12>					indirectBuffer		 = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.IndirectBuffer);
			Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

			Nexus::Ref<Nexus::Graphics::GraphicsPipelineD3D12> pipeline =
				std::dynamic_pointer_cast<GraphicsPipelineD3D12>(m_CurrentlyBoundPipeline.value());

			Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
				GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED, command.Stride);

			m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(DispatchDescription command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		m_CommandList->Dispatch(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
	}

	void CommandExecutorD3D12::ExecuteCommand(DispatchIndirectDescription command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		if (Ref<DeviceBuffer> buffer = command.IndirectBuffer)
		{
			Ref<DeviceBufferD3D12>					indirectBuffer		 = std::dynamic_pointer_cast<DeviceBufferD3D12>(buffer);
			Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

			Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
				GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH, command.Stride);

			m_CommandList->ExecuteIndirect(signature.Get(), 1, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawMeshDescription command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawMeshIndirectDescription command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		Nexus::Graphics::PipelineType pipelineType = m_CurrentlyBoundPipeline.value()->GetType();

		Ref<ResourceSetD3D12> d3d12ResourceSet = std::dynamic_pointer_cast<ResourceSetD3D12>(command);
		GraphicsDeviceD3D12	 *deviceD3D12	   = (GraphicsDeviceD3D12 *)device;

		const std::map<std::string, Nexus::Graphics::StorageBufferView> &storageBuffers = d3d12ResourceSet->GetBoundStorageBuffers();
		for (const auto &[name, view] : storageBuffers)
		{
			Ref<DeviceBuffer>	   storageBuffer	  = view.BufferHandle;
			Ref<DeviceBufferD3D12> d3d12StorageBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(storageBuffer);
			auto				   resourceHandle	  = d3d12StorageBuffer->GetHandle();
			deviceD3D12->ResourceBarrierBuffer(m_CommandList.Get(),
											   d3d12StorageBuffer,
											   D3D12_RESOURCE_STATE_COPY_DEST,
											   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}

		std::vector<ID3D12DescriptorHeap *> heaps;
		if (d3d12ResourceSet->HasSamplerHeap())
		{
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> samplerDescriptorHeap = d3d12ResourceSet->GetSamplerDescriptorHeap();
			heaps.push_back(samplerDescriptorHeap.Get());
		}

		if (d3d12ResourceSet->HasConstantBufferTextureHeap())
		{
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> textureConstantBufferDescriptorHeap =
				d3d12ResourceSet->GetTextureConstantBufferDescriptorHeap();
			heaps.push_back(textureConstantBufferDescriptorHeap.Get());
		}

		m_CommandList->SetDescriptorHeaps(heaps.size(), heaps.data());

		uint32_t descriptorIndex = 0;

		if (pipelineType == PipelineType::Graphics)
		{
			// bind samplers
			if (d3d12ResourceSet->HasSamplerHeap())
			{
				m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetSamplerStartHandle());
			}

			// bind textures/constant buffers
			if (d3d12ResourceSet->HasConstantBufferTextureHeap())
			{
				m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetTextureConstantBufferStartHandle());
			}

			if (d3d12ResourceSet->GetBoundStorageBuffers().size() > 0)
			{
				// m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetTextureConstantBufferStartHandle());
				m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetStorageBufferStartHandle());
			}
		}

		else if (pipelineType == PipelineType::Compute)
		{
			// bind samplers
			if (d3d12ResourceSet->HasSamplerHeap())
			{
				m_CommandList->SetComputeRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetSamplerStartHandle());
			}

			// bind textures/constant buffers
			if (d3d12ResourceSet->HasConstantBufferTextureHeap())
			{
				m_CommandList->SetComputeRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetTextureConstantBufferStartHandle());
			}
		}
		else
		{
			throw std::runtime_error("Failed to find a valid pipeline type");
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device)
	{
		if (!ValidateForClearColour(m_CurrentRenderTarget, command.Index))
		{
			return;
		}

		float clearColor[] = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};

		if (command.Rect.has_value())
		{
			Graphics::ClearRect rect = command.Rect.value();

			D3D12_RECT d3d12Rect = {};
			d3d12Rect.left		 = rect.X;
			d3d12Rect.top		 = rect.Y;
			d3d12Rect.right		 = rect.X + rect.Width;
			d3d12Rect.bottom	 = rect.Y + rect.Height;

			auto handle = m_DescriptorHandles[command.Index];
			m_CommandList->ClearRenderTargetView(handle, clearColor, 1, &d3d12Rect);
		}
		else
		{
			auto handle = m_DescriptorHandles[command.Index];
			m_CommandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device)
	{
		if (!ValidateForClearDepth(m_CurrentRenderTarget))
		{
			return;
		}

		if (m_DepthHandle.ptr)
		{
			D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;

			if (command.Rect.has_value())
			{
				Graphics::ClearRect rect = command.Rect.value();

				D3D12_RECT d3d12Rect = {};
				d3d12Rect.left		 = rect.X;
				d3d12Rect.top		 = rect.Y;
				d3d12Rect.right		 = rect.X + rect.Width;
				d3d12Rect.bottom	 = rect.Y + rect.Height;
				m_CommandList->ClearDepthStencilView(m_DepthHandle, clearFlags, command.Value.Depth, command.Value.Stencil, 1, &d3d12Rect);
			}
			else
			{
				m_CommandList->ClearDepthStencilView(m_DepthHandle, clearFlags, command.Value.Depth, command.Value.Stencil, 0, nullptr);
			}
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
	{
		if (command.GetType() == RenderTargetType::Swapchain)
		{
			WeakRef<Swapchain> swapchain = command.GetSwapchain();
			SetSwapchain(swapchain, device);
		}
		else
		{
			WeakRef<Framebuffer> framebuffer = command.GetFramebuffer();
			SetFramebuffer(framebuffer, device);
		}

		m_CurrentRenderTarget = command;

		if (m_CurrentRenderTarget.value().HasDepthAttachment())
		{
			m_CommandList->OMSetRenderTargets(m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, &m_DepthHandle);
		}
		else
		{
			m_CommandList->OMSetRenderTargets(m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, nullptr);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const Viewport &command, GraphicsDevice *device)
	{
		if (!ValidateForSetViewport(m_CurrentRenderTarget, command))
		{
			return;
		}

		D3D12_VIEWPORT vp;
		vp.TopLeftX = command.X;
		vp.TopLeftY = command.Y;
		vp.Width	= command.Width;
		vp.Height	= command.Height;
		vp.MinDepth = command.MinDepth;
		vp.MaxDepth = command.MaxDepth;
		m_CommandList->RSSetViewports(1, &vp);
	}

	void CommandExecutorD3D12::ExecuteCommand(const Scissor &command, GraphicsDevice *device)
	{
		if (!ValidateForSetScissor(m_CurrentRenderTarget, command))
		{
			return;
		}

		RECT rect;
		rect.left	= command.X;
		rect.top	= command.Y;
		rect.right	= command.Width + command.X;
		rect.bottom = command.Height + command.Y;
		m_CommandList->RSSetScissorRects(1, &rect);
	}

	void CommandExecutorD3D12::ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device)
	{
		if (!ValidateForResolveToSwapchain(command))
		{
			return;
		}

		Ref<Framebuffer> framebuffer = command.Source;
		Ref<Swapchain>	 swapchain	 = command.Target;

		auto framebufferD3D12 = std::dynamic_pointer_cast<FramebufferD3D12>(framebuffer);
		auto swapchainD3D12	  = std::dynamic_pointer_cast<SwapchainD3D12>(swapchain);

		Nexus::Ref<Nexus::Graphics::TextureD3D12> framebufferTexture = framebufferD3D12->GetD3D12ColorTexture(command.SourceIndex);
		Microsoft::WRL::ComPtr<ID3D12Resource2>	  swapchainTexture	 = swapchainD3D12->RetrieveBufferHandle();
		DXGI_FORMAT								  format			 = D3D12::GetD3D12PixelFormat(Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm);
		D3D12_RESOURCE_STATES					  swapchainState	 = swapchainD3D12->GetCurrentTextureState();

		if (framebufferD3D12->GetFramebufferSpecification().Width > swapchainD3D12->GetWindow()->GetWindowSize().X)
		{
			return;
		}

		if (framebufferD3D12->GetFramebufferSpecification().Height > swapchainD3D12->GetWindow()->GetWindowSize().Y)
		{
			return;
		}

		GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

		deviceD3D12->ResourceBarrier(m_CommandList.Get(), framebufferTexture, 0, 0, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);

		deviceD3D12->ResourceBarrier(m_CommandList.Get(),
									 swapchainTexture.Get(),
									 0,
									 0,
									 1,
									 swapchainD3D12->GetCurrentTextureState(),
									 D3D12_RESOURCE_STATE_RESOLVE_DEST);

		swapchainD3D12->SetTextureState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
		m_CommandList->ResolveSubresource(swapchainTexture.Get(), 0, framebufferTexture->GetHandle().Get(), 0, format);
	}

	void CommandExecutorD3D12::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
	{
		Ref<TimingQueryD3D12>					queryD3D12 = std::dynamic_pointer_cast<TimingQueryD3D12>(command.Query);
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap	   = queryD3D12->GetQueryHeap();

		m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
	}

	void CommandExecutorD3D12::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
	{
		Ref<TimingQueryD3D12>					queryD3D12 = std::dynamic_pointer_cast<TimingQueryD3D12>(command.Query);
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap	   = queryD3D12->GetQueryHeap();

		m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferD3D12> source = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.BufferCopy.Source);
		Ref<DeviceBufferD3D12> dest	  = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.BufferCopy.Destination);
		m_CommandList->CopyBufferRegion(dest->GetHandle().Get(),
										command.BufferCopy.WriteOffset,
										source->GetHandle().Get(),
										command.BufferCopy.ReadOffset,
										command.BufferCopy.Size);
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferD3D12> buffer  = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.BufferTextureCopy.BufferHandle);
		Ref<TextureD3D12>	   texture = std::dynamic_pointer_cast<TextureD3D12>(command.BufferTextureCopy.TextureHandle);

		size_t	 sizeInBytes	  = GetPixelFormatSizeInBytes(texture->GetDescription().Format);
		size_t	 rowPitch		  = sizeInBytes * command.BufferTextureCopy.TextureSubresource.Width;
		uint32_t subresourceIndex = Utils::CalculateSubresource(command.BufferTextureCopy.TextureSubresource.MipLevel,
																command.BufferTextureCopy.TextureSubresource.ArrayLayer,
																command.BufferTextureCopy.TextureHandle->GetDescription().MipLevels);

		D3D12_BOX textureBounds = {};
		textureBounds.left		= command.BufferTextureCopy.TextureSubresource.X;
		textureBounds.right		= command.BufferTextureCopy.TextureSubresource.X + command.BufferTextureCopy.TextureSubresource.Width;
		textureBounds.top		= command.BufferTextureCopy.TextureSubresource.Y;
		textureBounds.bottom	= command.BufferTextureCopy.TextureSubresource.Y + command.BufferTextureCopy.TextureSubresource.Height;
		textureBounds.front		= command.BufferTextureCopy.TextureSubresource.Z;
		textureBounds.back		= command.BufferTextureCopy.TextureSubresource.Z + command.BufferTextureCopy.TextureSubresource.Depth;

		Microsoft::WRL::ComPtr<ID3D12Resource2> bufferHandle = buffer->GetHandle();
		D3D12_TEXTURE_COPY_LOCATION				srcLocation	 = {};
		srcLocation.pResource								 = bufferHandle.Get();
		srcLocation.Type									 = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint.Offset					 = command.BufferTextureCopy.BufferOffset;
		srcLocation.PlacedFootprint.Footprint.Width			 = command.BufferTextureCopy.TextureSubresource.Width;
		srcLocation.PlacedFootprint.Footprint.Height		 = command.BufferTextureCopy.TextureSubresource.Height;
		srcLocation.PlacedFootprint.Footprint.Depth			 = command.BufferTextureCopy.TextureSubresource.Depth;
		srcLocation.PlacedFootprint.Footprint.RowPitch		 = rowPitch;
		srcLocation.PlacedFootprint.Footprint.Format		 = texture->GetFormat();

		Microsoft::WRL::ComPtr<ID3D12Resource2> textureHandle = texture->GetHandle();
		D3D12_TEXTURE_COPY_LOCATION				dstLocation	  = {};
		dstLocation.pResource								  = textureHandle.Get();
		dstLocation.Type									  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex						  = subresourceIndex;

		D3D12_RESOURCE_STATES resourceState =
			texture->GetResourceState(command.BufferTextureCopy.TextureSubresource.Z, command.BufferTextureCopy.TextureSubresource.MipLevel);

		D3D12_RESOURCE_BARRIER toReadBarrier = {};
		toReadBarrier.Type					 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toReadBarrier.Flags					 = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		toReadBarrier.Transition.pResource	 = textureHandle.Get();
		toReadBarrier.Transition.Subresource = subresourceIndex;
		toReadBarrier.Transition.StateBefore = resourceState;
		toReadBarrier.Transition.StateAfter	 = D3D12_RESOURCE_STATE_COPY_DEST;

		D3D12_RESOURCE_BARRIER toDefaultBarrier = {};
		toDefaultBarrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toDefaultBarrier.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
		toDefaultBarrier.Transition.pResource	= textureHandle.Get();
		toDefaultBarrier.Transition.Subresource = subresourceIndex;
		toDefaultBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		toDefaultBarrier.Transition.StateAfter	= resourceState;

		m_CommandList->ResourceBarrier(1, &toReadBarrier);
		m_CommandList->CopyTextureRegion(&dstLocation,
										 command.BufferTextureCopy.TextureSubresource.X,
										 command.BufferTextureCopy.TextureSubresource.Y,
										 command.BufferTextureCopy.TextureSubresource.Z,
										 &srcLocation,
										 &textureBounds);
		m_CommandList->ResourceBarrier(1, &toDefaultBarrier);
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferD3D12> buffer  = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.TextureBufferCopy.BufferHandle);
		Ref<TextureD3D12>	   texture = std::dynamic_pointer_cast<TextureD3D12>(command.TextureBufferCopy.TextureHandle);

		size_t sizeInBytes = GetPixelFormatSizeInBytes(texture->GetDescription().Format);
		size_t rowPitch	   = sizeInBytes * texture->GetDescription().Width;

		D3D12_BOX textureBounds = {};
		textureBounds.left		= command.TextureBufferCopy.TextureSubresource.X;
		textureBounds.right		= command.TextureBufferCopy.TextureSubresource.X + command.TextureBufferCopy.TextureSubresource.Width;
		textureBounds.top		= command.TextureBufferCopy.TextureSubresource.Y;
		textureBounds.bottom	= command.TextureBufferCopy.TextureSubresource.Y + command.TextureBufferCopy.TextureSubresource.Height;
		textureBounds.front		= command.TextureBufferCopy.TextureSubresource.Z;
		textureBounds.back		= command.TextureBufferCopy.TextureSubresource.Z + command.TextureBufferCopy.TextureSubresource.Depth;

		Microsoft::WRL::ComPtr<ID3D12Resource2> textureHandle = texture->GetHandle();
		D3D12_TEXTURE_COPY_LOCATION				srcLocation	  = {};
		srcLocation.pResource								  = textureHandle.Get();
		srcLocation.Type									  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLocation.SubresourceIndex						  = command.TextureBufferCopy.TextureSubresource.MipLevel;

		Microsoft::WRL::ComPtr<ID3D12Resource2> bufferHandle = buffer->GetHandle();
		D3D12_TEXTURE_COPY_LOCATION				dstLocation	 = {};
		dstLocation.pResource								 = bufferHandle.Get();
		dstLocation.Type									 = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dstLocation.PlacedFootprint.Offset					 = command.TextureBufferCopy.BufferOffset;
		dstLocation.PlacedFootprint.Footprint.Format		 = texture->GetFormat();
		dstLocation.PlacedFootprint.Footprint.Width			 = command.TextureBufferCopy.TextureSubresource.Width;
		dstLocation.PlacedFootprint.Footprint.Height		 = command.TextureBufferCopy.TextureSubresource.Height;
		dstLocation.PlacedFootprint.Footprint.Depth			 = command.TextureBufferCopy.TextureSubresource.Depth;
		dstLocation.PlacedFootprint.Footprint.RowPitch		 = rowPitch;

		D3D12_RESOURCE_STATES resourceState =
			texture->GetResourceState(command.TextureBufferCopy.TextureSubresource.Z, command.TextureBufferCopy.TextureSubresource.MipLevel);

		D3D12_RESOURCE_BARRIER toDestBarrier = {};
		toDestBarrier.Type					 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toDestBarrier.Flags					 = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		toDestBarrier.Transition.pResource	 = textureHandle.Get();
		toDestBarrier.Transition.Subresource = command.TextureBufferCopy.TextureSubresource.MipLevel;
		toDestBarrier.Transition.StateBefore = resourceState;
		toDestBarrier.Transition.StateAfter	 = D3D12_RESOURCE_STATE_COPY_SOURCE;

		D3D12_RESOURCE_BARRIER toDefaultBarrier = {};
		toDefaultBarrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toDefaultBarrier.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
		toDefaultBarrier.Transition.pResource	= textureHandle.Get();
		toDefaultBarrier.Transition.Subresource = command.TextureBufferCopy.TextureSubresource.MipLevel;
		toDefaultBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		toDefaultBarrier.Transition.StateAfter	= resourceState;

		// copy texture data into the buffer (the 0's are for the offset into the destination texture, which we do not need here)
		m_CommandList->ResourceBarrier(1, &toDestBarrier);
		m_CommandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, &textureBounds);
		m_CommandList->ResourceBarrier(1, &toDefaultBarrier);
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device)
	{
		Ref<TextureD3D12> srcTexture = std::dynamic_pointer_cast<TextureD3D12>(command.TextureCopy.Source);
		Ref<TextureD3D12> dstTexture = std::dynamic_pointer_cast<TextureD3D12>(command.TextureCopy.Destination);

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};

		Microsoft::WRL::ComPtr<ID3D12Resource2> srcHandle = srcTexture->GetHandle();
		Microsoft::WRL::ComPtr<ID3D12Resource2> dstHandle = dstTexture->GetHandle();

		D3D12_RESOURCE_STATES srcResourceState =
			srcTexture->GetResourceState(command.TextureCopy.SourceSubresource.Z, command.TextureCopy.SourceSubresource.MipLevel);

		D3D12_RESOURCE_STATES dstResourceState =
			dstTexture->GetResourceState(command.TextureCopy.DestinationSubresource.Z, command.TextureCopy.DestinationSubresource.MipLevel);

		D3D12_BOX textureBounds = {};
		textureBounds.left		= command.TextureCopy.SourceSubresource.X;
		textureBounds.right		= command.TextureCopy.SourceSubresource.X + command.TextureCopy.SourceSubresource.Width;
		textureBounds.top		= command.TextureCopy.SourceSubresource.Y;
		textureBounds.bottom	= command.TextureCopy.SourceSubresource.Y + command.TextureCopy.SourceSubresource.Height;
		textureBounds.front		= command.TextureCopy.SourceSubresource.Z;
		textureBounds.back		= command.TextureCopy.SourceSubresource.Z + command.TextureCopy.SourceSubresource.Depth;

		// set up source
		{
			srcLocation.pResource		 = srcHandle.Get();
			srcLocation.Type			 = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			srcLocation.SubresourceIndex = command.TextureCopy.SourceSubresource.MipLevel;

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource   = srcHandle.Get();
			barrier.Transition.Subresource = command.TextureCopy.SourceSubresource.MipLevel;
			barrier.Transition.StateBefore = srcResourceState;
			barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;
			m_CommandList->ResourceBarrier(1, &barrier);
		}

		// set up destination
		{
			dstLocation.pResource		 = dstHandle.Get();
			dstLocation.Type			 = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dstLocation.SubresourceIndex = command.TextureCopy.DestinationSubresource.MipLevel;

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource   = dstHandle.Get();
			barrier.Transition.Subresource = command.TextureCopy.SourceSubresource.MipLevel;
			barrier.Transition.StateBefore = srcResourceState;
			barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
			m_CommandList->ResourceBarrier(1, &barrier);
		}

		m_CommandList->CopyTextureRegion(&dstLocation,
										 command.TextureCopy.DestinationSubresource.X,
										 command.TextureCopy.DestinationSubresource.Y,
										 command.TextureCopy.DestinationSubresource.Z,
										 &srcLocation,
										 &textureBounds);

		// restore resource states
		{
			D3D12_RESOURCE_BARRIER barriers[2];
			barriers[0].Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barriers[0].Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barriers[0].Transition.pResource   = srcHandle.Get();
			barriers[0].Transition.Subresource = command.TextureCopy.SourceSubresource.MipLevel;
			barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			barriers[0].Transition.StateAfter  = srcResourceState;

			barriers[1].Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barriers[1].Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barriers[1].Transition.pResource   = dstHandle.Get();
			barriers[1].Transition.Subresource = command.TextureCopy.DestinationSubresource.MipLevel;
			barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barriers[1].Transition.StateAfter  = dstResourceState;

			m_CommandList->ResourceBarrier(2, barriers);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(BeginDebugGroupCommand command, GraphicsDevice *device)
	{
		if (m_PIXBeginEvent && m_PIXEndEvent)
		{
			m_PIXBeginEvent(m_CommandList.Get(), PIX_COLOR_DEFAULT, command.GroupName.c_str());
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(EndDebugGroupCommand command, GraphicsDevice *device)
	{
		if (m_PIXBeginEvent && m_PIXEndEvent)
		{
			m_PIXEndEvent(m_CommandList.Get());
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(InsertDebugMarkerCommand command, GraphicsDevice *device)
	{
		if (m_PIXSetMarker)
		{
			m_PIXSetMarker(m_CommandList.Get(), PIX_COLOR_DEFAULT, command.MarkerName.c_str());
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device)
	{
		float blendFactor[4] = {command.BlendFactorDesc.Red,
								command.BlendFactorDesc.Green,
								command.BlendFactorDesc.Blue,
								command.BlendFactorDesc.Alpha};

		m_CommandList->OMSetBlendFactor(blendFactor);
	}

	void CommandExecutorD3D12::ExecuteCommand(SetStencilReferenceCommand command, GraphicsDevice *device)
	{
		m_CommandList->OMSetStencilRef(command.StencilReference);
	}

	void CommandExecutorD3D12::ExecuteCommand(BuildAccelerationStructuresCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(AccelerationStructureCopyDescription command, GraphicsDevice *Device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(AccelerationStructureDeviceBufferCopyDescription command, GraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(DeviceBufferAccelerationStructureCopyDescription command, GraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::SetSwapchain(WeakRef<Swapchain> swapchain, GraphicsDevice *device)
	{
		if (Ref<Swapchain> sc = swapchain.lock())
		{
			Ref<SwapchainD3D12> swapchainD3D12 = std::dynamic_pointer_cast<SwapchainD3D12>(sc);

			if (sc->GetDescription().Samples == 1)
			{
				std::vector<D3D12_RESOURCE_BARRIER> barriers;
				GraphicsDeviceD3D12				   *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

				ResetPreviousRenderTargets(device);

				m_DescriptorHandles = {swapchainD3D12->RetrieveRenderTargetViewDescriptorHandle()};
				m_DepthHandle		= swapchainD3D12->RetrieveDepthBufferDescriptorHandle();

				deviceD3D12->ResourceBarrierSwapchainColour(m_CommandList.Get(), swapchainD3D12, D3D12_RESOURCE_STATE_RENDER_TARGET);
				deviceD3D12->ResourceBarrierSwapchainDepth(m_CommandList.Get(), swapchainD3D12, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			}
			else
			{
				SetFramebuffer(std::dynamic_pointer_cast<FramebufferD3D12>(swapchainD3D12->GetMultisampledFramebuffer()), device);
			}
		}
	}

	void CommandExecutorD3D12::SetFramebuffer(WeakRef<Framebuffer> framebuffer, GraphicsDevice *device)
	{
		ResetPreviousRenderTargets(device);
		GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

		if (auto fb = framebuffer.lock())
		{
			Ref<FramebufferD3D12> framebufferD3D12 = std::dynamic_pointer_cast<FramebufferD3D12>(fb);

			m_DescriptorHandles = framebufferD3D12->GetColorAttachmentCPUHandles();
			m_DepthHandle		= framebufferD3D12->GetDepthAttachmentCPUHandle();

			for (size_t i = 0; i < framebufferD3D12->GetColorTextureCount(); i++)
			{
				Ref<TextureD3D12> texture = framebufferD3D12->GetD3D12ColorTexture(i);
				deviceD3D12->ResourceBarrier(m_CommandList.Get(), texture, 0, 0, D3D12_RESOURCE_STATE_RENDER_TARGET);
			}

			if (fb->HasDepthTexture())
			{
				Ref<TextureD3D12> depthBuffer = framebufferD3D12->GetD3D12DepthTexture();
				deviceD3D12->ResourceBarrier(m_CommandList.Get(), depthBuffer, 0, 0, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			}
			else
			{
				m_DepthHandle = {};
			}
		}
	}

	void CommandExecutorD3D12::ResetPreviousRenderTargets(GraphicsDevice *device)
	{
		if (!m_CurrentRenderTarget.has_value())
		{
			return;
		}

		RenderTarget target = m_CurrentRenderTarget.value();

		// check that the swapchain is still valid
		WeakRef<Swapchain> swapchain = target.GetSwapchain();
		if (auto sc = swapchain.lock())
		{
			Ref<SwapchainD3D12> swapchainD3D12 = std::dynamic_pointer_cast<SwapchainD3D12>(sc);

			auto swapchainColourState = swapchainD3D12->GetCurrentTextureState();
			if (swapchainColourState != D3D12_RESOURCE_STATE_PRESENT)
			{
				D3D12_RESOURCE_BARRIER presentBarrier;
				presentBarrier.Type					  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				presentBarrier.Flags				  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				presentBarrier.Transition.pResource	  = swapchainD3D12->RetrieveBufferHandle().Get();
				presentBarrier.Transition.Subresource = 0;
				presentBarrier.Transition.StateBefore = swapchainColourState;
				presentBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
				m_CommandList->ResourceBarrier(1, &presentBarrier);

				swapchainD3D12->SetTextureState(D3D12_RESOURCE_STATE_PRESENT);
			}
		}

		m_CurrentRenderTarget = {};
		m_DepthHandle		  = {};
	}

	void CommandExecutorD3D12::CreateDrawIndirectSignatureCommand()
	{
		D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
		argumentDesc.Type						  = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

		D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
		commandSignatureDesc.pArgumentDescs				  = &argumentDesc;
		commandSignatureDesc.NumArgumentDescs			  = 1;
		commandSignatureDesc.ByteStride					  = sizeof(D3D12_DRAW_ARGUMENTS);

		m_Device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(m_DrawIndirectCommandSignature.GetAddressOf()));
	}

	void CommandExecutorD3D12::CreateDrawIndexedIndirectSignatureCommand()
	{
		D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
		argumentDesc.Type						  = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

		D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
		commandSignatureDesc.pArgumentDescs				  = &argumentDesc;
		commandSignatureDesc.NumArgumentDescs			  = 1;
		commandSignatureDesc.ByteStride					  = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);

		m_Device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(m_DrawIndexedIndirectCommandSignature.GetAddressOf()));
	}

	void CommandExecutorD3D12::CreateDispatchIndirectSignatureCommand()
	{
		D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
		argumentDesc.Type						  = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

		D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
		commandSignatureDesc.pArgumentDescs				  = &argumentDesc;
		commandSignatureDesc.NumArgumentDescs			  = 1;
		commandSignatureDesc.ByteStride					  = sizeof(D3D12_DISPATCH_ARGUMENTS);

		m_Device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(m_DispatchIndirectCommandSignature.GetAddressOf()));
	}

	Microsoft::WRL::ComPtr<ID3D12CommandSignature> CommandExecutorD3D12::GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE type,
																											 size_t						  stride)
	{
		// element found in map
		if (m_IndirectCommandSignatures[type].find(stride) != m_IndirectCommandSignatures[type].end())
		{
			return m_IndirectCommandSignatures[type][stride];
		}
		else
		{
			D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
			argumentDesc.Type						  = type;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
			commandSignatureDesc.pArgumentDescs				  = &argumentDesc;
			commandSignatureDesc.NumArgumentDescs			  = 1;
			commandSignatureDesc.ByteStride					  = stride;

			Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature;
			m_Device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(signature.GetAddressOf()));
			m_IndirectCommandSignatures[type][stride] = signature;
			return signature;
		}
	}
}	 // namespace Nexus::Graphics

#endif