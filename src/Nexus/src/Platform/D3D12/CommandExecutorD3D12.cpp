#if defined(NX_PLATFORM_D3D12)

	#include "CommandExecutorD3D12.hpp"

	#include "DeviceBufferD3D12.hpp"
	#include "FramebufferD3D12.hpp"
	#include "PipelineD3D12.hpp"
	#include "ResourceSetD3D12.hpp"
	#include "SwapchainD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TimingQueryD3D12.hpp"

namespace Nexus::Graphics
{
	CommandExecutorD3D12::CommandExecutorD3D12()
	{
	}

	CommandExecutorD3D12::~CommandExecutorD3D12()
	{
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

	void CommandExecutorD3D12::SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> commandList)
	{
		m_CommandList = commandList;
	}

	void CommandExecutorD3D12::ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		Ref<GraphicsPipelineD3D12> pipeline			 = m_CurrentlyBoundPipeline.value();
		DeviceBufferD3D12		  *d3d12VertexBuffer = (DeviceBufferD3D12 *)command.View.BufferHandle;
		const auto				  &bufferLayout		 = pipeline->GetPipelineDescription().Layouts.at(command.Slot);

		D3D12_VERTEX_BUFFER_VIEW bufferView;
		bufferView.BufferLocation = d3d12VertexBuffer->GetHandle()->GetGPUVirtualAddress() + command.View.Offset;
		bufferView.SizeInBytes	  = d3d12VertexBuffer->GetDescription().SizeInBytes - command.View.Offset;
		bufferView.StrideInBytes  = bufferLayout.GetStride();

