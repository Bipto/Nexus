
#if defined(NX_PLATFORM_D3D12)

	#include "CommandExecutorD3D12.hpp"

	#include "DeviceBufferD3D12.hpp"
	#include "FramebufferD3D12.hpp"
	#include "PipelineD3D12.hpp"
	#include "ResourceSetD3D12.hpp"
	#include "SwapchainD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TimingQueryD3D12.hpp"

	#include "Nexus-Core/Utils/Utils.hpp"

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
		const std::vector<std::unique_ptr<IGraphicsCommand>> &commands = commandList->GetCommands();

		for (const auto &command : commands) { command->Execute(this, device); }

		m_CurrentFramebuffer = {};
	}

	void CommandExecutorD3D12::Reset()
	{
		m_DescriptorHandles = {};
		m_DepthHandle		= {};

		m_CurrentlyBoundResourceSet = nullptr;

		m_CurrentFramebuffer	 = {};
		m_CurrentlyBoundPipeline = {};
	}

	void CommandExecutorD3D12::SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList)
	{
		m_CommandList = commandList;
	}

	void CommandExecutorD3D12::SetCommandQueue(CommandQueueD3D12 *commandQueue)
	{
		m_CommandQueue = commandQueue;
	}

	void CommandExecutorD3D12::FlushReadbacks(IGraphicsDevice *device)
	{
		if (m_ReadbackCopies.size() == 0)
		{
			return;
		}

		std::vector<D3D12ReadbackBufferCopyOperation> readbackCopies = m_ReadbackCopies;
		m_ReadbackCopies.clear();

		device->WaitForIdle();

		for (const D3D12ReadbackBufferCopyOperation &readbackCopy : readbackCopies)
		{
			const uint32_t copyPerRow = std::min(readbackCopy.SourceRowPitch, readbackCopy.DestinationRowPitch);

			uint8_t *srcData = readbackCopy.ReadbackBuffer->Map();

			std::vector<uint8_t> pixels(readbackCopy.NumRows * readbackCopy.DestinationRowPitch, 0);

			for (uint32_t y = 0; y < readbackCopy.NumRows; y++)
			{
				const uint8_t *srcRow = srcData + y * readbackCopy.SourceRowPitch;
				uint8_t		  *dstRow = pixels.data() + y * readbackCopy.DestinationRowPitch;
				memcpy(dstRow, srcRow, readbackCopy.DestinationRowPitch);
			}

			memcpy(srcData, pixels.data(), pixels.size());

			readbackCopy.ReadbackBuffer->Unmap();
		}

		device->WaitForIdle();
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

	void CommandExecutorD3D12::ExecuteCommand(WeakRef<IPipeline> command, IGraphicsDevice *device)
	{
		Ref<IPipeline> pipeline = std::dynamic_pointer_cast<IPipeline>(command.lock());

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

		NX_VALIDATE(command.IndirectBuffer->CheckUsage(Graphics::BufferUsage_Indirect), "Buffer passed to DrawIndirect is not an indirect buffer");

		if (m_CurrentlyBoundPipeline.value()->GetType() == PipelineType::Graphics)
		{
			if (DeviceBufferD3D12 *indirectBuffer = dynamic_cast<DeviceBufferD3D12 *>(command.IndirectBuffer))
			{
				Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

				Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
					GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DRAW, command.Stride);

				m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
			}
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentFramebuffer))
		{
			return;
		}

		NX_VALIDATE(command.IndirectBuffer->CheckUsage(Graphics::BufferUsage_Indirect), "Buffer passed to DrawIndirect is not an indirect buffer");

		if (m_CurrentlyBoundPipeline.value()->GetType() == PipelineType::Graphics)
		{
			if (DeviceBufferD3D12 *indirectBuffer = dynamic_cast<DeviceBufferD3D12 *>(command.IndirectBuffer))
			{
				Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

				Nexus::Ref<Nexus::Graphics::GraphicsPipelineD3D12> pipeline =
					std::dynamic_pointer_cast<GraphicsPipelineD3D12>(m_CurrentlyBoundPipeline.value());

				Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
					GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED, command.Stride);

				m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
			}
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

		if (DeviceBufferD3D12 *indirectBuffer = dynamic_cast<DeviceBufferD3D12 *>(command.IndirectBuffer))
		{
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

		{
			if (DeviceBufferD3D12 *indirectBuffer = dynamic_cast<DeviceBufferD3D12 *>(command.IndirectBuffer))
			{
				Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

				Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature =
					GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH, command.Stride);

				m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
			}
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device)
	{
		Nexus::Graphics::PipelineType pipelineType = m_CurrentlyBoundPipeline.value()->GetType();

		ResourceSetHandle	 handle			  = desc.TargetResourceSet;
		ResourceSetD3D12	*d3d12ResourceSet = handle.AsDerived<ResourceSetD3D12>();
		GraphicsDeviceD3D12 *deviceD3D12	  = (GraphicsDeviceD3D12 *)device;

		if (d3d12ResourceSet)
		{
			m_CurrentlyBoundResourceSet = d3d12ResourceSet;

			bool isGraphics = pipelineType != PipelineType::Compute;
			d3d12ResourceSet->Bind(isGraphics, m_CommandList, desc.DynamicOffsets);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device)
	{
		if (!ValidateForClearColour(m_CurrentFramebuffer, command.Index))
		{
			return;
		}

		float clearColor[] = {command.Colour.Red, command.Colour.Green, command.Colour.Blue, command.Colour.Alpha};

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

	void CommandExecutorD3D12::ExecuteCommand(FramebufferHandle command, IGraphicsDevice *device)
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
		if (const TextureD3D12 *const source = command.Source.AsDerived<const TextureD3D12>())
		{
			uint32_t sourceSubresource = Utils::CalculateSubresource(command.SourceMipLevel, command.SourceArrayLayer, source->GetMipLevels());
			Microsoft::WRL::ComPtr<ID3D12Resource2> sourceHandle = source->GetHandle();

			if (const TextureD3D12 *dest = command.Destination.AsDerived<const TextureD3D12>())
			{
				uint32_t destinationSubresource =
					Utils::CalculateSubresource(command.DestinationMipLevel, command.DestinationArrayLayer, dest->GetMipLevels());
				Microsoft::WRL::ComPtr<ID3D12Resource2> destHandle = dest->GetHandle();

				PixelFormat destFormat = dest->GetPixelFormat();

				m_CommandList->ResolveSubresource(destHandle.Get(),
												  destinationSubresource,
												  sourceHandle.Get(),
												  sourceSubresource,
												  D3D12::GetD3D12PixelFormat(destFormat));
			}
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device)
	{
		if (const TimingQueryD3D12 *queryD3D12 = command.Query.AsDerived<const TimingQueryD3D12>())
		{
			Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap = queryD3D12->GetQueryHeap();
			m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device)
	{
		if (const TimingQueryD3D12 *queryD3D12 = command.Query.AsDerived<const TimingQueryD3D12>())
		{
			Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap = queryD3D12->GetQueryHeap();
			m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device)
	{
		DeviceBufferD3D12 *source = dynamic_cast<DeviceBufferD3D12 *>(command.BufferCopy.Source);
		DeviceBufferD3D12 *dest	  = dynamic_cast<DeviceBufferD3D12 *>(command.BufferCopy.Destination);

		if (source && dest)
		{
			for (const auto &copy : command.BufferCopy.Copies)
			{
				m_CommandList->CopyBufferRegion(dest->GetHandle().Get(), copy.WriteOffset, source->GetHandle().Get(), copy.ReadOffset, copy.Size);
			}
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device)
	{
		GraphicsDeviceD3D12					 *deviceD3D12  = (GraphicsDeviceD3D12 *)device;
		Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

		const TextureD3D12 *texture = command.BufferTextureCopy.TextureHandle.AsDerived<const TextureD3D12>();

		if (!texture)
		{
			return;
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> textureHandle = texture->GetHandle();

		const bool layeredTexture	= texture->IsLayeredTexture();
		uint32_t   subresourceIndex = Utils::CalculateSubresource(command.BufferTextureCopy.MipLevel,
																  layeredTexture ? command.BufferTextureCopy.TextureOffset.Z : 0,
																  command.BufferTextureCopy.TextureHandle->GetMipLevels());

		D3D12_BOX textureBounds = {};
		textureBounds.left		= command.BufferTextureCopy.TextureOffset.X;
		textureBounds.right		= command.BufferTextureCopy.TextureOffset.X + command.BufferTextureCopy.TextureExtent.Width;
		textureBounds.top		= command.BufferTextureCopy.TextureOffset.Y;
		textureBounds.bottom	= command.BufferTextureCopy.TextureOffset.Y + command.BufferTextureCopy.TextureExtent.Height;

		if (texture->GetType() == TextureType::Texture3D)
		{
			textureBounds.front = command.BufferTextureCopy.TextureOffset.Z;
			textureBounds.back	= command.BufferTextureCopy.TextureOffset.Z + 1;
		}
		else
		{
			textureBounds.front = 0;
			textureBounds.back	= 1;
		}

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint	  = {};
		UINT							   numRows		  = {};
		UINT64							   rowSizeInBytes = {};
		UINT64							   totalBytes	  = {};

		D3D12_RESOURCE_DESC resourceDesc = textureHandle->GetDesc();
		nativeDevice->GetCopyableFootprints(&resourceDesc,
											subresourceIndex,
											1,
											command.BufferTextureCopy.BufferOffset,
											&footprint,
											&numRows,
											&rowSizeInBytes,
											&totalBytes);

		Ref<IDeviceBuffer>					   stagingBuffer	  = CreateStagingBuffer(totalBytes, true, device);
		DeviceBufferD3D12					  *sourceBufferD3D12  = dynamic_cast<DeviceBufferD3D12 *>(command.BufferTextureCopy.BufferHandle);
		Ref<DeviceBufferD3D12>				   stagingBufferD3D12 = std::dynamic_pointer_cast<DeviceBufferD3D12>(stagingBuffer);
		Microsoft::WRL::ComPtr<ID3D12Resource> bufferHandle		  = stagingBufferD3D12->GetHandle();

		uint8_t *sourceData	 = sourceBufferD3D12->Map();
		uint8_t *stagingData = stagingBuffer->Map();

		if (!texture || !sourceBufferD3D12 || !stagingBufferD3D12)
		{
			throw std::runtime_error("Invalid cast in ExecuteCommand");
		}

		uint32_t rowSize = command.BufferTextureCopy.TextureExtent.Width * GetPixelFormatSizeInBytes(texture->GetPixelFormat());

		for (uint32_t y = 0; y < command.BufferTextureCopy.TextureExtent.Height; y++)
		{
			uint8_t *dstRow = stagingData + y * footprint.Footprint.RowPitch;
			uint8_t *srcRow = sourceData + y * rowSize;

			memcpy(dstRow, srcRow, rowSize);

			// Optional: clear padding to avoid garbage
			if (rowSize < footprint.Footprint.RowPitch)
			{
				memset(dstRow + rowSize, 0, footprint.Footprint.RowPitch - rowSize);
			}
		}

		sourceBufferD3D12->Unmap();
		stagingBuffer->Unmap();

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource					= bufferHandle.Get();
		srcLocation.Type						= D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint				= footprint;

		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource					= textureHandle.Get();
		dstLocation.Type						= D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex			= subresourceIndex;

		const uint32_t zOffset = texture->GetType() == TextureType::Texture3D ? command.BufferTextureCopy.TextureOffset.Z : 0;

		m_CommandList->CopyTextureRegion(&dstLocation,
										 command.BufferTextureCopy.TextureOffset.X,
										 command.BufferTextureCopy.TextureOffset.Y,
										 zOffset,
										 &srcLocation,
										 &textureBounds);
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device)
	{
		GraphicsDeviceD3D12					 *deviceD3D12  = (GraphicsDeviceD3D12 *)device;
		Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

		DeviceBufferD3D12  *buffer	= dynamic_cast<DeviceBufferD3D12 *>(command.TextureBufferCopy.BufferHandle);
		const TextureD3D12 *texture = command.TextureBufferCopy.TextureHandle.AsDerived<const TextureD3D12>();

		if (!texture)
		{
			return;
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> textureHandle = texture->GetHandle();

		bool arrayedTexture = command.TextureBufferCopy.TextureHandle->GetType() == TextureType::Texture3D ||
							  command.TextureBufferCopy.TextureHandle->GetType() == TextureType::TextureCube;
		uint32_t subresourceIndex = Utils::CalculateSubresource(command.TextureBufferCopy.MipLevel,
																arrayedTexture ? command.TextureBufferCopy.TextureOffset.Z : 0,
																command.TextureBufferCopy.TextureHandle->GetMipLevels());

		D3D12_BOX textureBounds = {};
		textureBounds.left		= command.TextureBufferCopy.TextureOffset.X;
		textureBounds.right		= command.TextureBufferCopy.TextureOffset.X + command.TextureBufferCopy.TextureExtent.Width;
		textureBounds.top		= command.TextureBufferCopy.TextureOffset.Y;
		textureBounds.bottom	= command.TextureBufferCopy.TextureOffset.Y + command.TextureBufferCopy.TextureExtent.Height;
		textureBounds.front		= command.TextureBufferCopy.TextureOffset.Z;
		textureBounds.back		= command.TextureBufferCopy.TextureOffset.Z + 1;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint	  = {};
		UINT							   numRows		  = {};
		UINT64							   rowSizeInBytes = {};
		UINT64							   totalBytes	  = {};

		D3D12_RESOURCE_DESC resourceDesc = textureHandle->GetDesc();
		nativeDevice->GetCopyableFootprints(&resourceDesc,
											subresourceIndex,
											1,
											command.TextureBufferCopy.BufferOffset,
											&footprint,
											&numRows,
											&rowSizeInBytes,
											&totalBytes);

		Microsoft::WRL::ComPtr<ID3D12Resource> bufferHandle = buffer->GetHandle();

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource					= textureHandle.Get();
		srcLocation.Type						= D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLocation.SubresourceIndex			= subresourceIndex;

		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource					= bufferHandle.Get();
		dstLocation.Type						= D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dstLocation.PlacedFootprint				= footprint;

		// copy texture data into the buffer (the 0's are for the offset into the destination texture, which we do not need here)
		m_CommandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, &textureBounds);

		size_t tightPitch = command.TextureBufferCopy.TextureExtent.Width * GetPixelFormatSizeInBytes(texture->GetDescription().Format);

		D3D12ReadbackBufferCopyOperation &readbackOperation = m_ReadbackCopies.emplace_back();
		readbackOperation.ReadbackBuffer					= buffer;
		readbackOperation.SourceRowPitch					= footprint.Footprint.RowPitch;
		readbackOperation.DestinationRowPitch				= tightPitch;
		readbackOperation.Height							= command.TextureBufferCopy.TextureExtent.Height;
		readbackOperation.NumRows							= numRows;
	}

	void CommandExecutorD3D12::ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device)
	{
		GraphicsDeviceD3D12					 *deviceD3D12  = (GraphicsDeviceD3D12 *)device;
		Microsoft::WRL::ComPtr<ID3D12Device9> nativeDevice = deviceD3D12->GetD3D12Device();

		const TextureD3D12 *srcTexture = command.TextureCopy.Source.AsDerived<const TextureD3D12>();
		const TextureD3D12 *dstTexture = command.TextureCopy.Destination.AsDerived<const TextureD3D12>();

		Microsoft::WRL::ComPtr<ID3D12Resource2> srcHandle = srcTexture->GetHandle();
		Microsoft::WRL::ComPtr<ID3D12Resource2> dstHandle = dstTexture->GetHandle();

		// retrieve source index
		bool	 srcArrayedTexture	 = srcTexture->GetType() == TextureType::Texture3D || srcTexture->GetType() == TextureType::TextureCube;
		uint32_t srcSubresourceIndex = Utils::CalculateSubresource(command.TextureCopy.SourceMipLevel,
																   srcArrayedTexture ? command.TextureCopy.SourceOffset.Z : 0,
																   command.TextureCopy.Source->GetMipLevels());

		// retrieve destination footprint
		bool	 dstArrayedTexture	 = dstTexture->GetType() == TextureType::Texture3D || dstTexture->GetType() == TextureType::TextureCube;
		uint32_t dstSubresourceIndex = Utils::CalculateSubresource(command.TextureCopy.DestinationMipLevel,
																   dstArrayedTexture ? command.TextureCopy.DestinationOffset.Z : 0,
																   command.TextureCopy.Destination->GetMipLevels());

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource					= srcHandle.Get();
		srcLocation.Type						= D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLocation.SubresourceIndex			= srcSubresourceIndex;

		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource					= dstHandle.Get();
		dstLocation.Type						= D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex			= dstSubresourceIndex;

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
		textureBounds.back		= command.TextureCopy.SourceOffset.Z + 1;

		m_CommandList->CopyTextureRegion(&dstLocation,
										 command.TextureCopy.DestinationOffset.X,
										 command.TextureCopy.DestinationOffset.Y,
										 command.TextureCopy.DestinationOffset.Z,
										 &srcLocation,
										 &textureBounds);
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
		float blendFactor[4] = {command.BlendFactor.Red, command.BlendFactor.Green, command.BlendFactor.Blue, command.BlendFactor.Alpha};

		m_CommandList->OMSetBlendFactor(blendFactor);
	}

	void CommandExecutorD3D12::ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device)
	{
		m_CommandList->OMSetStencilRef(command.StencilReference);
	}

	void CommandExecutorD3D12::ExecuteCommand(const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device)
	{
		for (const auto &accelerationStructureBuildDesc : command.BuildDescriptions)
		{
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS  inputs	   = {};
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
			std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>			  geometry	   = {};

			D3D12::GetD3D12AccelerationStructureInputs(accelerationStructureBuildDesc, inputs, geometry);

			D3D12_GPU_VIRTUAL_ADDRESS srcAddress	 = 0;
			D3D12_GPU_VIRTUAL_ADDRESS destAddress	 = 0;
			D3D12_GPU_VIRTUAL_ADDRESS scratchAddress = accelerationStructureBuildDesc.ScratchBuffer;

			if (accelerationStructureBuildDesc.Source.IsValid())
			{
				srcAddress = accelerationStructureBuildDesc.Source->GetDeviceAddress(0);
			}

			if (accelerationStructureBuildDesc.Destination.IsValid())
			{
				destAddress = accelerationStructureBuildDesc.Destination->GetDeviceAddress(0);
			}

			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
			buildDesc.SourceAccelerationStructureData					 = srcAddress;
			buildDesc.DestAccelerationStructureData						 = destAddress;
			buildDesc.ScratchAccelerationStructureData					 = scratchAddress;
			buildDesc.Inputs											 = inputs;
			m_CommandList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
		}
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
		if (!m_CurrentlyBoundResourceSet && !m_CurrentlyBoundPipeline.has_value())
			return;

		bool isGraphics = m_CurrentlyBoundPipeline.value()->GetType() != PipelineType::Compute;

		m_CurrentlyBoundResourceSet
			->SetPushConstants(command.Name, command.Data.data(), command.Offset, command.Data.size(), isGraphics, m_CommandList);
	}

	void CommandExecutorD3D12::ExecuteCommand(const BarrierGroupDescription &command, IGraphicsDevice *device)
	{
		GraphicsDeviceD3D12 *deviceD3D12   = (GraphicsDeviceD3D12 *)device;
		const auto			&d3d12Features = deviceD3D12->GetD3D12DeviceFeatures();

		// enhanced barriers
		if (d3d12Features.SupportsEnhancedBarriers)
		{
			std::vector<D3D12_GLOBAL_BARRIER>  globalBarriers  = {};
			std::vector<D3D12_TEXTURE_BARRIER> textureBarriers = {};
			std::vector<D3D12_BUFFER_BARRIER>  bufferBarriers  = {};

			// memory barriers
			for (const auto &memoryBarrier : command.MemoryBarriers)
			{
				D3D12_BARRIER_SYNC	 beforeSync	  = D3D12::GetBarrierSync(memoryBarrier.BeforeStage);
				D3D12_BARRIER_SYNC	 afterSync	  = D3D12::GetBarrierSync(memoryBarrier.AfterStage);
				D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(memoryBarrier.BeforeAccess);
				D3D12_BARRIER_ACCESS afterAccess  = D3D12::GetBarrierAccess(memoryBarrier.AfterAccess);

				D3D12_GLOBAL_BARRIER &barrier = globalBarriers.emplace_back();
				barrier.SyncBefore			  = beforeSync;
				barrier.SyncAfter			  = afterSync;
				barrier.AccessBefore		  = beforeAccess;
				barrier.AccessAfter			  = afterAccess;
			}

			// texture barriers
			for (const auto &textureBarrier : command.TextureBarriers)
			{
				const TextureD3D12					   *texture = textureBarrier.Texture.AsDerived<const TextureD3D12>();
				Microsoft::WRL::ComPtr<ID3D12Resource2> handle	= texture->GetHandle();

				D3D12_BARRIER_SYNC	 beforeSync	  = D3D12::GetBarrierSync(textureBarrier.BeforeStage);
				D3D12_BARRIER_SYNC	 afterSync	  = D3D12::GetBarrierSync(textureBarrier.AfterStage);
				D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(textureBarrier.BeforeAccess);
				D3D12_BARRIER_ACCESS afterAccess  = D3D12::GetBarrierAccess(textureBarrier.AfterAccess);

				D3D12_BARRIER_LAYOUT afterLayout = D3D12::GetBarrierLayout(textureBarrier.Layout);

				for (uint32_t arrayLayer = textureBarrier.TextureSubresourceRange.BaseArrayLayer;
					 arrayLayer < textureBarrier.TextureSubresourceRange.BaseArrayLayer + textureBarrier.TextureSubresourceRange.LayerCount;
					 arrayLayer++)
				{
					for (uint32_t mipLevel = textureBarrier.TextureSubresourceRange.BaseMipLevel;
						 mipLevel < textureBarrier.TextureSubresourceRange.BaseMipLevel + textureBarrier.TextureSubresourceRange.LevelCount;
						 mipLevel++)
					{
						D3D12_BARRIER_LAYOUT beforeLayout = D3D12::GetBarrierLayout(texture->GetTextureLayout(arrayLayer, mipLevel));

						if (beforeLayout == afterLayout)
						{
							continue;
						}

						D3D12_TEXTURE_BARRIER &barrier			  = textureBarriers.emplace_back();
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
			}

			// buffer barriers
			for (const auto &bufferBarrier : command.BufferBarriers)
			{
				Ref<DeviceBufferD3D12>					buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(bufferBarrier.Buffer);
				Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

				D3D12_BARRIER_SYNC	 beforeSync	  = D3D12::GetBarrierSync(bufferBarrier.BeforeStage);
				D3D12_BARRIER_SYNC	 afterSync	  = D3D12::GetBarrierSync(bufferBarrier.AfterStage);
				D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(bufferBarrier.BeforeAccess);
				D3D12_BARRIER_ACCESS afterAccess  = D3D12::GetBarrierAccess(bufferBarrier.AfterAccess);

				D3D12_BUFFER_BARRIER barrier = bufferBarriers.emplace_back();
				barrier.SyncBefore			 = beforeSync;
				barrier.SyncAfter			 = afterSync;
				barrier.AccessBefore		 = beforeAccess;
				barrier.AccessAfter			 = afterAccess;
				barrier.pResource			 = handle.Get();

				// these have to be hardcoded to these values as transitioning part of a buffer is not currently supported
				barrier.Offset = 0;
				barrier.Size   = UINT64_MAX;
			}

			// create the barriers to submit
			std::vector<D3D12_BARRIER_GROUP> barrierGroups = {};
			if (globalBarriers.size() > 0)
			{
				D3D12_BARRIER_GROUP &globalBarrierGroup = barrierGroups.emplace_back();
				globalBarrierGroup.Type					= D3D12_BARRIER_TYPE_GLOBAL;
				globalBarrierGroup.NumBarriers			= static_cast<UINT32>(globalBarriers.size());
				globalBarrierGroup.pGlobalBarriers		= globalBarriers.data();
			}

			if (textureBarriers.size() > 0)
			{
				D3D12_BARRIER_GROUP &textureBarrierGroup = barrierGroups.emplace_back();
				textureBarrierGroup.Type				 = D3D12_BARRIER_TYPE_TEXTURE;
				textureBarrierGroup.NumBarriers			 = static_cast<UINT32>(textureBarriers.size());
				textureBarrierGroup.pTextureBarriers	 = textureBarriers.data();
			}

			if (bufferBarriers.size() > 0)
			{
				D3D12_BARRIER_GROUP &bufferBarrierGroup = barrierGroups.emplace_back();
				bufferBarrierGroup.Type					= D3D12_BARRIER_TYPE_BUFFER;
				bufferBarrierGroup.NumBarriers			= static_cast<UINT32>(bufferBarriers.size());
				bufferBarrierGroup.pBufferBarriers		= bufferBarriers.data();
			}

			m_CommandList->Barrier(static_cast<UINT32>(barrierGroups.size()), barrierGroups.data());
		}
		// resource barriers
		else
		{
			std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers = {};

			for (const auto &textureBarrier : command.TextureBarriers)
			{
				const TextureD3D12					   *texture = textureBarrier.Texture.AsDerived<const TextureD3D12>();
				Microsoft::WRL::ComPtr<ID3D12Resource2> handle	= texture->GetHandle();

				for (uint32_t arrayLayer = textureBarrier.TextureSubresourceRange.BaseArrayLayer;
					 arrayLayer < textureBarrier.TextureSubresourceRange.BaseArrayLayer + textureBarrier.TextureSubresourceRange.LayerCount;
					 arrayLayer++)
				{
					for (uint32_t mipLevel = textureBarrier.TextureSubresourceRange.BaseMipLevel;
						 mipLevel < textureBarrier.TextureSubresourceRange.BaseMipLevel + textureBarrier.TextureSubresourceRange.LevelCount;
						 mipLevel++)
					{
						D3D12_RESOURCE_STATES beforeState = D3D12::GetTextureResourceState(texture->GetTextureLayout(arrayLayer, mipLevel));
						D3D12_RESOURCE_STATES afterState  = D3D12::GetTextureResourceState(textureBarrier.Layout);

						if (beforeState == afterState)
						{
							continue;
						}

						uint32_t subresourceIndex = Utils::CalculateSubresource(mipLevel, arrayLayer, texture->GetDescription().MipLevels);

						D3D12_RESOURCE_BARRIER &barrier = resourceBarriers.emplace_back();
						barrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
						barrier.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
						barrier.Transition.pResource	= handle.Get();
						barrier.Transition.Subresource	= subresourceIndex;
						barrier.Transition.StateBefore	= beforeState;
						barrier.Transition.StateAfter	= afterState;
					}
				}
			}

			for (const auto &bufferBarrier : command.BufferBarriers)
			{
				if (bufferBarrier.BeforeAccess == BarrierAccess::ShaderWrite)
				{
					Ref<DeviceBufferD3D12>					buffer = std::dynamic_pointer_cast<DeviceBufferD3D12>(bufferBarrier.Buffer);
					Microsoft::WRL::ComPtr<ID3D12Resource2> handle = buffer->GetHandle();

					D3D12_RESOURCE_BARRIER barrier = resourceBarriers.emplace_back();
					barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_UAV;
					barrier.UAV.pResource		   = handle.Get();
					barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				}
			}

			m_CommandList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), resourceBarriers.data());
		}

		// update texture layouts
		// enumerate through all texture barriers and create the required subresource ranges
		for (const TextureBarrierDesc &textureBarrier : command.TextureBarriers)
		{
			TextureHandle textureHandle = textureBarrier.Texture;
			TextureD3D12 *textureD3D12	= textureHandle.AsDerived<TextureD3D12>();

			for (uint32_t arrayLayer = textureBarrier.TextureSubresourceRange.BaseArrayLayer;
				 arrayLayer < textureBarrier.TextureSubresourceRange.BaseArrayLayer + textureBarrier.TextureSubresourceRange.LayerCount;
				 arrayLayer++)
			{
				for (uint32_t mipLevel = textureBarrier.TextureSubresourceRange.BaseMipLevel;
					 mipLevel < textureBarrier.TextureSubresourceRange.BaseMipLevel + textureBarrier.TextureSubresourceRange.LevelCount;
					 mipLevel++)
				{
					textureD3D12->SetTextureLayout(arrayLayer, mipLevel, textureBarrier.Layout);
				}
			}
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(const TraceRaysDescription &desc, IGraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device)
	{
		FramebufferHandle framebuffer = command.TargetFramebuffer;

		if (framebuffer->GetSampleCount() > 1)
		{
			for (size_t textureIndex = 0; textureIndex < framebuffer->GetColorTextureCount(); textureIndex++)
			{
				std::optional<FramebufferColourAttachmentDescription> colourAttachmentDescOpt = framebuffer->GetColorTextureBinding(textureIndex);
				if (colourAttachmentDescOpt.has_value())
				{
					FramebufferColourAttachmentDescription colourAttachmentDesc = colourAttachmentDescOpt.value();
					if (colourAttachmentDesc.ResolveAttachment.has_value())
					{
						for (uint32_t layerIndex = 0; layerIndex < colourAttachmentDesc.ColourAttachment.LayerCount; layerIndex++)
						{
							FramebufferTextureDescription resolveAttachmentDesc = colourAttachmentDesc.ResolveAttachment.value();

							uint32_t sourceArrayIndex = colourAttachmentDesc.ColourAttachment.BaseArrayLayer + layerIndex;
							uint32_t destArrayIndex	  = resolveAttachmentDesc.BaseArrayLayer + layerIndex;

							TextureLayout sourceLayout =
								colourAttachmentDesc.ColourAttachment.TargetTexture->GetTextureLayout(sourceArrayIndex,
																									  colourAttachmentDesc.ColourAttachment.MipLevel);
							TextureLayout destLayout =
								resolveAttachmentDesc.TargetTexture->GetTextureLayout(destArrayIndex, resolveAttachmentDesc.MipLevel);

							BarrierGroupDescription barrierGroup = {};

							// to resolve compatible layouts
							{
								TextureBarrierDesc sourceBarrier	  = {};
								sourceBarrier.Texture				  = colourAttachmentDesc.ColourAttachment.TargetTexture;
								sourceBarrier.BeforeAccess			  = BarrierAccess::ColourAttachmentWrite;
								sourceBarrier.AfterAccess			  = BarrierAccess::ColourAttachmentRead;
								sourceBarrier.BeforeStage			  = BarrierPipelineStage::ColourAttachmentOutput;
								sourceBarrier.AfterStage			  = BarrierPipelineStage::Resolve;
								sourceBarrier.Layout				  = TextureLayout::ResolveSrc;
								sourceBarrier.TextureSubresourceRange = {.BaseMipLevel	 = colourAttachmentDesc.ColourAttachment.MipLevel,
																		 .LevelCount	 = 1,
																		 .BaseArrayLayer = sourceArrayIndex,
																		 .LayerCount	 = 1};
								barrierGroup.TextureBarriers.emplace_back(sourceBarrier);

								TextureBarrierDesc destBarrier		= {};
								destBarrier.Texture					= resolveAttachmentDesc.TargetTexture;
								destBarrier.BeforeAccess			= BarrierAccess::ColourAttachmentWrite;
								destBarrier.AfterAccess				= BarrierAccess::ColourAttachmentRead;
								destBarrier.BeforeStage				= BarrierPipelineStage::ColourAttachmentOutput;
								destBarrier.AfterStage				= BarrierPipelineStage::Resolve;
								destBarrier.Layout					= TextureLayout::ResolveDest;
								destBarrier.TextureSubresourceRange = {.BaseMipLevel   = resolveAttachmentDesc.MipLevel,
																	   .LevelCount	   = 1,
																	   .BaseArrayLayer = destArrayIndex,
																	   .LayerCount	   = 1};
								barrierGroup.TextureBarriers.emplace_back(destBarrier);

								ExecuteCommand(barrierGroup, device);
							}

							// execute resolve
							{
								ResolveTextureDescription resolveDesc = {};
								resolveDesc.Source					  = colourAttachmentDesc.ColourAttachment.TargetTexture;
								resolveDesc.SourceMipLevel			  = colourAttachmentDesc.ColourAttachment.MipLevel;
								resolveDesc.SourceArrayLayer		  = sourceArrayIndex;
								resolveDesc.Destination				  = resolveAttachmentDesc.TargetTexture;
								resolveDesc.DestinationMipLevel		  = resolveAttachmentDesc.MipLevel;
								resolveDesc.DestinationArrayLayer	  = destArrayIndex;

								ExecuteCommand(resolveDesc, device);
							}

							// to resolve compatible layouts
							{
								BarrierGroupDescription barrierGroup = {};

								TextureBarrierDesc sourceBarrier	  = {};
								sourceBarrier.Texture				  = colourAttachmentDesc.ColourAttachment.TargetTexture;
								sourceBarrier.BeforeAccess			  = BarrierAccess::TransferRead;
								sourceBarrier.AfterAccess			  = BarrierAccess::NoAccess;
								sourceBarrier.BeforeStage			  = BarrierPipelineStage::Resolve;
								sourceBarrier.AfterStage			  = BarrierPipelineStage::NoStage;
								sourceBarrier.Layout				  = sourceLayout;
								sourceBarrier.TextureSubresourceRange = {.BaseMipLevel	 = colourAttachmentDesc.ColourAttachment.MipLevel,
																		 .LevelCount	 = 1,
																		 .BaseArrayLayer = sourceArrayIndex,
																		 .LayerCount	 = 1};
								barrierGroup.TextureBarriers.emplace_back(sourceBarrier);

								TextureBarrierDesc destBarrier		= {};
								destBarrier.Texture					= resolveAttachmentDesc.TargetTexture;
								destBarrier.BeforeAccess			= BarrierAccess::TransferRead;
								destBarrier.AfterAccess				= BarrierAccess::NoAccess;
								destBarrier.BeforeStage				= BarrierPipelineStage::Resolve;
								destBarrier.AfterStage				= BarrierPipelineStage::NoStage;
								destBarrier.Layout					= destLayout;
								destBarrier.TextureSubresourceRange = {.BaseMipLevel   = resolveAttachmentDesc.MipLevel,
																	   .LevelCount	   = 1,
																	   .BaseArrayLayer = destArrayIndex,
																	   .LayerCount	   = 1};
								barrierGroup.TextureBarriers.emplace_back(destBarrier);
							}
						}
					}
				}
			}
		}
	}

	void CommandExecutorD3D12::SetFramebuffer(FramebufferHandle framebuffer, IGraphicsDevice *device)
	{
		ResetPreviousRenderTargets(device);
		GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

		if (framebuffer.IsValid())
		{
			FramebufferD3D12 *framebufferD3D12 = framebuffer.AsDerived<FramebufferD3D12>();

			m_DescriptorHandles = framebufferD3D12->GetColourAttachmentCPUHandles();
			m_DepthHandle		= framebufferD3D12->GetDepthAttachmentCPUHandle();

			if (!framebufferD3D12->HasDepthTexture())
			{
				m_DepthHandle = {};
			}

			if (framebufferD3D12->HasDepthTexture())
			{
				m_CommandList->OMSetRenderTargets(m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, &m_DepthHandle);
			}
			else
			{
				m_CommandList->OMSetRenderTargets(m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, nullptr);
			}

			m_CurrentFramebuffer = framebuffer;
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
		TextureHandle							textureHandle = command.Texture;
		TextureD3D12						   *texture		  = textureHandle.AsDerived<TextureD3D12>();
		Microsoft::WRL::ComPtr<ID3D12Resource2> handle		  = texture->GetHandle();
		std::vector<D3D12_RESOURCE_BARRIER>		barriers	  = {};

		for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
			 arrayLayer < command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
			 arrayLayer++)
		{
			for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
				 mipLevel < command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
				 mipLevel++)
			{
				D3D12_RESOURCE_STATES beforeState = D3D12::GetTextureResourceState(texture->GetTextureLayout(arrayLayer, mipLevel));
				D3D12_RESOURCE_STATES afterState  = D3D12::GetTextureResourceState(command.Layout);

				if (beforeState == afterState)
				{
					continue;
				}

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

		if (barriers.size() > 0)
		{
			m_CommandList->ResourceBarrier(barriers.size(), barriers.data());
		}

		for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
			 arrayLayer < command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
			 arrayLayer++)
		{
			for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
				 mipLevel < command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
				 mipLevel++)
			{
				texture->SetTextureLayout(arrayLayer, mipLevel, command.Layout);
			}
		}
	}

	void CommandExecutorD3D12::InsertTextureBarrier(const TextureBarrierDesc &command)
	{
		TextureHandle							textureHandle = command.Texture;
		TextureD3D12						   *texture		  = textureHandle.AsDerived<TextureD3D12>();
		Microsoft::WRL::ComPtr<ID3D12Resource2> handle		  = texture->GetHandle();

		bool		  transitionEachSubresourceSeparately = false;
		TextureLayout testLayout						  = texture->GetTextureLayout(0, 0);

		if (command.TextureSubresourceRange.LayerCount == texture->GetDescription().DepthOrArrayLayers &&
			command.TextureSubresourceRange.LevelCount == texture->GetDescription().MipLevels)
		{
			for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
				 arrayLayer < command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
				 arrayLayer++)
			{
				for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
					 mipLevel < command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
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

			// we do not need to try to insert a barrier if the layout is already correct
			if (beforeLayout == afterLayout)
			{
				return;
			}

			D3D12_BARRIER_SYNC	 beforeSync	  = D3D12::GetBarrierSync(command.BeforeStage);
			D3D12_BARRIER_SYNC	 afterSync	  = D3D12::GetBarrierSync(command.AfterStage);
			D3D12_BARRIER_ACCESS beforeAccess = D3D12::GetBarrierAccess(command.BeforeAccess);
			D3D12_BARRIER_ACCESS afterAccess  = D3D12::GetBarrierAccess(command.AfterAccess);

			// if this is the first time using the resource, we need to hardcode the before access value
			if (beforeLayout == D3D12_BARRIER_LAYOUT_UNDEFINED)
			{
				beforeAccess = D3D12_BARRIER_ACCESS_NO_ACCESS;
			}

			D3D12_TEXTURE_BARRIER &barrier			  = barriers.emplace_back();
			barrier.SyncBefore						  = beforeSync;
			barrier.SyncAfter						  = afterSync;
			barrier.AccessBefore					  = beforeAccess;
			barrier.AccessAfter						  = afterAccess;
			barrier.LayoutBefore					  = beforeLayout;
			barrier.LayoutAfter						  = afterLayout;
			barrier.pResource						  = handle.Get();
			barrier.Subresources.FirstArraySlice	  = command.TextureSubresourceRange.BaseArrayLayer;
			barrier.Subresources.NumArraySlices		  = command.TextureSubresourceRange.LayerCount;
			barrier.Subresources.IndexOrFirstMipLevel = command.TextureSubresourceRange.BaseMipLevel;
			barrier.Subresources.NumMipLevels		  = command.TextureSubresourceRange.LevelCount;
			barrier.Subresources.FirstPlane			  = 0;
			barrier.Subresources.NumPlanes			  = 1;
		}
		else
		{
			for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
				 arrayLayer < command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
				 arrayLayer++)
			{
				for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
					 mipLevel < command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
					 mipLevel++)
				{
					D3D12_BARRIER_LAYOUT beforeLayout = D3D12::GetBarrierLayout(testLayout);
					D3D12_BARRIER_LAYOUT afterLayout  = D3D12::GetBarrierLayout(command.Layout);

					// we do not need to try to insert a barrier if the layout is already correct
					if (beforeLayout == afterLayout)
					{
						continue;
					}

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
		}

		D3D12_BARRIER_GROUP barrierGroup = {};
		barrierGroup.Type				 = D3D12_BARRIER_TYPE_TEXTURE;
		barrierGroup.NumBarriers		 = barriers.size();
		barrierGroup.pTextureBarriers	 = barriers.data();

		m_CommandList->Barrier(1, &barrierGroup);

		for (uint32_t arrayLayer = command.TextureSubresourceRange.BaseArrayLayer;
			 arrayLayer < command.TextureSubresourceRange.BaseArrayLayer + command.TextureSubresourceRange.LayerCount;
			 arrayLayer++)
		{
			for (uint32_t mipLevel = command.TextureSubresourceRange.BaseMipLevel;
				 mipLevel < command.TextureSubresourceRange.BaseMipLevel + command.TextureSubresourceRange.LevelCount;
				 mipLevel++)
			{
				texture->SetTextureLayout(arrayLayer, mipLevel, command.Layout);
			}
		}
	}

	Ref<IDeviceBuffer> CommandExecutorD3D12::CreateStagingBuffer(size_t size, bool upload, IGraphicsDevice *device)
	{
		Ref<IDeviceBuffer> &buffer = m_UploadBuffers.emplace_back();

		DeviceBufferDescription description = {};
		upload ? description.Access = BufferMemoryAccess::Upload : description.Access = BufferMemoryAccess::Readback;
		description.SizeInBytes	  = size;
		description.StrideInBytes = size;
		description.DebugName	  = "Staging Buffer";
		buffer					  = device->CreateDeviceBuffer(description);

		return buffer;
	}
}	 // namespace Nexus::Graphics

#endif