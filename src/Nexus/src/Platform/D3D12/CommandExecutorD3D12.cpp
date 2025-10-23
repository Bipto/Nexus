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

	void CommandExecutorD3D12::ExecuteCommands(Ref<ICommandList> commandList, IGraphicsDevice *device)
	{
		const std::vector<RenderCommandData> &commands = commandList->GetCommandData();

		for (const auto &element : commands)
		{
			std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, element);
		}

		m_CurrentFramebuffer = {};
	}

	void CommandExecutorD3D12::Reset()
	{
	}

	void CommandExecutorD3D12::SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList)
	{
		m_CommandList = commandList;
	}

	void CommandExecutorD3D12::ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
		{
			return;
		}

		if (m_CurrentlyBoundPipeline.value()->GetType() == PipelineType::Graphics)
		{
			Ref<GraphicsPipelineD3D12> pipeline			 = std::dynamic_pointer_cast<GraphicsPipelineD3D12>(m_CurrentlyBoundPipeline.value());
			Ref<DeviceBufferD3D12>	   d3d12VertexBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.View.BufferHandle);
			const auto				  &bufferLayout		 = pipeline->GetPipelineDescription().Layouts.at(command.Slot);

			D3D12_VERTEX_BUFFER_VIEW bufferView = {};
			bufferView.BufferLocation			= d3d12VertexBuffer->GetHandle()->GetGPUVirtualAddress() + command.View.Offset;
			bufferView.SizeInBytes				= command.View.Size;
			bufferView.StrideInBytes			= pipeline->GetPipelineDescription().Layouts.at(command.Slot).GetStride();

			m_CommandList->IASetVertexBuffers(command.Slot, 1, &bufferView);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
		{
			return;
		}

		Ref<DeviceBufferD3D12> d3d12IndexBuffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.View.BufferHandle);

		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		indexBufferView.BufferLocation			= d3d12IndexBuffer->GetHandle()->GetGPUVirtualAddress() + command.View.Offset;
		indexBufferView.SizeInBytes				= command.View.Size;
		indexBufferView.Format					= D3D12::GetD3D12IndexBufferFormat(command.View.BufferFormat);

		m_CommandList->IASetIndexBuffer(&indexBufferView);
	}

	void CommandExecutorD3D12::ExecuteCommand(WeakRef<Pipeline> command, IGraphicsDevice *device)
	{
		Ref<Pipeline> pipeline = std::dynamic_pointer_cast<Pipeline>(command.lock());

		Ref<PipelineD3D12> d3d12Pipeline = std::dynamic_pointer_cast<PipelineD3D12>(pipeline);
		d3d12Pipeline->Bind(m_CommandList);
		m_CurrentlyBoundPipeline = pipeline;
	}

	void CommandExecutorD3D12::ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
		{
			return;
		}

		m_CommandList->DrawInstanced(command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorD3D12::ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
		{
			return;
		}

		m_CommandList->DrawIndexedInstanced(command.IndexCount,
											command.InstanceCount,
											command.IndexStart,
											command.VertexStart,
											command.InstanceStart);
	}

	void CommandExecutorD3D12::ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
		{
			return;
		}

		NX_VALIDATE(command.IndirectBuffer->CheckUsage(Graphics::BufferUsage::Indirect), "Buffer passed to DrawIndirect is not an indirect buffer");

		if (m_CurrentlyBoundPipeline.value()->GetType() == PipelineType::Graphics)
		{
			Ref<DeviceBufferD3D12>					indirectBuffer		 = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.IndirectBuffer);
			Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

			Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
				GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DRAW, command.Stride);

			m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
		{
			return;
		}

		NX_VALIDATE(command.IndirectBuffer->CheckUsage(Graphics::BufferUsage::Indirect), "Buffer passed to DrawIndirect is not an indirect buffer");

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

	void CommandExecutorD3D12::ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		m_CommandList->Dispatch(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
	}

	void CommandExecutorD3D12::ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		if (Ref<IDeviceBuffer> buffer = command.IndirectBuffer)
		{
			Ref<DeviceBufferD3D12>					indirectBuffer		 = std::dynamic_pointer_cast<DeviceBufferD3D12>(buffer);
			Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

			Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
				GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH, command.Stride);

			m_CommandList->ExecuteIndirect(signature.Get(), 1, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		m_CommandList->DispatchMesh(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
	}

	void CommandExecutorD3D12::ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		if (Ref<IDeviceBuffer> buffer = command.IndirectBuffer)
		{
			Ref<DeviceBufferD3D12>					indirectBuffer		 = std::dynamic_pointer_cast<DeviceBufferD3D12>(buffer);
			Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

			Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
				GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH, command.Stride);

			m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(Ref<IResourceSet> command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
		{
			return;
		}

		Nexus::Graphics::PipelineType pipelineType = m_CurrentlyBoundPipeline.value()->GetType();

		Ref<ResourceSetD3D12> d3d12ResourceSet = std::dynamic_pointer_cast<ResourceSetD3D12>(command);
		GraphicsDeviceD3D12	 *deviceD3D12	   = (GraphicsDeviceD3D12 *)device;

		const std::vector<ID3D12DescriptorHeap *> &heaps = d3d12ResourceSet->GetDescriptorHeaps();
		m_CommandList->SetDescriptorHeaps(heaps.size(), heaps.data());

		uint32_t heapIndex = 0;

		if (pipelineType == PipelineType::Graphics)
		{
			for (const D3D12_GPU_DESCRIPTOR_HANDLE &descriptorTable : d3d12ResourceSet->GetDescriptorTables())
			{
				m_CommandList->SetGraphicsRootDescriptorTable(heapIndex++, descriptorTable);
			}
		}
		else if (pipelineType == PipelineType::Compute)
		{
			for (const D3D12_GPU_DESCRIPTOR_HANDLE &descriptorTable : d3d12ResourceSet->GetDescriptorTables())
			{
				m_CommandList->SetComputeRootDescriptorTable(heapIndex++, descriptorTable);
			}
		}
		else
		{
			throw std::runtime_error("Failed to find a valid pipeline type");
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device)
	{
		if (!ValidateForClearColour(m_CurrentFramebuffer, command.Index))
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

			const auto &handle = m_DescriptorHandles[command.Index];
			m_CommandList->ClearRenderTargetView(handle, clearColor, 1, &d3d12Rect);
		}
		else
		{
			const auto &handle = m_DescriptorHandles[command.Index];
			m_CommandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device)
	{
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

	void CommandExecutorD3D12::ExecuteCommand(WeakRef<IFramebuffer> command, IGraphicsDevice *device)
	{
		SetFramebuffer(command, device);
	}

	void CommandExecutorD3D12::ExecuteCommand(const Viewport &command, IGraphicsDevice *device)
	{
		D3D12_VIEWPORT vp = {};
		vp.TopLeftX		  = command.X;
		vp.TopLeftY		  = command.Y;
		vp.Width		  = command.Width;
		vp.Height		  = command.Height;
		vp.MinDepth		  = command.MinDepth;
		vp.MaxDepth		  = command.MaxDepth;
		m_CommandList->RSSetViewports(1, &vp);
	}

	void CommandExecutorD3D12::ExecuteCommand(const Scissor &command, IGraphicsDevice *device)
	{
		RECT rect	= {};
		rect.left	= command.X;
		rect.top	= command.Y;
		rect.right	= command.Width + command.X;
		rect.bottom = command.Height + command.Y;
		m_CommandList->RSSetScissorRects(1, &rect);
	}

	void CommandExecutorD3D12::ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device)
	{
		Ref<TextureD3D12> source			= std::dynamic_pointer_cast<TextureD3D12>(command.Source);
		uint32_t		  sourceSubresource = Utils::CalculateSubresource(command.SourceMipLevel, command.SourceArrayLayer, source->GetMipLevels());
		Microsoft::WRL::ComPtr<ID3D12Resource2> sourceHandle = source->GetHandle();

		Ref<TextureD3D12> dest = std::dynamic_pointer_cast<TextureD3D12>(command.Destination);
		uint32_t		  destinationSubresource =
			Utils::CalculateSubresource(command.DestinationMipLevel, command.DestinationArrayLayer, dest->GetMipLevels());
		Microsoft::WRL::ComPtr<ID3D12Resource2> destHandle = dest->GetHandle();

		PixelFormat destFormat = dest->GetPixelFormat();

		m_CommandList->ResolveSubresource(destHandle.Get(),
										  destinationSubresource,
										  sourceHandle.Get(),
										  sourceSubresource,
										  D3D12::GetD3D12PixelFormat(destFormat));
	}

	void CommandExecutorD3D12::ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device)
	{
		Ref<TimingQueryD3D12>					queryD3D12 = std::dynamic_pointer_cast<TimingQueryD3D12>(command.Query);
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap	   = queryD3D12->GetQueryHeap();

		m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
	}

	void CommandExecutorD3D12::ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device)
	{
		Ref<TimingQueryD3D12>					queryD3D12 = std::dynamic_pointer_cast<TimingQueryD3D12>(command.Query);
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap	   = queryD3D12->GetQueryHeap();

		m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device)
	{
		Ref<DeviceBufferD3D12> source = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.BufferCopy.Source);
		Ref<DeviceBufferD3D12> dest	  = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.BufferCopy.Destination);

		for (const auto &copy : command.BufferCopy.Copies)
		{
			m_CommandList->CopyBufferRegion(dest->GetHandle().Get(), copy.WriteOffset, source->GetHandle().Get(), copy.ReadOffset, copy.Size);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device)
	{
		Ref<DeviceBufferD3D12> buffer  = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.BufferTextureCopy.BufferHandle);
		Ref<TextureD3D12>	   texture = std::dynamic_pointer_cast<TextureD3D12>(command.BufferTextureCopy.TextureHandle);

		size_t	 sizeInBytes	  = GetPixelFormatSizeInBytes(texture->GetDescription().Format);
		size_t	 rowPitch		  = sizeInBytes * command.BufferTextureCopy.TextureExtent.Width;
		uint32_t subresourceIndex = Utils::CalculateSubresource(command.BufferTextureCopy.MipLevel,
																command.BufferTextureCopy.TextureOffset.Z,
																command.BufferTextureCopy.TextureHandle->GetDescription().MipLevels);

		D3D12_BOX textureBounds = {};
		textureBounds.left		= command.BufferTextureCopy.TextureOffset.X;
		textureBounds.right		= command.BufferTextureCopy.TextureOffset.X + command.BufferTextureCopy.TextureExtent.Width;
		textureBounds.top		= command.BufferTextureCopy.TextureOffset.Y;
		textureBounds.bottom	= command.BufferTextureCopy.TextureOffset.Y + command.BufferTextureCopy.TextureExtent.Height;
		textureBounds.front		= command.BufferTextureCopy.TextureOffset.Z;
		textureBounds.back		= command.BufferTextureCopy.TextureOffset.Z + command.BufferTextureCopy.TextureExtent.Depth;

		Microsoft::WRL::ComPtr<ID3D12Resource2> bufferHandle = buffer->GetHandle();
		D3D12_TEXTURE_COPY_LOCATION				srcLocation	 = {};
		srcLocation.pResource								 = bufferHandle.Get();
		srcLocation.Type									 = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint.Offset					 = command.BufferTextureCopy.BufferOffset;
		srcLocation.PlacedFootprint.Footprint.Width			 = command.BufferTextureCopy.TextureExtent.Width;
		srcLocation.PlacedFootprint.Footprint.Height		 = command.BufferTextureCopy.TextureExtent.Height;
		srcLocation.PlacedFootprint.Footprint.Depth			 = command.BufferTextureCopy.TextureExtent.Depth;
		srcLocation.PlacedFootprint.Footprint.RowPitch		 = rowPitch;
		srcLocation.PlacedFootprint.Footprint.Format		 = texture->GetFormat();

		Microsoft::WRL::ComPtr<ID3D12Resource2> textureHandle = texture->GetHandle();
		D3D12_TEXTURE_COPY_LOCATION				dstLocation	  = {};
		dstLocation.pResource								  = textureHandle.Get();
		dstLocation.Type									  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex						  = subresourceIndex;

		TextureLayout		  layout = texture->GetTextureLayout(command.BufferTextureCopy.TextureOffset.Z, command.BufferTextureCopy.MipLevel);
		D3D12_RESOURCE_STATES resourceState = D3D12::GetTextureResourceState(layout);

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
										 command.BufferTextureCopy.TextureOffset.X,
										 command.BufferTextureCopy.TextureOffset.Y,
										 command.BufferTextureCopy.TextureOffset.Z,
										 &srcLocation,
										 &textureBounds);
		m_CommandList->ResourceBarrier(1, &toDefaultBarrier);
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device)
	{
		Ref<DeviceBufferD3D12> buffer  = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.TextureBufferCopy.BufferHandle);
		Ref<TextureD3D12>	   texture = std::dynamic_pointer_cast<TextureD3D12>(command.TextureBufferCopy.TextureHandle);

		size_t sizeInBytes = GetPixelFormatSizeInBytes(texture->GetDescription().Format);
		size_t rowPitch	   = sizeInBytes * texture->GetDescription().Width;

		D3D12_BOX textureBounds = {};
		textureBounds.left		= command.TextureBufferCopy.TextureOffset.X;
		textureBounds.right		= command.TextureBufferCopy.TextureOffset.X + command.TextureBufferCopy.TextureExtent.Width;
		textureBounds.top		= command.TextureBufferCopy.TextureOffset.Y;
		textureBounds.bottom	= command.TextureBufferCopy.TextureOffset.Y + command.TextureBufferCopy.TextureExtent.Height;
		textureBounds.front		= command.TextureBufferCopy.TextureOffset.Z;
		textureBounds.back		= command.TextureBufferCopy.TextureOffset.Z + command.TextureBufferCopy.TextureExtent.Depth;

		Microsoft::WRL::ComPtr<ID3D12Resource2> textureHandle = texture->GetHandle();
		D3D12_TEXTURE_COPY_LOCATION				srcLocation	  = {};
		srcLocation.pResource								  = textureHandle.Get();
		srcLocation.Type									  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLocation.SubresourceIndex						  = command.TextureBufferCopy.MipLevel;

		Microsoft::WRL::ComPtr<ID3D12Resource2> bufferHandle = buffer->GetHandle();
		D3D12_TEXTURE_COPY_LOCATION				dstLocation	 = {};
		dstLocation.pResource								 = bufferHandle.Get();
		dstLocation.Type									 = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dstLocation.PlacedFootprint.Offset					 = command.TextureBufferCopy.BufferOffset;
		dstLocation.PlacedFootprint.Footprint.Format		 = texture->GetFormat();
		dstLocation.PlacedFootprint.Footprint.Width			 = command.TextureBufferCopy.TextureExtent.Width;
		dstLocation.PlacedFootprint.Footprint.Height		 = command.TextureBufferCopy.TextureExtent.Height;
		dstLocation.PlacedFootprint.Footprint.Depth			 = command.TextureBufferCopy.TextureExtent.Depth;
		dstLocation.PlacedFootprint.Footprint.RowPitch		 = rowPitch;

		TextureLayout		  layout = texture->GetTextureLayout(command.TextureBufferCopy.TextureOffset.Z, command.TextureBufferCopy.MipLevel);
		D3D12_RESOURCE_STATES resourceState = D3D12::GetTextureResourceState(layout);

		D3D12_RESOURCE_BARRIER toDestBarrier = {};
		toDestBarrier.Type					 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toDestBarrier.Flags					 = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		toDestBarrier.Transition.pResource	 = textureHandle.Get();
		toDestBarrier.Transition.Subresource = command.TextureBufferCopy.MipLevel;
		toDestBarrier.Transition.StateBefore = resourceState;
		toDestBarrier.Transition.StateAfter	 = D3D12_RESOURCE_STATE_COPY_SOURCE;

		D3D12_RESOURCE_BARRIER toDefaultBarrier = {};
		toDefaultBarrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toDefaultBarrier.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
		toDefaultBarrier.Transition.pResource	= textureHandle.Get();
		toDefaultBarrier.Transition.Subresource = command.TextureBufferCopy.MipLevel;
		toDefaultBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		toDefaultBarrier.Transition.StateAfter	= resourceState;

		// copy texture data into the buffer (the 0's are for the offset into the destination texture, which we do not need here)
		m_CommandList->ResourceBarrier(1, &toDestBarrier);
		m_CommandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, &textureBounds);
		m_CommandList->ResourceBarrier(1, &toDefaultBarrier);
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device)
	{
		Ref<TextureD3D12> srcTexture = std::dynamic_pointer_cast<TextureD3D12>(command.TextureCopy.Source);
		Ref<TextureD3D12> dstTexture = std::dynamic_pointer_cast<TextureD3D12>(command.TextureCopy.Destination);

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};

		Microsoft::WRL::ComPtr<ID3D12Resource2> srcHandle = srcTexture->GetHandle();
		Microsoft::WRL::ComPtr<ID3D12Resource2> dstHandle = dstTexture->GetHandle();

		TextureLayout		  srcLayout		   = srcTexture->GetTextureLayout(command.TextureCopy.SourceOffset.Z, command.TextureCopy.SourceMipLevel);
		D3D12_RESOURCE_STATES srcResourceState = D3D12::GetTextureResourceState(srcLayout);

		TextureLayout dstLayout = dstTexture->GetTextureLayout(command.TextureCopy.DestinationOffset.Z, command.TextureCopy.DestinationMipLevel);
		D3D12_RESOURCE_STATES dstResourceState = D3D12::GetTextureResourceState(dstLayout);

		D3D12_BOX textureBounds = {};
		textureBounds.left		= command.TextureCopy.SourceOffset.X;
		textureBounds.right		= command.TextureCopy.SourceOffset.X + command.TextureCopy.Extent.Width;
		textureBounds.top		= command.TextureCopy.SourceOffset.Y;
		textureBounds.bottom	= command.TextureCopy.SourceOffset.Y + command.TextureCopy.Extent.Height;
		textureBounds.front		= command.TextureCopy.SourceOffset.Z;
		textureBounds.back		= command.TextureCopy.SourceOffset.Z + command.TextureCopy.Extent.Depth;

		// set up source
		{
			srcLocation.pResource		 = srcHandle.Get();
			srcLocation.Type			 = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			srcLocation.SubresourceIndex = command.TextureCopy.SourceMipLevel;

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource   = srcHandle.Get();
			barrier.Transition.Subresource = command.TextureCopy.SourceMipLevel;
			barrier.Transition.StateBefore = srcResourceState;
			barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;
			m_CommandList->ResourceBarrier(1, &barrier);
		}

		// set up destination
		{
			dstLocation.pResource		 = dstHandle.Get();
			dstLocation.Type			 = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dstLocation.SubresourceIndex = command.TextureCopy.DestinationMipLevel;

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource   = dstHandle.Get();
			barrier.Transition.Subresource = command.TextureCopy.DestinationMipLevel;
			barrier.Transition.StateBefore = srcResourceState;
			barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
			m_CommandList->ResourceBarrier(1, &barrier);
		}

		m_CommandList->CopyTextureRegion(&dstLocation,
										 command.TextureCopy.DestinationOffset.X,
										 command.TextureCopy.DestinationOffset.Y,
										 command.TextureCopy.DestinationOffset.Z,
										 &srcLocation,
										 &textureBounds);

		// restore resource states
		{
			D3D12_RESOURCE_BARRIER barriers[2] = {};
			barriers[0].Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barriers[0].Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barriers[0].Transition.pResource   = srcHandle.Get();
			barriers[0].Transition.Subresource = command.TextureCopy.SourceMipLevel;
			barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			barriers[0].Transition.StateAfter  = srcResourceState;

			barriers[1].Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barriers[1].Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barriers[1].Transition.pResource   = dstHandle.Get();
			barriers[1].Transition.Subresource = command.TextureCopy.DestinationMipLevel;
			barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barriers[1].Transition.StateAfter  = dstResourceState;

			m_CommandList->ResourceBarrier(2, barriers);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device)
	{
		if (m_PIXBeginEvent && m_PIXEndEvent)
		{
			uint32_t colour = Utils::PackColour(command.Colour);
			m_PIXBeginEvent(m_CommandList.Get(), colour, command.GroupName.c_str());
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device)
	{
		if (m_PIXBeginEvent && m_PIXEndEvent)
		{
			m_PIXEndEvent(m_CommandList.Get());
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device)
	{
		if (m_PIXSetMarker)
		{
			uint32_t colour = Utils::PackColour(command.Colour);
			m_PIXSetMarker(m_CommandList.Get(), colour, command.MarkerName.c_str());
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device)
	{
		float blendFactor[4] = {command.BlendFactorDesc.Red,
								command.BlendFactorDesc.Green,
								command.BlendFactorDesc.Blue,
								command.BlendFactorDesc.Alpha};

		m_CommandList->OMSetBlendFactor(blendFactor);
	}

	void CommandExecutorD3D12::ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device)
	{
		m_CommandList->OMSetStencilRef(command.StencilReference);
	}

	void CommandExecutorD3D12::ExecuteCommand(const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(const MemoryBarrierDesc &command, IGraphicsDevice *device)
	{
		GraphicsDeviceD3D12 *deviceD3D12   = (GraphicsDeviceD3D12 *)device;
		const auto			&d3d12Features = deviceD3D12->GetD3D12DeviceFeatures();

		if (d3d12Features.SupportsEnhancedBarriers)
		{
			D3D12_BARRIER_SYNC	 beforeSync	  = D3D12::GetBarrierSync(command.BeforeStage);
			D3D12_BARRIER_SYNC	 afterSync	  = D3D12::GetBarrierSync(command.AfterStage);
			D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(command.BeforeAccess);
			D3D12_BARRIER_ACCESS afterAccess  = D3D12::GetBarrierAccess(command.AfterAccess);

			D3D12_GLOBAL_BARRIER barrier = {};
			barrier.SyncBefore			 = beforeSync;
			barrier.SyncAfter			 = afterSync;
			barrier.AccessBefore		 = beforeAccess;
			barrier.AccessAfter			 = afterAccess;

			D3D12_BARRIER_GROUP barrierGroup = {};
			barrierGroup.Type				 = D3D12_BARRIER_TYPE_GLOBAL;
			barrierGroup.NumBarriers		 = 1;
			barrierGroup.pGlobalBarriers	 = &barrier;

			m_CommandList->Barrier(1, &barrierGroup);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const TextureBarrierDesc &command, IGraphicsDevice *device)
	{
		GraphicsDeviceD3D12 *deviceD3D12   = (GraphicsDeviceD3D12 *)device;
		const auto			&d3d12Features = deviceD3D12->GetD3D12DeviceFeatures();

		// if we support the newer enhanced barriers API, then we submit a texture barrier, otherwise we use the legacy ResourceBarrier API
		if (d3d12Features.SupportsEnhancedBarriers)
		{
			InsertTextureBarrier(command);
		}
		else
		{
			InsertResourceBarrier(command);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const BufferBarrierDesc &command, IGraphicsDevice *device)
	{
		GraphicsDeviceD3D12 *deviceD3D12   = (GraphicsDeviceD3D12 *)device;
		const auto			&d3d12Features = deviceD3D12->GetD3D12DeviceFeatures();

		Ref<DeviceBufferD3D12>					buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(command.Buffer);
		Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

		// if we support the newer enhanced barriers API, then we submit a texture barrier, otherwise we use the legacy ResourceBarrier API
		if (d3d12Features.SupportsEnhancedBarriers)
		{
			D3D12_BARRIER_SYNC	 beforeSync	  = D3D12::GetBarrierSync(command.BeforeStage);
			D3D12_BARRIER_SYNC	 afterSync	  = D3D12::GetBarrierSync(command.AfterStage);
			D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(command.BeforeAccess);
			D3D12_BARRIER_ACCESS afterAccess  = D3D12::GetBarrierAccess(command.AfterAccess);

			D3D12_BUFFER_BARRIER barrier = {};
			barrier.SyncBefore			 = beforeSync;
			barrier.SyncAfter			 = afterSync;
			barrier.AccessBefore		 = beforeAccess;
			barrier.AccessAfter			 = afterAccess;
			barrier.pResource			 = handle.Get();

			// these have to be hardcoded to these values as transitioning part of a buffer is not currently supported
			barrier.Offset = 0;
			barrier.Size   = UINT64_MAX;

			D3D12_BARRIER_GROUP barrierGroup = {};
			barrierGroup.Type				 = D3D12_BARRIER_TYPE_BUFFER;
			barrierGroup.NumBarriers		 = 1;
			barrierGroup.pBufferBarriers	 = &barrier;

			m_CommandList->Barrier(1, &barrierGroup);
		}
		else
		{
			if (command.BeforeAccess == BarrierAccess::ShaderWrite)
			{
				D3D12_RESOURCE_BARRIER barrier = {};
				barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_UAV;
				barrier.UAV.pResource		   = handle.Get();
				barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;

				m_CommandList->ResourceBarrier(1, &barrier);
			}
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device)
	{
		Ref<IFramebuffer> framebuffer = command.TargetFramebuffer;

		if (framebuffer->GetSampleCount() > 1)
		{
			for (size_t i = 0; i < framebuffer->GetColorTextureCount(); i++)
			{
				std::optional<FramebufferColourAttachmentDescription> colourAttachmentDescOpt = framebuffer->GetColorTextureBinding(i);
				if (colourAttachmentDescOpt.has_value())
				{
					FramebufferColourAttachmentDescription colourAttachmentDesc = colourAttachmentDescOpt.value();
					if (colourAttachmentDesc.ResolveAttachment.has_value())
					{
						for (uint32_t i = 0; i < colourAttachmentDesc.ColourAttachment.LayerCount; i++)
						{
							FramebufferTextureDescription resolveAttachmentDesc = colourAttachmentDesc.ResolveAttachment.value();

							ResolveTextureDescription resolveDesc = {};
							resolveDesc.Source					  = colourAttachmentDesc.ColourAttachment.TargetTexture;
							resolveDesc.SourceMipLevel			  = colourAttachmentDesc.ColourAttachment.MipLevel;
							resolveDesc.SourceArrayLayer		  = colourAttachmentDesc.ColourAttachment.BaseArrayLayer + i;
							resolveDesc.Destination				  = resolveAttachmentDesc.TargetTexture;
							resolveDesc.DestinationMipLevel		  = resolveAttachmentDesc.MipLevel;
							resolveDesc.DestinationArrayLayer	  = resolveAttachmentDesc.BaseArrayLayer + i;

							ExecuteCommand(resolveDesc, device);
						}
					}
				}
			}
		}
	}

	void CommandExecutorD3D12::SetFramebuffer(WeakRef<IFramebuffer> framebuffer, IGraphicsDevice *device)
	{
		ResetPreviousRenderTargets(device);
		GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

		if (auto fb = framebuffer.lock())
		{
			Ref<FramebufferD3D12> framebufferD3D12 = std::dynamic_pointer_cast<FramebufferD3D12>(fb);

			m_DescriptorHandles = framebufferD3D12->GetColourAttachmentCPUHandles();
			m_DepthHandle		= framebufferD3D12->GetDepthAttachmentCPUHandle();

			if (!fb->HasDepthTexture())
			{
				m_DepthHandle = {};
			}

			if (fb->HasDepthTexture())
			{
				m_CommandList->OMSetRenderTargets(m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, &m_DepthHandle);
			}
			else
			{
				m_CommandList->OMSetRenderTargets(m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, nullptr);
			}

			m_CurrentFramebuffer = fb;
		}
	}

	void CommandExecutorD3D12::ResetPreviousRenderTargets(IGraphicsDevice *device)
	{
		m_CurrentFramebuffer = {};
		m_DepthHandle		 = {};
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

	void CommandExecutorD3D12::InsertResourceBarrier(const TextureBarrierDesc &command)
	{
		Ref<TextureD3D12>						texture = std::dynamic_pointer_cast<TextureD3D12>(command.ITexture);
		Microsoft::WRL::ComPtr<ID3D12Resource2> handle	= texture->GetHandle();

		bool		  transitionEachSubresourceSeparately = false;
		TextureLayout testLayout						  = texture->GetTextureLayout(0, 0);

		if (command.SubresourceRange.LayerCount == texture->GetDescription().DepthOrArrayLayers &&
			command.SubresourceRange.LevelCount == texture->GetDescription().MipLevels)
		{
			for (uint32_t arrayLayer = command.SubresourceRange.BaseArrayLayer;
				 arrayLayer < command.SubresourceRange.BaseArrayLayer + command.SubresourceRange.LayerCount;
				 arrayLayer++)
			{
				for (uint32_t mipLevel = command.SubresourceRange.BaseMipLevel;
					 mipLevel < command.SubresourceRange.BaseMipLevel + command.SubresourceRange.LevelCount;
					 mipLevel++)
				{
					TextureLayout subresourceLayout = texture->GetTextureLayout(arrayLayer, mipLevel);
					if (subresourceLayout != testLayout)
					{
						transitionEachSubresourceSeparately = true;
						break;
					}
				}
			}
		}
		else
		{
			transitionEachSubresourceSeparately = true;
		}

		std::vector<D3D12_RESOURCE_BARRIER> barriers = {};

		if (!transitionEachSubresourceSeparately)
		{
			D3D12_RESOURCE_STATES beforeState = D3D12::GetTextureResourceState(testLayout);
			D3D12_RESOURCE_STATES afterState  = D3D12::GetTextureResourceState(command.Layout);

			D3D12_RESOURCE_BARRIER &barrier = barriers.emplace_back();
			barrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource	= handle.Get();
			barrier.Transition.Subresource	= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore	= beforeState;
			barrier.Transition.StateAfter	= afterState;
		}
		else
		{
			for (uint32_t arrayLayer = command.SubresourceRange.BaseArrayLayer;
				 arrayLayer < command.SubresourceRange.BaseArrayLayer + command.SubresourceRange.LayerCount;
				 arrayLayer++)
			{
				for (uint32_t mipLevel = command.SubresourceRange.BaseMipLevel;
					 mipLevel < command.SubresourceRange.BaseMipLevel + command.SubresourceRange.LevelCount;
					 mipLevel++)
				{
					D3D12_RESOURCE_STATES beforeState = D3D12::GetTextureResourceState(texture->GetTextureLayout(arrayLayer, mipLevel));
					D3D12_RESOURCE_STATES afterState  = D3D12::GetTextureResourceState(command.Layout);

					uint32_t subresourceIndex = Utils::CalculateSubresource(mipLevel, arrayLayer, texture->GetDescription().MipLevels);

					D3D12_RESOURCE_BARRIER &barrier = barriers.emplace_back();
					barrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barrier.Transition.pResource	= handle.Get();
					barrier.Transition.Subresource	= subresourceIndex;
					barrier.Transition.StateBefore	= beforeState;
					barrier.Transition.StateAfter	= afterState;
				}
			}

			m_CommandList->ResourceBarrier(barriers.size(), barriers.data());
		}
	}

	void CommandExecutorD3D12::InsertTextureBarrier(const TextureBarrierDesc &command)
	{
		Ref<TextureD3D12>						texture = std::dynamic_pointer_cast<TextureD3D12>(command.ITexture);
		Microsoft::WRL::ComPtr<ID3D12Resource2> handle	= texture->GetHandle();

		bool		  transitionEachSubresourceSeparately = false;
		TextureLayout testLayout						  = texture->GetTextureLayout(0, 0);

		if (command.SubresourceRange.LayerCount == texture->GetDescription().DepthOrArrayLayers &&
			command.SubresourceRange.LevelCount == texture->GetDescription().MipLevels)
		{
			for (uint32_t arrayLayer = command.SubresourceRange.BaseArrayLayer;
				 arrayLayer < command.SubresourceRange.BaseArrayLayer + command.SubresourceRange.LayerCount;
				 arrayLayer++)
			{
				for (uint32_t mipLevel = command.SubresourceRange.BaseMipLevel;
					 mipLevel < command.SubresourceRange.BaseMipLevel + command.SubresourceRange.LevelCount;
					 mipLevel++)
				{
					TextureLayout subresourceLayout = texture->GetTextureLayout(arrayLayer, mipLevel);
					if (subresourceLayout != testLayout)
					{
						transitionEachSubresourceSeparately = true;
						break;
					}
				}
			}
		}
		else
		{
			transitionEachSubresourceSeparately = true;
		}

		std::vector<D3D12_TEXTURE_BARRIER> barriers = {};

		if (!transitionEachSubresourceSeparately)
		{
			D3D12_BARRIER_LAYOUT beforeLayout = D3D12::GetBarrierLayout(testLayout);
			D3D12_BARRIER_LAYOUT afterLayout  = D3D12::GetBarrierLayout(command.Layout);

			D3D12_BARRIER_SYNC	 beforeSync	  = D3D12::GetBarrierSync(command.BeforeStage);
			D3D12_BARRIER_SYNC	 afterSync	  = D3D12::GetBarrierSync(command.AfterStage);
			D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(command.BeforeAccess);
			D3D12_BARRIER_ACCESS afterAccess  = D3D12::GetBarrierAccess(command.AfterAccess);

			D3D12_TEXTURE_BARRIER &barrier			  = barriers.emplace_back();
			barrier.SyncBefore						  = beforeSync;
			barrier.SyncAfter						  = afterSync;
			barrier.AccessBefore					  = beforeAccess;
			barrier.AccessAfter						  = afterAccess;
			barrier.LayoutBefore					  = beforeLayout;
			barrier.LayoutAfter						  = afterLayout;
			barrier.pResource						  = handle.Get();
			barrier.Subresources.FirstArraySlice	  = command.SubresourceRange.BaseArrayLayer;
			barrier.Subresources.NumArraySlices		  = command.SubresourceRange.LayerCount;
			barrier.Subresources.IndexOrFirstMipLevel = command.SubresourceRange.BaseMipLevel;
			barrier.Subresources.NumMipLevels		  = command.SubresourceRange.LevelCount;
			barrier.Subresources.FirstPlane			  = 0;
			barrier.Subresources.NumPlanes			  = 1;
		}
		else
		{
			for (uint32_t arrayLayer = command.SubresourceRange.BaseArrayLayer;
				 arrayLayer < command.SubresourceRange.BaseArrayLayer + command.SubresourceRange.LayerCount;
				 arrayLayer++)
			{
				for (uint32_t mipLevel = command.SubresourceRange.BaseMipLevel;
					 mipLevel < command.SubresourceRange.BaseMipLevel + command.SubresourceRange.LevelCount;
					 mipLevel++)
				{
					D3D12_BARRIER_LAYOUT beforeLayout = D3D12::GetBarrierLayout(testLayout);
					D3D12_BARRIER_LAYOUT afterLayout  = D3D12::GetBarrierLayout(command.Layout);

					D3D12_BARRIER_SYNC	 beforeSync	  = D3D12::GetBarrierSync(command.BeforeStage);
					D3D12_BARRIER_SYNC	 afterSync	  = D3D12::GetBarrierSync(command.AfterStage);
					D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(command.BeforeAccess);
					D3D12_BARRIER_ACCESS afterAccess  = D3D12::GetBarrierAccess(command.AfterAccess);

					D3D12_TEXTURE_BARRIER &barrier			  = barriers.emplace_back();
					barrier.SyncBefore						  = beforeSync;
					barrier.SyncAfter						  = afterSync;
					barrier.AccessBefore					  = beforeAccess;
					barrier.AccessAfter						  = afterAccess;
					barrier.LayoutBefore					  = beforeLayout;
					barrier.LayoutAfter						  = afterLayout;
					barrier.pResource						  = handle.Get();
					barrier.Subresources.FirstArraySlice	  = arrayLayer;
					barrier.Subresources.NumArraySlices		  = 1;
					barrier.Subresources.IndexOrFirstMipLevel = mipLevel;
					barrier.Subresources.NumMipLevels		  = 1;
					barrier.Subresources.FirstPlane			  = 0;
					barrier.Subresources.NumPlanes			  = 1;
				}
			}

			D3D12_BARRIER_GROUP barrierGroup = {};
			barrierGroup.Type				 = D3D12_BARRIER_TYPE_TEXTURE;
			barrierGroup.NumBarriers		 = barriers.size();
			barrierGroup.pTextureBarriers	 = barriers.data();

			m_CommandList->Barrier(1, &barrierGroup);
		}
	}
}	 // namespace Nexus::Graphics

#endif