#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Graphics/CommandExecutor.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"

namespace Nexus::Graphics
{
	struct SetVertexBufferCommandStorage
	{
		size_t	 HandleIndex = 0;
		size_t	 Offset		 = 0;
		size_t	 Size		 = 0;
		uint32_t Slot		 = 0;
	};

	struct SetIndexBufferCommandStorage
	{
		size_t		HandleIndex = 0;
		size_t		Offset		= 0;
		size_t		Size		= 0;
		IndexFormat Format		= IndexFormat::UInt32;
	};

	struct BufferCopyStorage
	{
		size_t	 SrcHandleIndex	 = 0;
		size_t	 DestHandleIndex = 0;
		uint64_t ReadOffset		 = 0;
		uint64_t WriteOffset	 = 0;
		uint64_t Size			 = 0;
	};

	struct BufferTextureCopyStorage
	{
		size_t				   BufferHandleIndex  = 0;
		size_t				   TextureHandleIndex = 0;
		uint64_t			   BufferOffset		  = 0;
		SubresourceDescription TextureSubresource;
	};

	struct TextureCopyStorage
	{
		size_t				   SrcHandleIndex  = 0;
		size_t				   DestHandleIndex = 0;
		SubresourceDescription SrcSubresource  = {};
		SubresourceDescription DestSubresource = {};
	};

	static void DispatchSetVertexBufferCmd(void						 *data,
										   std::vector<Ref<void>>	 &handles,
										   std::vector<RenderTarget> &renderTargets,
										   GraphicsDevice			 *device,
										   CommandExecutor			 *executor)
	{
		SetVertexBufferCommandStorage *storage = (SetVertexBufferCommandStorage *)data;

		SetVertexBufferCommand command = {};
		command.View.BufferHandle	   = std::static_pointer_cast<DeviceBuffer>(handles[storage->HandleIndex]);
		command.View.Offset			   = storage->Offset;
		command.View.Size			   = storage->Size;
		command.Slot				   = storage->Slot;

		executor->ExecuteCommand(command, device);
	}

	static void DispatchSetIndexBufferCmd(void						*data,
										  std::vector<Ref<void>>	&handles,
										  std::vector<RenderTarget> &renderTargets,
										  GraphicsDevice			*device,
										  CommandExecutor			*executor)
	{
		SetIndexBufferCommandStorage *storage = (SetIndexBufferCommandStorage *)data;

		SetIndexBufferCommand command = {};
		command.View.BufferHandle	  = std::static_pointer_cast<DeviceBuffer>(handles[storage->HandleIndex]);
		command.View.Offset			  = storage->Offset;
		command.View.Size			  = storage->Size;
		command.View.BufferFormat	  = storage->Format;

		executor->ExecuteCommand(command, device);
	}

	static void DispatchSetPipelineCmd(void						 *data,
									   std::vector<Ref<void>>	 &handles,
									   std::vector<RenderTarget> &renderTargets,
									   GraphicsDevice			 *device,
									   CommandExecutor			 *executor)
	{
		size_t		 *handleIndex = (size_t *)data;
		Ref<Pipeline> pipeline	  = std::static_pointer_cast<Pipeline>(handles[*handleIndex]);

		SetPipelineCommand command = {.PipelineToBind = pipeline};
		executor->ExecuteCommand(command, device);
	}

	static void DispatchDrawCmd(void					  *data,
								std::vector<Ref<void>>	  &handles,
								std::vector<RenderTarget> &renderTargets,
								GraphicsDevice			  *device,
								CommandExecutor			  *executor)
	{
		DrawDescription *drawDesc = (DrawDescription *)data;
		executor->ExecuteCommand(*drawDesc, device);
	}

	static void DispatchDrawIndexedCmd(void						 *data,
									   std::vector<Ref<void>>	 &handles,
									   std::vector<RenderTarget> &renderTargets,
									   GraphicsDevice			 *device,
									   CommandExecutor			 *executor)
	{
		DrawIndexedDescription *drawDesc = (DrawIndexedDescription *)data;
		executor->ExecuteCommand(*drawDesc, device);
	}

