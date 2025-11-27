#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "DeviceBufferOpenGL.hpp"
	#include "Nexus-Core/Graphics/CommandExecutor.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"
	#include "PipelineOpenGL.hpp"
	#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
	class CommandExecutorOpenGL final : public CommandExecutor
	{
	  public:
		CommandExecutorOpenGL() = default;
		virtual ~CommandExecutorOpenGL();
		void ExecuteCommands(Ref<ICommandList> commandList, IGraphicsDevice *device);
		void Reset();

		void ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(WeakRef<Pipeline> command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device) final;
		void ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(WeakRef<IFramebuffer> command, IGraphicsDevice *device) final;
		void ExecuteCommand(const Viewport &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const Scissor &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device) final;
		void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const MemoryBarrierDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const TextureBarrierDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const BufferBarrierDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const TraceRaysDescription &desc, IGraphicsDevice *device) final;
		void ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device) final;

		void BindResourceSet(const GladGLContext &context);
		void ExecuteGraphicsCommand(Ref<GraphicsPipelineOpenGL>																pipeline,
									const std::map<uint32_t, Nexus::Graphics::VertexBufferView>							   &vertexBuffers,
									std::optional<Nexus::Graphics::IndexBufferView>											indexBuffer,
									uint32_t																				vertexOffset,
									uint32_t																				instanceOffset,
									std::function<void(Ref<GraphicsPipelineOpenGL> pipeline, const GladGLContext &context)> drawCall);

	  private:
		std::optional<Ref<Pipeline>>				 m_CurrentlyBoundPipeline	   = {};
		Ref<IFramebuffer>							 m_CurrentRenderTarget		   = {};
		std::map<uint32_t, VertexBufferView>		 m_CurrentlyBoundVertexBuffers = {};
		std::optional<IndexBufferView>				 m_BoundIndexBuffer			   = {};
		std::optional<ResourceSetBindingDescription> m_BoundResourceSet			   = {};
	};
}	 // namespace Nexus::Graphics

#endif