		m_CommandList->IASetVertexBuffers(command.Slot, 1, &bufferView);
	}

	void CommandExecutorD3D12::ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		DeviceBufferD3D12 *d3d12IndexBuffer = (DeviceBufferD3D12 *)command.View.BufferHandle;

		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		indexBufferView.BufferLocation = d3d12IndexBuffer->GetHandle()->GetGPUVirtualAddress() + command.View.Offset;
		indexBufferView.SizeInBytes	   = d3d12IndexBuffer->GetDescription().SizeInBytes - command.View.Offset;
		indexBufferView.Format		   = D3D12::GetD3D12IndexBufferFormat(command.View.BufferFormat);

		m_CommandList->IASetIndexBuffer(&indexBufferView);
	}

	void CommandExecutorD3D12::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
	{
		Ref<GraphicsPipelineD3D12> d3d12Pipeline = std::dynamic_pointer_cast<GraphicsPipelineD3D12>(command.lock());
		const auto		  &description	 = d3d12Pipeline->GetPipelineDescription();

		m_CommandList->OMSetDepthBounds(description.DepthStencilDesc.MinDepth, description.DepthStencilDesc.MaxDepth);
		m_CommandList->SetPipelineState(d3d12Pipeline->GetPipelineState());
		m_CommandList->SetGraphicsRootSignature(d3d12Pipeline->GetRootSignature());
		m_CommandList->IASetPrimitiveTopology(d3d12Pipeline->GetD3DPrimitiveTopology());

		m_CurrentlyBoundPipeline = d3d12Pipeline;
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_CommandList->DrawInstanced(command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device)
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

	void CommandExecutorD3D12::ExecuteCommand(DrawIndirectCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		NX_ASSERT(command.IndirectBuffer->GetDescription().Type == DeviceBufferType::Indirect,
				  "Buffer passed to DrawIndirect is not an indirect buffer");

		DeviceBufferD3D12					   *indirectBuffer		 = (DeviceBufferD3D12 *)command.IndirectBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

		Nexus::Ref<Nexus::Graphics::GraphicsPipelineD3D12> pipeline	 = m_CurrentlyBoundPipeline.value();
		Microsoft::WRL::ComPtr<ID3D12CommandSignature>	   signature = pipeline->GetDrawIndirectCommandSignature();

		m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
	}

	void CommandExecutorD3D12::ExecuteCommand(DrawIndirectIndexedCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		NX_ASSERT(command.IndirectBuffer->GetDescription().Type == DeviceBufferType::Indirect,
				  "Buffer passed to DrawIndirect is not an indirect buffer");

		DeviceBufferD3D12					   *indirectBuffer		 = (DeviceBufferD3D12 *)command.IndirectBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource2> indirectBufferHandle = indirectBuffer->GetHandle();

		Nexus::Ref<Nexus::Graphics::GraphicsPipelineD3D12> pipeline	 = m_CurrentlyBoundPipeline.value();
		Microsoft::WRL::ComPtr<ID3D12CommandSignature>	   signature = pipeline->GetDrawIndexedIndirectCommandSignature();

		m_CommandList->ExecuteIndirect(signature.Get(), command.DrawCount, indirectBufferHandle.Get(), command.Offset, nullptr, 0);
	}

	void CommandExecutorD3D12::ExecuteCommand(DispatchCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(DispatchIndirectCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorD3D12::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		Ref<ResourceSetD3D12> d3d12ResourceSet = std::dynamic_pointer_cast<ResourceSetD3D12>(command);

		std::vector<ID3D12DescriptorHeap *> heaps;
		if (d3d12ResourceSet->HasSamplerHeap())
		{
			heaps.push_back(d3d12ResourceSet->GetSamplerDescriptorHeap());
		}

		if (d3d12ResourceSet->HasConstantBufferTextureHeap())
		{
			heaps.push_back(d3d12ResourceSet->GetTextureConstantBufferDescriptorHeap());
		}

		m_CommandList->SetDescriptorHeaps(heaps.size(), heaps.data());

		uint32_t descriptorIndex = 0;

		// bind samplers
		{
			m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetSamplerStartHandle());
		}

		// bind textures/constant buffers
		{
			m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetTextureConstantBufferStartHandle());
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device)
	{
		if (!ValidateForClearColour(m_CurrentRenderTarget, command.Index))
		{
			return;
		}

		float clearColor[] = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};

		auto handle = m_DescriptorHandles[command.Index];
		m_CommandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
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

			m_CommandList->ClearDepthStencilView(m_DepthHandle, clearFlags, command.Value.Depth, command.Value.Stencil, 0, nullptr);
		}
	}

	void CommandExecutorD3D12::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
	{
		if (command.GetType() == RenderTargetType::Swapchain)
		{
			auto d3d12Swapchain = (SwapchainD3D12 *)command.GetData<Swapchain *>();
			SetSwapchain(d3d12Swapchain, device);
		}
		else
		{
			auto d3d12Framebuffer = std::dynamic_pointer_cast<FramebufferD3D12>(command.GetData<Ref<Framebuffer>>());
			SetFramebuffer(d3d12Framebuffer, device);
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

		auto framebufferD3D12 = std::dynamic_pointer_cast<FramebufferD3D12>(command.Source.lock());
		auto swapchainD3D12	  = (SwapchainD3D12 *)command.Target;

		Nexus::Ref<Nexus::Graphics::Texture2D_D3D12> framebufferTexture = framebufferD3D12->GetD3D12ColorTexture(command.SourceIndex);
		Microsoft::WRL::ComPtr<ID3D12Resource2>		 swapchainTexture	= swapchainD3D12->RetrieveBufferHandle();
		DXGI_FORMAT									 format = D3D12::GetD3D12PixelFormat(Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm, false);
		D3D12_RESOURCE_STATES						 swapchainState = swapchainD3D12->GetCurrentTextureState();

		if (framebufferD3D12->GetFramebufferSpecification().Width > swapchainD3D12->GetWindow()->GetWindowSize().X)
		{
			return;
		}

		if (framebufferD3D12->GetFramebufferSpecification().Height > swapchainD3D12->GetWindow()->GetWindowSize().Y)
		{
			return;
		}

		GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

		deviceD3D12->ResourceBarrier(m_CommandList.Get(), framebufferTexture, 0, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
		deviceD3D12->ResourceBarrier(m_CommandList.Get(),
									 swapchainTexture.Get(),
									 0,
									 swapchainD3D12->GetCurrentTextureState(),
									 D3D12_RESOURCE_STATE_RESOLVE_DEST);
		swapchainD3D12->SetTextureState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE);

		m_CommandList->ResolveSubresource(swapchainTexture.Get(), 0, framebufferTexture->GetD3D12ResourceHandle().Get(), 0, format);
	}

	void CommandExecutorD3D12::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
	{
		Ref<TimingQueryD3D12>					queryD3D12 = std::dynamic_pointer_cast<TimingQueryD3D12>(command.Query.lock());
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap	   = queryD3D12->GetQueryHeap();

		m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
	}

	void CommandExecutorD3D12::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
	{
		Ref<TimingQueryD3D12>					queryD3D12 = std::dynamic_pointer_cast<TimingQueryD3D12>(command.Query.lock());
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap	   = queryD3D12->GetQueryHeap();

		m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
	}

	void CommandExecutorD3D12::ExecuteCommand(SetStencilRefCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_CommandList->OMSetStencilRef(command.Value);
	}

	void CommandExecutorD3D12::ExecuteCommand(SetDepthBoundsCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_CommandList->OMSetDepthBounds(command.Min, command.Max);
	}

	void CommandExecutorD3D12::ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		const float blends[] = {command.R, command.G, command.B, command.A};
		m_CommandList->OMSetBlendFactor(blends);
	}

	void CommandExecutorD3D12::SetSwapchain(SwapchainD3D12 *swapchain, GraphicsDevice *device)
	{
		if (swapchain->GetSpecification().Samples == 1)
		{
			std::vector<D3D12_RESOURCE_BARRIER> barriers;
			GraphicsDeviceD3D12				   *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

			ResetPreviousRenderTargets(device);

			m_DescriptorHandles = {swapchain->RetrieveRenderTargetViewDescriptorHandle()};
			m_DepthHandle		= swapchain->RetrieveDepthBufferDescriptorHandle();

			deviceD3D12->ResourceBarrierSwapchainColour(m_CommandList.Get(), swapchain, D3D12_RESOURCE_STATE_RENDER_TARGET);
			deviceD3D12->ResourceBarrierSwapchainDepth(m_CommandList.Get(), swapchain, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		}
		else
		{
			SetFramebuffer(std::dynamic_pointer_cast<FramebufferD3D12>(swapchain->GetMultisampledFramebuffer()), device);
		}
	}

	void CommandExecutorD3D12::SetFramebuffer(Ref<FramebufferD3D12> framebuffer, GraphicsDevice *device)
	{
		ResetPreviousRenderTargets(device);
		GraphicsDeviceD3D12 *deviceD3D12 = (GraphicsDeviceD3D12 *)device;

		m_DescriptorHandles = framebuffer->GetColorAttachmentCPUHandles();
		m_DepthHandle		= framebuffer->GetDepthAttachmentCPUHandle();

		for (size_t i = 0; i < framebuffer->GetColorTextureCount(); i++)
		{
			Ref<Texture2D_D3D12> texture = framebuffer->GetD3D12ColorTexture(i);
			deviceD3D12->ResourceBarrier(m_CommandList.Get(), texture, 0, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		if (framebuffer->HasDepthTexture())
		{
			Ref<Texture2D_D3D12> depthBuffer = framebuffer->GetD3D12DepthTexture();
			deviceD3D12->ResourceBarrier(m_CommandList.Get(), depthBuffer, 0, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		}
		else
		{
			m_DepthHandle = {};
		}
	}

	void CommandExecutorD3D12::ResetPreviousRenderTargets(GraphicsDevice *device)
	{
		if (!m_CurrentRenderTarget.has_value())
		{
			return;
		}

		RenderTarget target = m_CurrentRenderTarget.value();

		if (target.GetType() == RenderTargetType::Swapchain)
		{
			auto d3d12Swapchain = (SwapchainD3D12 *)target.GetData<Swapchain *>();

			auto swapchainColourState = d3d12Swapchain->GetCurrentTextureState();
			if (swapchainColourState != D3D12_RESOURCE_STATE_PRESENT)
			{
				D3D12_RESOURCE_BARRIER presentBarrier;
				presentBarrier.Type					  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				presentBarrier.Flags				  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				presentBarrier.Transition.pResource	  = d3d12Swapchain->RetrieveBufferHandle().Get();
				presentBarrier.Transition.Subresource = 0;
				presentBarrier.Transition.StateBefore = swapchainColourState;
				presentBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
				m_CommandList->ResourceBarrier(1, &presentBarrier);

				d3d12Swapchain->SetTextureState(D3D12_RESOURCE_STATE_PRESENT);
			}
		}

		m_CurrentRenderTarget = {};
		m_DepthHandle		  = {};
	}
}	 // namespace Nexus::Graphics

#endif