#pragma once

#include "Color.hpp"
#include "Framebuffer.hpp"
#include "GPUBuffer.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Pipeline.hpp"
#include "RenderTarget.hpp"
#include "Scissor.hpp"
#include "Texture.hpp"
#include "TimingQuery.hpp"
#include "Viewport.hpp"

namespace Nexus::Graphics
{
	struct SetVertexBufferCommand
	{
		WeakRef<VertexBuffer> VertexBufferRef = {};
		uint32_t			  Slot			  = 0;
	};

	union ClearColor
	{
		float	 f32[4];
		int32_t	 i32[4];
		uint32_t ui32[4];
	};

	/// @brief A struct representing a set of values to use  to clear the colour
	/// buffer
	struct ClearColorValue
	{
		/// @brief The red channel as a value between 0.0f and 1.0f
		float Red = 1.0f;

		/// @brief The green channel as a value between 0.0f and 1.0f
		float Green = 1.0f;

		/// @brief The blue channel as a value between 0.0f and 1.0f
		float Blue = 1.0f;

		/// @brief The alpha channel as a value between 0.0f and 1.0f
		float Alpha = 1.0f;
	};

	/// @brief A struct representing a set of values to use to clear the
	/// depth/stencil buffer
	struct ClearDepthStencilValue
	{
		/// @brief The value to use to clear the depth buffer
		float Depth = 1.0f;

		/// @brief The value to use to clear the stencil buffer
		uint8_t Stencil = 0;
	};

	/// @brief A struct representing a draw command to be executed using a vertex
	/// buffer
	struct DrawElementCommand
	{
		/// @brief The starting index within the vertex buffer
		uint32_t Start = 0;

		/// @brief The number of vertices to draw
		uint32_t Count = 0;
	};

	/// @brief A struct representing a draw command using a vertex buffer and an
	/// indexed buffer
	struct DrawIndexedCommand
	{
		/// @brief The number of vertices to draw
		uint32_t Count = 0;

		/// @brief An offset into the index buffer
		uint32_t IndexStart = 0;

		uint32_t VertexStart = 0;
	};

	struct DrawInstancedCommand
	{
		uint32_t VertexCount   = 0;
		uint32_t InstanceCount = 0;
		uint32_t VertexStart   = 0;
		uint32_t InstanceStart = 0;
	};

	struct DrawInstancedIndexedCommand
	{
		uint32_t IndexCount	   = 0;
		uint32_t InstanceCount = 0;
		uint32_t VertexStart   = 0;
		uint32_t IndexStart	   = 0;
		uint32_t InstanceStart = 0;
	};

	struct ClearColorTargetCommand
	{
		uint32_t		Index = {};
		ClearColorValue Color = {};
	};

	struct ClearDepthStencilTargetCommand
	{
		ClearDepthStencilValue Value = {};
	};

	struct ResolveSamplesToSwapchainCommand
	{
		WeakRef<Framebuffer> Source		 = {};
		uint32_t			 SourceIndex = {};
		Swapchain			*Target		 = nullptr;
	};

	struct StartTimingQueryCommand
	{
		WeakRef<TimingQuery> Query = {};
	};

	struct StopTimingQueryCommand
	{
		WeakRef<TimingQuery> Query = {};
	};

	struct SetStencilRefCommand
	{
		uint32_t Value = {};
	};

	struct SetDepthBoundsCommand
	{
		float Min = {};
		float Max = {};
	};

	struct SetBlendFactorCommand
	{
		float R = {};
		float G = {};
		float B = {};
		float A = {};
	};

	typedef std::variant<SetVertexBufferCommand,
						 WeakRef<IndexBuffer>,
						 WeakRef<Pipeline>,
						 DrawElementCommand,
						 DrawIndexedCommand,
						 DrawInstancedCommand,
						 DrawInstancedIndexedCommand,
						 Ref<ResourceSet>,
						 ClearColorTargetCommand,
						 ClearDepthStencilTargetCommand,
						 RenderTarget,
						 Viewport,
						 Scissor,
						 ResolveSamplesToSwapchainCommand,
						 StartTimingQueryCommand,
						 StopTimingQueryCommand,
						 SetStencilRefCommand,
						 SetDepthBoundsCommand,
						 SetBlendFactorCommand>
		RenderCommandData;