	static void DispatchDrawIndirectCmd(void					  *data,
										std::vector<Ref<void>>	  &handles,
										std::vector<RenderTarget> &renderTargets,
										GraphicsDevice			  *device,
										CommandExecutor			  *executor)
	{
	}

	static void DispatchDrawIndirectIndexedCmd(void						 *data,
											   std::vector<Ref<void>>	 &handles,
											   std::vector<RenderTarget> &renderTargets,
											   GraphicsDevice			 *device,
											   CommandExecutor			 *executor)
	{
	}

	static void DispatchDispatchCmd(void					  *data,
									std::vector<Ref<void>>	  &handles,
									std::vector<RenderTarget> &renderTargets,
									GraphicsDevice			  *device,
									CommandExecutor			  *executor)
	{
	}

	static void DispatchDispatchIndirectCmd(void					  *data,
											std::vector<Ref<void>>	  &handles,
											std::vector<RenderTarget> &renderTargets,
											GraphicsDevice			  *device,
											CommandExecutor			  *executor)
	{
	}

	static void DispatchDrawMeshCmd(void					  *data,
									std::vector<Ref<void>>	  &handles,
									std::vector<RenderTarget> &renderTargets,
									GraphicsDevice			  *device,
									CommandExecutor			  *executor)
	{
	}

	static void DispatchDrawMeshIndirectCmd(void					  *data,
											std::vector<Ref<void>>	  &handles,
											std::vector<RenderTarget> &renderTargets,
											GraphicsDevice			  *device,
											CommandExecutor			  *executor)
	{
	}

	static void DispatchSetResourceSetCmd(void						*data,
										  std::vector<Ref<void>>	&handles,
										  std::vector<RenderTarget> &renderTargets,
										  GraphicsDevice			*device,
										  CommandExecutor			*executor)
	{
		size_t			*handleIndex = (size_t *)data;
		Ref<ResourceSet> resourceSet = std::static_pointer_cast<ResourceSet>(handles[*handleIndex]);

		executor->ExecuteCommand(resourceSet, device);
	}

	static void DispatchClearColourTargetCmd(void					   *data,
											 std::vector<Ref<void>>	   &handles,
											 std::vector<RenderTarget> &renderTargets,
											 GraphicsDevice			   *device,
											 CommandExecutor		   *executor)
	{
		ClearColourTargetCommand *command = (ClearColourTargetCommand *)data;
		executor->ExecuteCommand(*command, device);
	}

	static void DispatchClearDepthStencilTargetCmd(void						 *data,
												   std::vector<Ref<void>>	 &handles,
												   std::vector<RenderTarget> &renderTargets,
												   GraphicsDevice			 *device,
												   CommandExecutor			 *executor)
	{
		ClearDepthStencilTargetCommand *command = (ClearDepthStencilTargetCommand *)data;
		executor->ExecuteCommand(*command, device);
	}

	static void DispatchSetRenderTargetCmd(void						 *data,
										   std::vector<Ref<void>>	 &handles,
										   std::vector<RenderTarget> &renderTargets,
										   GraphicsDevice			 *device,
										   CommandExecutor			 *executor)
	{
		size_t		*renderTargetIndex = (size_t *)data;
		RenderTarget target			   = (RenderTarget)renderTargets[*renderTargetIndex];
		executor->ExecuteCommand(target, device);
	}

	static void DispatchSetViewportCmd(void						 *data,
									   std::vector<Ref<void>>	 &handles,
									   std::vector<RenderTarget> &renderTargets,
									   GraphicsDevice			 *device,
									   CommandExecutor			 *executor)
	{
		Viewport *viewport = (Viewport *)data;
		executor->ExecuteCommand(*viewport, device);
	}

	static void DispatchSetScissorCmd(void						*data,
									  std::vector<Ref<void>>	&handles,
									  std::vector<RenderTarget> &renderTargets,
									  GraphicsDevice			*device,
									  CommandExecutor			*executor)
	{
		Scissor *scissor = (Scissor *)data;
		executor->ExecuteCommand(*scissor, device);
	}

