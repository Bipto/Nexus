#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "DeviceBufferOpenGL.hpp"
	#include "Nexus-Core/Graphics/CommandExecutor.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"
	#include "PipelineOpenGL.hpp"
	#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
	class CommandExecutorOpenGL : public CommandExecutor
	{
	  public:
		CommandExecutorOpenGL() = default;
		virtual ~CommandExecutorOpenGL();
		virtual void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device) override;
		virtual void Reset() override;

	  private:
		virtual void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndexedDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndirectDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndirectIndexedDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DispatchDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DispatchIndirectDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawMeshDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawMeshIndirectDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(BeginDebugGroupCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(EndDebugGroupCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(InsertDebugMarkerCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device) override;

		void BindResourceSet(Ref<ResourceSetOpenGL> resourceSet, const GladGLContext &context);
		void ExecuteGraphicsCommand(Ref<GraphicsPipelineOpenGL>																pipeline,
									const std::map<uint32_t, Nexus::Graphics::VertexBufferView>							   &vertexBuffers,
									std::optional<Nexus::Graphics::IndexBufferView>											indexBuffer,
									uint32_t																				vertexOffset,
									uint32_t																				instanceOffset,
									std::function<void(Ref<GraphicsPipelineOpenGL> pipeline, const GladGLContext &context)> drawCall);

	  private:
		std::optional<Ref<Pipeline>>											m_CurrentlyBoundPipeline	  = {};
		std::optional<RenderTarget>												m_CurrentRenderTarget		  = {};
		std::map<uint32_t, VertexBufferView>									m_CurrentlyBoundVertexBuffers = {};
		std::optional<IndexBufferView>											m_BoundIndexBuffer			  = {};
		Nexus::Ref<Nexus::Graphics::ResourceSetOpenGL>							m_BoundResourceSet = {};
	};
}	 // namespace Nexus::Graphics

#endif