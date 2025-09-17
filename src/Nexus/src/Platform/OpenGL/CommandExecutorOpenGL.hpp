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
		void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device);
		void Reset();

		void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device);
		void ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device);
		void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device);
		void ExecuteCommand(DrawDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawIndexedDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawIndirectDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawIndirectIndexedDescription command, GraphicsDevice *device);
		void ExecuteCommand(DispatchDescription command, GraphicsDevice *device);
		void ExecuteCommand(DispatchIndirectDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawMeshDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawMeshIndirectDescription command, GraphicsDevice *device);
		void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device);
		void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device);
		void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device);
		void ExecuteCommand(RenderTarget command, GraphicsDevice *device);
		void ExecuteCommand(const Viewport &command, GraphicsDevice *device);
		void ExecuteCommand(const Scissor &command, GraphicsDevice *device);
		void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device);
		void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device);
		void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device);
		void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device);
		void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device);
		void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device);
		void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device);
		void ExecuteCommand(BeginDebugGroupCommand command, GraphicsDevice *device);
		void ExecuteCommand(EndDebugGroupCommand command, GraphicsDevice *device);
		void ExecuteCommand(InsertDebugMarkerCommand command, GraphicsDevice *device);
		void ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device);
		void ExecuteCommand(SetStencilReferenceCommand command, GraphicsDevice *device);
		void ExecuteCommand(BuildAccelerationStructuresCommand command, GraphicsDevice *device);
		void ExecuteCommand(AccelerationStructureCopyDescription command, GraphicsDevice *Device);
		void ExecuteCommand(AccelerationStructureDeviceBufferCopyDescription command, GraphicsDevice *device);
		void ExecuteCommand(DeviceBufferAccelerationStructureCopyDescription command, GraphicsDevice *device);
		void ExecuteCommand(const PushConstantsDesc &command, GraphicsDevice *device);

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