	static void DispatchResolveCmd(void						 *data,
								   std::vector<Ref<void>>	 &handles,
								   std::vector<RenderTarget> &renderTargets,
								   GraphicsDevice			 *device,
								   CommandExecutor			 *executor)
	{
	}

	static void DispatchStartTimingQueryCmd(void					  *data,
											std::vector<Ref<void>>	  &handles,
											std::vector<RenderTarget> &renderTargets,
											GraphicsDevice			  *device,
											CommandExecutor			  *executor)
	{
	}

	static void DispatchStopTimingQueryCmd(void						 *data,
										   std::vector<Ref<void>>	 &handles,
										   std::vector<RenderTarget> &renderTargets,
										   GraphicsDevice			 *device,
										   CommandExecutor			 *executor)
	{
	}

	static void DispatchCopyBufferToBufferCmd(void						*data,
											  std::vector<Ref<void>>	&handles,
											  std::vector<RenderTarget> &renderTargets,
											  GraphicsDevice			*device,
											  CommandExecutor			*executor)
	{
		BufferCopyStorage *storage = (BufferCopyStorage *)data;
		Ref<DeviceBuffer>  src	   = std::static_pointer_cast<DeviceBuffer>(handles[storage->SrcHandleIndex]);
		Ref<DeviceBuffer>  dst	   = std::static_pointer_cast<DeviceBuffer>(handles[storage->DestHandleIndex]);

		CopyBufferToBufferCommand command = {};
		command.BufferCopy.Source		  = src;
		command.BufferCopy.Destination	  = dst;
		command.BufferCopy.ReadOffset	  = storage->ReadOffset;
		command.BufferCopy.WriteOffset	  = storage->WriteOffset;
		command.BufferCopy.Size			  = storage->Size;
		executor->ExecuteCommand(command, device);
	}

	static void DispatchCopyBufferToTextureCmd(void						 *data,
											   std::vector<Ref<void>>	 &handles,
											   std::vector<RenderTarget> &renderTargets,
											   GraphicsDevice			 *device,
											   CommandExecutor			 *executor)
	{
		BufferTextureCopyStorage *storage = (BufferTextureCopyStorage *)data;
		Ref<DeviceBuffer>		  buffer  = std::static_pointer_cast<DeviceBuffer>(handles[storage->BufferHandleIndex]);
		Ref<Texture>			  texture = std::static_pointer_cast<Texture>(handles[storage->TextureHandleIndex]);

		CopyBufferToTextureCommand command			 = {};
		command.BufferTextureCopy.BufferHandle		 = buffer;
		command.BufferTextureCopy.TextureHandle		 = texture;
		command.BufferTextureCopy.BufferOffset		 = storage->BufferOffset;
		command.BufferTextureCopy.TextureSubresource = storage->TextureSubresource;

		executor->ExecuteCommand(command, device);
	}

	static void DispatchCopyTextureToBufferCmd(void						 *data,
											   std::vector<Ref<void>>	 &handles,
											   std::vector<RenderTarget> &renderTargets,
											   GraphicsDevice			 *device,
											   CommandExecutor			 *executor)
	{
		BufferTextureCopyStorage *storage = (BufferTextureCopyStorage *)data;
		Ref<DeviceBuffer>		  buffer  = std::static_pointer_cast<DeviceBuffer>(handles[storage->BufferHandleIndex]);
		Ref<Texture>			  texture = std::static_pointer_cast<Texture>(handles[storage->TextureHandleIndex]);

		CopyTextureToBufferCommand command			 = {};
		command.TextureBufferCopy.BufferHandle		 = buffer;
		command.TextureBufferCopy.TextureHandle		 = texture;
		command.TextureBufferCopy.BufferOffset		 = storage->BufferOffset;
		command.TextureBufferCopy.TextureSubresource = storage->TextureSubresource;

		executor->ExecuteCommand(command, device);
	}

