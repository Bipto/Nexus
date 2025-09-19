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
		void ExecuteCommands(Ref<CommandList> commandList, GraphicsDevice *device);
		void Reset();

		void ExecuteCommand(const SetVertexBufferCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const SetIndexBufferCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndexedDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndirectDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndirectIndexedDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DispatchDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DispatchIndirectDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawMeshDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawMeshIndirectDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) final;
		void ExecuteCommand(const ClearColorTargetCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const ClearDepthStencilTargetCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(RenderTarget command, GraphicsDevice *device) final;
		void ExecuteCommand(const Viewport &command, GraphicsDevice *device) final;
		void ExecuteCommand(const Scissor &command, GraphicsDevice *device) final;
		void ExecuteCommand(const ResolveSamplesToSwapchainCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const StartTimingQueryCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const StopTimingQueryCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const BeginDebugGroupCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const EndDebugGroupCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const InsertDebugMarkerCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const SetBlendFactorCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const SetStencilReferenceCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const BuildAccelerationStructuresCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const AccelerationStructureCopyDescription &command, GraphicsDevice *Device) final;
		void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const PushConstantsDesc &command, GraphicsDevice *device) final;
		void ExecuteCommand(const MemoryBarrierDesc &command, GraphicsDevice *device) final;
		void ExecuteCommand(const TextureBarrierDesc &comamnd, GraphicsDevice *device) final;
		void ExecuteCommand(const BufferBarrierDesc &command, GraphicsDevice *device) final;

		void BindResourceSet(Ref<ResourceSetOpenGL> resourceSet, const GladGLContext &context);
		void ExecuteGraphicsCommand(Ref<GraphicsPipelineOpenGL>																pipeline,
									const std::map<uint32_t, Nexus::Graphics::VertexBufferView>							   &vertexBuffers,
									std::optional<Nexus::Graphics::IndexBufferView>											indexBuffer,
									uint32_t																				vertexOffset,
									uint32_t																				instanceOffset,
									std::function<void(Ref<GraphicsPipelineOpenGL> pipeline, const GladGLContext &context)> drawCall);

	  private:
		std::optional<Ref<Pipeline>>				   m_CurrentlyBoundPipeline		 = {};
		std::optional<RenderTarget>					   m_CurrentRenderTarget		 = {};
		std::map<uint32_t, VertexBufferView>		   m_CurrentlyBoundVertexBuffers = {};
		std::optional<IndexBufferView>				   m_BoundIndexBuffer			 = {};
		Nexus::Ref<Nexus::Graphics::ResourceSetOpenGL> m_BoundResourceSet			 = {};
	};
}	 // namespace Nexus::Graphics

#endif