	struct CommandListSpecification
	{
	};

	/// @brief A class representing a command list
	class CommandList
	{
	  public:
		/// @brief A constructor creating a new command list
		NX_API CommandList(const CommandListSpecification &spec);

		/// @brief A virtual destructor allowing resources to be cleaned up
		NX_API virtual ~CommandList()
		{
			m_Commands.clear();
		}

		/// @brief A method that begins a command list
		/// @param beginInfo A parameter containing information about how to begin the
		/// command list
		NX_API void Begin();

		/// @brief A method that ends a command list
		NX_API void End();

		/// @brief A method that binds a vertex buffer to the pipeline
		/// @param vertexBuffer A pointer to the vertex buffer to bind
		NX_API void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot);

		/// @brief A method that binds an index buffer to the pipeline
		/// @param indexBuffer A pointer to the index buffer to bind
		NX_API void SetIndexBuffer(Ref<IndexBuffer> indexBuffer);

		/// @brief A method to bind a pipeline to a command list
		/// @param pipeline The pointer to the pipeline to bind
		NX_API void SetPipeline(Ref<Pipeline> pipeline);

		/// @brief A method that submits a draw call using the bound vertex buffer
		/// @param start The offset to begin rendering at
		/// @param count The number of vertices to draw
		NX_API void Draw(uint32_t start, uint32_t count);

		/// @brief A method that submits an indexed draw call using the bound vertex
		/// buffer and index buffer
		/// @param count The number of vertices to draw
		/// @param offset The offset to begin rendering at
		NX_API void DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart);

		/// @brief A method that submits an instanced draw call using bound vertex
		/// buffers
		/// @param vertexCount The number of vertices to draw
		/// @param instanceCount The number of instances to draw
		/// @param vertexStart An offset into the vertex buffer to start rendering at
		/// @param instanceStart An offset into the instance buffer to start rendering
		/// at
		NX_API void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart);

		/// @brief A method that submits an instanced draw call using bound vertex
		/// buffers and an index buffer
		/// @param indexCount The number of indices in the primitive
		/// @param instanceCount The number of instances to draw
		/// @param vertexStart An offset into the vertex buffer to start rendering at
		/// @param indexStart An offset into the index buffer to start rendering at
		/// @param instanceStart An offset into the instance buffer to start rendering
		/// at
		NX_API void DrawInstancedIndexed(uint32_t indexCount,
										 uint32_t instanceCount,
										 uint32_t vertexStart,
										 uint32_t indexStart,
										 uint32_t instanceStart);

		/// @brief A method that updates the resources bound within a pipeline
		/// @param resources A reference counted pointer to a ResourceSet
		NX_API void SetResourceSet(Ref<ResourceSet> resources);

		NX_API void ClearColorTarget(uint32_t index, const ClearColorValue &color);

		NX_API void ClearDepthTarget(const ClearDepthStencilValue &value);

		NX_API void SetRenderTarget(RenderTarget target);

		NX_API void SetViewport(const Viewport &viewport);

		NX_API void SetScissor(const Scissor &scissor);

		NX_API void ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target);

		NX_API void StartTimingQuery(Ref<TimingQuery> query);

		NX_API void StopTimingQuery(Ref<TimingQuery> query);

		NX_API void SetStencilRef(uint32_t stencil);

		NX_API void SetDepthBounds(float min, float max);

		NX_API void SetBlendFactor(float r, float g, float b, float a);

		NX_API const std::vector<RenderCommandData> &GetCommandData() const;
		NX_API const CommandListSpecification		&GetSpecification();

	  private:
		CommandListSpecification	   m_Specification = {};
		std::vector<RenderCommandData> m_Commands;
		bool						   m_Started = false;
	};

	/// @brief A typedef to simplify creating function pointers to render commands
	typedef void (*RenderCommand)(Ref<CommandList> commandList);
}	 // namespace Nexus::Graphics