	static void DispatchCopyTextureToTextureCmd(void					  *data,
												std::vector<Ref<void>>	  &handles,
												std::vector<RenderTarget> &renderTargets,
												GraphicsDevice			  *device,
												CommandExecutor			  *executor)
	{
		TextureCopyStorage *storage = (TextureCopyStorage *)data;
		Ref<Texture>		source	= std::static_pointer_cast<Texture>(handles[storage->SrcHandleIndex]);
		Ref<Texture>		dest	= std::static_pointer_cast<Texture>(handles[storage->DestHandleIndex]);

		CopyTextureToTextureCommand command = {};
		command.TextureCopy.Source			= source;
		command.TextureCopy.Destination		= dest;
		command.TextureCopy.SourceSubresource = storage->SrcSubresource;
		command.TextureCopy.DestinationSubresource = storage->DestSubresource;

		executor->ExecuteCommand(command, device);
	}

	static void DispatchBeginDebugGroupCmd(void						 *data,
										   std::vector<Ref<void>>	 &handles,
										   std::vector<RenderTarget> &renderTargets,
										   GraphicsDevice			 *device,
										   CommandExecutor			 *executor)
	{
	}

	static void DispatchEndDebugGroupCmd(void					   *data,
										 std::vector<Ref<void>>	   &handles,
										 std::vector<RenderTarget> &renderTargets,
										 GraphicsDevice			   *device,
										 CommandExecutor		   *executor)
	{
	}

	static void DispatchInsertDebugMarkerCmd(void					   *data,
											 std::vector<Ref<void>>	   &handles,
											 std::vector<RenderTarget> &renderTargets,
											 GraphicsDevice			   *device,
											 CommandExecutor		   *executor)
	{
	}

	static void DispatchSetBlendFactorCmd(void						*data,
										  std::vector<Ref<void>>	&handles,
										  std::vector<RenderTarget> &renderTargets,
										  GraphicsDevice			*device,
										  CommandExecutor			*executor)
	{
	}

	static void DispatchSetStencilReferenceCmd(void						 *data,
											   std::vector<Ref<void>>	 &handles,
											   std::vector<RenderTarget> &renderTargets,
											   GraphicsDevice			 *device,
											   CommandExecutor			 *executor)
	{
	}

	static void DispatchBuildAccelerationStructuresCmd(void						 *data,
													   std::vector<Ref<void>>	 &handles,
													   std::vector<RenderTarget> &renderTargets,
													   GraphicsDevice			 *device,
													   CommandExecutor			 *executor)
	{
	}

	static void DispatchAccelerationStructureCopyCmd(void					   *data,
													 std::vector<Ref<void>>	   &handles,
													 std::vector<RenderTarget> &renderTargets,
													 GraphicsDevice			   *device,
													 CommandExecutor		   *executor)
	{
	}

	static void DispatchAccelerationStructureDeviceBufferCopyCmd(void					   *data,
																 std::vector<Ref<void>>	   &handles,
																 std::vector<RenderTarget> &renderTargets,
																 GraphicsDevice			   *device,
																 CommandExecutor		   *executor)
	{
	}

	static void DispatchDeviceBufferAccelerationStructureCopyCmd(void					   *data,
																 std::vector<Ref<void>>	   &handles,
																 std::vector<RenderTarget> &renderTargets,
																 GraphicsDevice			   *device,
																 CommandExecutor		   *executor)
	{
	}

	static void DispatchPushConstantsCmd(void					   *data,
										 std::vector<Ref<void>>	   &handles,
										 std::vector<RenderTarget> &renderTargets,
										 GraphicsDevice			   *device,
										 CommandExecutor		   *executor)
	{
	}

	using CommandFn = void (*)(void *, std::vector<Ref<void>> &, std::vector<RenderTarget> &, GraphicsDevice *, CommandExecutor *);

	CommandFn c_DispatchTable[] = {DispatchSetVertexBufferCmd,
								   DispatchSetIndexBufferCmd,
								   DispatchSetPipelineCmd,
								   DispatchDrawCmd,
								   DispatchDrawIndexedCmd,
								   DispatchDrawIndirectCmd,
								   DispatchDrawIndirectIndexedCmd,
								   DispatchDispatchCmd,
								   DispatchDispatchIndirectCmd,
								   DispatchDrawMeshCmd,
								   DispatchDrawMeshIndirectCmd,
								   DispatchSetResourceSetCmd,
								   DispatchClearColourTargetCmd,
								   DispatchClearDepthStencilTargetCmd,
								   DispatchSetRenderTargetCmd,
								   DispatchSetViewportCmd,
								   DispatchSetScissorCmd,
								   DispatchResolveCmd,
								   DispatchStartTimingQueryCmd,
								   DispatchStopTimingQueryCmd,
								   DispatchCopyBufferToBufferCmd,
								   DispatchCopyBufferToTextureCmd,
								   DispatchCopyTextureToBufferCmd,
								   DispatchCopyTextureToTextureCmd,
								   DispatchBeginDebugGroupCmd,
								   DispatchEndDebugGroupCmd,
								   DispatchInsertDebugMarkerCmd,
								   DispatchSetBlendFactorCmd,
								   DispatchSetStencilReferenceCmd,
								   DispatchBuildAccelerationStructuresCmd,
								   DispatchAccelerationStructureCopyCmd,
								   DispatchAccelerationStructureDeviceBufferCopyCmd,
								   DispatchDeviceBufferAccelerationStructureCopyCmd,
								   DispatchPushConstantsCmd};

	CommandList::CommandList(const CommandListDescription &spec) : m_Description(spec)
	{
	}

	void CommandList::Begin()
	{
		NX_PROFILE_FUNCTION();

		if (m_Started)
		{
			NX_ERROR("Attempting to begin a command into a CommandList that has not "
					 "been closed");
		}

		m_Started	  = true;
		m_DebugGroups = 0;
		m_CommandBuffer.Reset();
	}

	void CommandList::End()
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to end a CommandList but the CommandList was not begun");
		}

		// clean up any unclosed debug groups
		for (uint32_t i = 0; i < m_DebugGroups; i++) { EndDebugGroup(); }

		// reset the debug group counter
		m_DebugGroups = 0;

		// end recording into the CommandList
		m_Started = false;
	}

	void CommandList::SetVertexBuffer(VertexBufferView vertexBuffer, uint32_t slot)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.SetVertexBuffer(vertexBuffer, slot);
	}

	void CommandList::SetIndexBuffer(IndexBufferView indexBuffer)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.SetIndexBuffer(indexBuffer);
	}

	void CommandList::SetPipeline(Ref<Pipeline> pipeline)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.SetPipeline(pipeline);
	}

	void CommandList::Draw(const DrawDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.Draw(desc);
	}

	void CommandList::DrawIndexed(const DrawIndexedDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.DrawIndexed(desc);
	}

	void CommandList::DrawIndirect(const DrawIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.DrawIndirect(desc);
	}

	void CommandList::DrawIndexedIndirect(const DrawIndirectIndexedDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.DrawIndexedIndirect(desc);
	}

	void CommandList::Dispatch(const DispatchDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.Dispatch(desc);
	}

	void CommandList::DispatchIndirect(const DispatchIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.DispatchIndirect(desc);
	}

	void CommandList::DrawMesh(const DrawMeshDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.DrawMesh(desc);
	}

	void CommandList::DrawMeshIndirect(const DrawMeshIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.DrawMeshIndirect(desc);
	}

	void CommandList::SetResourceSet(Ref<ResourceSet> resources)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.SetResourceSet(resources);
	}

	void CommandList::ClearColourTarget(uint32_t index, const ClearColorValue &color, ClearRect clearRect)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.ClearColourTarget(index, color, clearRect);
	}

	void CommandList::ClearColourTarget(uint32_t index, const ClearColorValue &color)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.ClearColourTarget(index, color);
	}

	void CommandList::ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.ClearDepthTarget(value, clearRect);
	}

	void CommandList::ClearDepthTarget(const ClearDepthStencilValue &value)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.ClearDepthTarget(value);
	}

	void CommandList::SetRenderTarget(RenderTarget target)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.SetRenderTarget(target);
	}

	void CommandList::SetViewport(const Viewport &viewport)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.SetViewport(viewport);
	}

	void CommandList::SetScissor(const Scissor &scissor)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.SetScissor(scissor);
	}

	void CommandList::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Ref<Swapchain> target)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.ResolveFramebuffer(source, sourceIndex, target);
	}

	void Nexus::Graphics::CommandList::StartTimingQuery(Ref<TimingQuery> query)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.StartTimingQuery(query);
	}

	void Nexus::Graphics::CommandList::StopTimingQuery(Ref<TimingQuery> query)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.StopTimingQuery(query);
	}

	void CommandList::CopyBufferToBuffer(const BufferCopyDescription &bufferCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.CopyBufferToBuffer(bufferCopy);
	}

	void CommandList::CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.CopyBufferToTexture(bufferTextureCopy);
	}

	void CommandList::CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.CopyTextureToBuffer(textureBufferCopy);
	}

	void CommandList::CopyTextureToTexture(const TextureCopyDescription &textureCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.CopyTextureToTexture(textureCopy);
	}

	void CommandList::BeginDebugGroup(const std::string &name)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.BeginDebugGroup(name);
		m_DebugGroups++;
	}

	void CommandList::EndDebugGroup()
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.EndDebugGroup();
		m_DebugGroups--;
	}

	void CommandList::InsertDebugMarker(const std::string &name)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_CommandBuffer.InsertDebugMarker(name);
	}

	void CommandList::SetBlendFactor(const BlendFactorDesc &blendFactor)
	{
		NX_PROFILE_FUNCTION();
		m_CommandBuffer.SetBlendFactor(blendFactor);
	}

	void CommandList::SetStencilReference(uint32_t stencilReference)
	{
		NX_PROFILE_FUNCTION();
		m_CommandBuffer.SetStencilReference(stencilReference);
	}

	void CommandList::BuildAccelerationStructures(const std::vector<AccelerationStructureBuildDescription> &description)
	{
		NX_PROFILE_FUNCTION();
		m_CommandBuffer.BuildAccelerationStructures(description);
	}

	void CommandList::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();
		m_CommandBuffer.CopyAccelerationStructure(description);
	}

	void CommandList::CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();
		m_CommandBuffer.CopyAccelerationStructureToDeviceBuffer(description);
	}

	void CommandList::CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();
		m_CommandBuffer.CopyDeviceBufferToAccelerationStructure(description);
	}

	void CommandList::WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset)
	{
		NX_PROFILE_FUNCTION();

		/*PushConstantsDesc pushConstantDesc = {};
		pushConstantDesc.Name			   = name;
		pushConstantDesc.Offset			   = offset;
		pushConstantDesc.Data.resize(size);
		memcpy(pushConstantDesc.Data.data(), data, size);*/

		m_CommandBuffer.WritePushConstants(name, data, size, offset);
	}

	const CommandListDescription &CommandList::GetDescription()
	{
		NX_PROFILE_FUNCTION();

		return m_Description;
	}

	CommandBuffer &CommandList::GetCommandBuffer()
	{
		return m_CommandBuffer;
	}

	const CommandBuffer &CommandList::GetCommandBuffer() const
	{
		return m_CommandBuffer;
	}

	bool CommandList::IsRecording() const
	{
		NX_PROFILE_FUNCTION();

		return m_Started;
	}

	void CommandBuffer::Reset()
	{
		m_CommandData.clear();
		m_Handles.clear();
	}

	void CommandBuffer::SetVertexBuffer(const VertexBufferView &vertexBuffer, uint32_t slot)
	{
		SetVertexBufferCommandStorage storage = {};
		storage.Offset						  = vertexBuffer.Offset;
		storage.Size						  = vertexBuffer.Size;
		storage.Slot						  = slot;
		storage.HandleIndex					  = m_Handles.size();
		AddCommand(CommandType::SetVertexBuffer, storage);

		m_Handles.push_back(vertexBuffer.BufferHandle);
	}

	void CommandBuffer::SetIndexBuffer(IndexBufferView indexBuffer)
	{
		SetIndexBufferCommandStorage storage = {};
		storage.Offset						 = indexBuffer.Offset;
		storage.Size						 = indexBuffer.Size;
		storage.Format						 = indexBuffer.BufferFormat;
		storage.HandleIndex					 = m_Handles.size();
		AddCommand(CommandType::SetIndexBuffer, storage);

		m_Handles.push_back(indexBuffer.BufferHandle);
	}

	void CommandBuffer::SetPipeline(Ref<Pipeline> pipeline)
	{
		size_t handleIndex = m_Handles.size();
		AddCommand(CommandType::SetPipeline, handleIndex);

		m_Handles.push_back(pipeline);
	}

	void CommandBuffer::Draw(const DrawDescription &desc)
	{
		AddCommand(CommandType::Draw, desc);
	}

	void CommandBuffer::DrawIndexed(const DrawIndexedDescription &desc)
	{
		AddCommand(CommandType::DrawIndexed, desc);
	}

	void CommandBuffer::DrawIndirect(const DrawIndirectDescription &desc)
	{
	}

	void CommandBuffer::DrawIndexedIndirect(const DrawIndirectIndexedDescription &desc)
	{
	}

	void CommandBuffer::Dispatch(const DispatchDescription &desc)
	{
	}

	void CommandBuffer::DispatchIndirect(const DispatchIndirectDescription &desc)
	{
	}

	void CommandBuffer::DrawMesh(const DrawMeshDescription &desc)
	{
	}

	void CommandBuffer::DrawMeshIndirect(const DrawMeshIndirectDescription &desc)
	{
	}

	void CommandBuffer::SetResourceSet(Ref<ResourceSet> resources)
	{
		size_t handleIndex = m_Handles.size();
		AddCommand(CommandType::SetResourceSet, handleIndex);

		m_Handles.push_back(resources);
	}

	void CommandBuffer::ClearColourTarget(uint32_t index, const ClearColorValue &color, ClearRect clearRect)
	{
		ClearColourTargetCommand command = {};
		command.Index					 = index;
		command.Color					 = color;
		command.Rect					 = clearRect;
		AddCommand(CommandType::ClearColourTarget, command);
	}

	void CommandBuffer::ClearColourTarget(uint32_t index, const ClearColorValue &color)
	{
		ClearColourTargetCommand command = {};
		command.Index					 = index;
		command.Color					 = color;
		command.Rect					 = {0, 0, 0, 0};
		AddCommand(CommandType::ClearColourTarget, command);
	}

	void CommandBuffer::ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect)
	{
		ClearDepthStencilTargetCommand command = {};
		command.Value						   = value;
		command.Rect						   = clearRect;
		AddCommand(CommandType::ClearDepthStencilTarget, command);
	}

	void CommandBuffer::ClearDepthTarget(const ClearDepthStencilValue &value)
	{
		ClearDepthStencilTargetCommand command = {};
		command.Value						   = value;
		command.Rect						   = {0, 0, 0, 0};
		AddCommand(CommandType::ClearDepthStencilTarget, command);
	}

	void CommandBuffer::SetRenderTarget(RenderTarget target)
	{
		size_t renderTargetIndex = m_RenderTargets.size();
		AddCommand(CommandType::SetRenderTarget, renderTargetIndex);

		m_RenderTargets.push_back(target);
	}

	void CommandBuffer::SetViewport(const Viewport &viewport)
	{
		AddCommand(CommandType::SetViewport, viewport);
	}

	void CommandBuffer::SetScissor(const Scissor &scissor)
	{
		AddCommand(CommandType::SetScissor, scissor);
	}

	void CommandBuffer::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Ref<Swapchain> target)
	{
	}

	void CommandBuffer::StartTimingQuery(Ref<TimingQuery> query)
	{
	}

	void CommandBuffer::StopTimingQuery(Ref<TimingQuery> query)
	{
	}

	void CommandBuffer::CopyBufferToBuffer(const BufferCopyDescription &bufferCopy)
	{
		BufferCopyStorage storage = {};
		storage.SrcHandleIndex	  = m_Handles.size();
		storage.DestHandleIndex	  = m_Handles.size() + 1;
		storage.ReadOffset		  = bufferCopy.ReadOffset;
		storage.WriteOffset		  = bufferCopy.WriteOffset;
		storage.Size			  = bufferCopy.Size;
		AddCommand(CommandType::CopyBufferToBuffer, storage);

		m_Handles.push_back(bufferCopy.Source);
		m_Handles.push_back(bufferCopy.Destination);
	}

	void CommandBuffer::CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy)
	{
		BufferTextureCopyStorage storage = {};
		storage.BufferHandleIndex		 = m_Handles.size();
		storage.TextureHandleIndex		 = m_Handles.size() + 1;
		storage.BufferOffset			 = bufferTextureCopy.BufferOffset;
		storage.TextureSubresource		 = bufferTextureCopy.TextureSubresource;
		AddCommand(CommandType::CopyBufferToTexture, storage);

		m_Handles.push_back(bufferTextureCopy.BufferHandle);
		m_Handles.push_back(bufferTextureCopy.TextureHandle);
	}

	void CommandBuffer::CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy)
	{
		BufferTextureCopyStorage storage = {};
		storage.BufferHandleIndex		 = m_Handles.size();
		storage.TextureHandleIndex		 = m_Handles.size() + 1;
		storage.BufferOffset			 = textureBufferCopy.BufferOffset;
		storage.TextureSubresource		 = textureBufferCopy.TextureSubresource;
		AddCommand(CommandType::CopyTextureToBuffer, storage);

		m_Handles.push_back(textureBufferCopy.BufferHandle);
		m_Handles.push_back(textureBufferCopy.TextureHandle);
	}

	void CommandBuffer::CopyTextureToTexture(const TextureCopyDescription &textureCopy)
	{
		TextureCopyStorage storage = {};
		storage.SrcHandleIndex	   = m_Handles.size();
		storage.DestHandleIndex	   = m_Handles.size() + 1;
		storage.SrcSubresource	   = textureCopy.SourceSubresource;
		storage.DestSubresource	   = textureCopy.DestinationSubresource;
		AddCommand(CommandType::CopyTextureToTexture, storage);

		m_Handles.push_back(textureCopy.Source);
		m_Handles.push_back(textureCopy.Destination);
	}

	void CommandBuffer::BeginDebugGroup(const std::string &name)
	{
	}

	void CommandBuffer::EndDebugGroup()
	{
	}

	void CommandBuffer::InsertDebugMarker(const std::string &name)
	{
	}

	void CommandBuffer::SetBlendFactor(const BlendFactorDesc &blendFactor)
	{
	}

	void CommandBuffer::SetStencilReference(uint32_t stencilReference)
	{
	}

	void CommandBuffer::BuildAccelerationStructures(const std::vector<AccelerationStructureBuildDescription> &description)
	{
	}

	void CommandBuffer::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
	{
	}

	void CommandBuffer::CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description)
	{
	}

	void CommandBuffer::CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description)
	{
	}

	void CommandBuffer::WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset)
	{
	}

	void CommandBuffer::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		uint8_t *ptr = m_CommandData.data();
		uint8_t *end = ptr + m_CommandData.size();

		while (ptr < end)
		{
			CommandHeader *header = reinterpret_cast<CommandHeader *>(ptr);
			ptr += sizeof(CommandHeader);

			void *payload = ptr;

			c_DispatchTable[(size_t)header->Type](payload, m_Handles, m_RenderTargets, device, executor);

			ptr += header->Size;
		}
	}
}	 // namespace Nexus::Graphics
