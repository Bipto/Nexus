#pragma once

#include "Color.hpp"
#include "DeviceBuffer.hpp"
#include "Framebuffer.hpp"
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
	struct BufferCopyDescription
	{
		Ref<DeviceBuffer> Source	  = nullptr;
		Ref<DeviceBuffer> Destination = nullptr;
		uint64_t		  ReadOffset  = 0;
		uint64_t		  WriteOffset = 0;
		uint64_t		  Size		  = 0;
	};

	enum class ImageAspect : uint16_t
	{
		Colour,
		Depth,
		Stencil,
		DepthStencil
	};

	struct SubresourceDescription
	{
		uint32_t	X		   = 0;
		uint32_t	Y		   = 0;
		uint32_t	Z		   = 0;
		uint32_t	Width	   = 0;
		uint32_t	Height	   = 0;
		uint32_t	Depth	   = 0;
		uint32_t	MipLevel   = 0;
		uint32_t	ArrayLayer = 0;
		ImageAspect Aspect	   = ImageAspect::Colour;
	};

	struct BufferTextureCopyDescription
	{
		Ref<DeviceBuffer>	   BufferHandle		  = nullptr;
		Ref<Texture>		   TextureHandle	  = nullptr;
		uint64_t			   BufferOffset		  = 0;
		SubresourceDescription TextureSubresource = {};
	};

	struct TextureCopyDescription
	{
		Ref<Texture>		   Source				  = nullptr;
		Ref<Texture>		   Destination			  = nullptr;
		SubresourceDescription SourceSubresource	  = {};
		SubresourceDescription DestinationSubresource = {};
	};

	struct SetVertexBufferCommand
	{
		uint32_t		 Slot = 0;
		VertexBufferView View = {};
	};

	struct SetIndexBufferCommand
	{
		IndexBufferView View = {};
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

	struct DrawCommand
	{
		uint32_t VertexCount   = 0;
		uint32_t InstanceCount = 0;
		uint32_t VertexStart   = 0;
		uint32_t InstanceStart = 0;
	};

	struct DrawIndexedCommand
	{
		uint32_t IndexCount	   = 0;
		uint32_t InstanceCount = 0;
		uint32_t VertexStart   = 0;
		uint32_t IndexStart	   = 0;
		uint32_t InstanceStart = 0;
	};

	struct DrawIndirectCommand
	{
		Ref<DeviceBuffer> IndirectBuffer = nullptr;
		uint32_t		  Offset		 = 0;
		uint32_t		  DrawCount		 = 0;
	};

	struct DrawIndirectIndexedCommand
	{
		Ref<DeviceBuffer> IndirectBuffer = nullptr;
		uint32_t		  Offset		 = 0;
		uint32_t		  DrawCount		 = 0;
	};

	struct DispatchCommand
	{
		uint32_t WorkGroupCountX = 0;
		uint32_t WorkGroupCountY = 0;
		uint32_t WorkGroupCountZ = 0;
	};

	struct DispatchIndirectCommand
	{
		Ref<DeviceBuffer>	  IndirectBuffer = {};
		uint32_t			  Offset		 = 0;
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
		Ref<Framebuffer>	 Source		 = {};
		uint32_t			 SourceIndex = {};
		Ref<Swapchain>		 Target		 = {};
	};

	struct StartTimingQueryCommand
	{
		Ref<TimingQuery> Query = {};
	};

	struct StopTimingQueryCommand
	{
		Ref<TimingQuery> Query = {};
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

	struct TextureSubresourceRange
	{
		uint32_t BaseMipLayer	 = 0;
		uint32_t MipLayerCount	 = 1;
		uint32_t BaseArrayLayer	 = 0;
		uint32_t ArrayLayerCount = 1;
	};

	enum class BarrierStage
	{
		None,
		All,
		Graphics,
		VertexInput,
		VertexShader,
		FragmentShader,
		TesselationControlShader,
		TesselationEvaluationShader,
		GeometryShader,
		ComputeShader,
		RenderTarget,
		TransferSource,
		TransferDestination,
		Resolve
	};

	enum class BarrierAccess
	{
		None,
		All,
		VertexBuffer,
		IndexBuffer,
		RenderTarget,
		StorageImage,
		DepthStencilRead,
		DepthStencilWrite,
		ResolveSource,
		ResolveDestination,
		CopySource,
		CopyDestination,
		DrawIndirect,
	};

	enum class BarrierLayout
	{
		Undefined,
		General,
		Present,
		RenderTarget,
		DepthStencilRead,
		DepthStencilWrite,
		CopySource,
		CopyDestination,
		ResolveSource,
		ResolveDestimation,
		ShaderReadOnly,
		ShaderReadWrite
	};

	struct MemoryBarrierDesc
	{
		BarrierStage  BeforeStage  = BarrierStage::All;
		BarrierStage  AfterStage   = BarrierStage::All;
		BarrierAccess BeforeAccess = BarrierAccess::All;
		BarrierAccess AfterAccess  = BarrierAccess::All;
	};

	struct TextureBarrierDesc
	{
		Graphics::Texture	   *Texture			 = nullptr;
		BarrierStage			BeforeStage		 = BarrierStage::All;
		BarrierStage			AfterStage		 = BarrierStage::All;
		BarrierLayout			BeforeLayout	 = BarrierLayout::General;
		BarrierLayout			AfterLayout		 = BarrierLayout::General;
		BarrierAccess			BeforeAccess	 = BarrierAccess::All;
		BarrierAccess			AfterAccess		 = BarrierAccess::All;
		TextureSubresourceRange SubresourceRange = {};
	};

	struct BufferBarrierDesc
	{
		Graphics::DeviceBuffer *Buffer		 = nullptr;
		BarrierStage			BeforeStage	 = BarrierStage::All;
		BarrierStage			AfterStage	 = BarrierStage::All;
		BarrierAccess			BeforeAccess = BarrierAccess::All;
		BarrierAccess			AfterAccess	 = BarrierAccess::All;
	};

	struct BarrierDesc
	{
		std::vector<MemoryBarrierDesc>	MemoryBarriers	= {};
		std::vector<TextureBarrierDesc> TextureBarriers = {};
		std::vector<BufferBarrierDesc>	BufferBarriers	= {};
	};

	struct CopyBufferToBufferCommand
	{
		BufferCopyDescription BufferCopy = {};
	};

	struct CopyBufferToTextureCommand
	{
		BufferTextureCopyDescription BufferTextureCopy = {};
	};

	struct CopyTextureToBufferCommand
	{
		BufferTextureCopyDescription TextureBufferCopy = {};
	};

	struct CopyTextureToTextureCommand
	{
		TextureCopyDescription TextureCopy = {};
	};

	typedef std::variant<SetVertexBufferCommand,
						 SetIndexBufferCommand,
						 WeakRef<Pipeline>,
						 DrawCommand,
						 DrawIndexedCommand,
						 DrawIndirectCommand,
						 DrawIndirectIndexedCommand,
						 DispatchCommand,
						 DispatchIndirectCommand,
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
						 SetBlendFactorCommand,
						 BarrierDesc,
						 CopyBufferToBufferCommand,
						 CopyBufferToTextureCommand,
						 CopyTextureToBufferCommand,
						 CopyTextureToTextureCommand>
		RenderCommandData;

	struct CommandListSpecification
	{
	};

	/// @brief A class representing a command list
	class NX_API CommandList
	{
	  public:
		/// @brief A constructor creating a new command list
		CommandList(const CommandListSpecification &spec);

		/// @brief A virtual destructor allowing resources to be cleaned up
		virtual ~CommandList()
		{
			m_Commands.clear();
		}

		/// @brief A method that begins a command list
		/// @param beginInfo A parameter containing information about how to begin the
		/// command list
		void Begin();

		/// @brief A method that ends a command list
		void End();

		/// @brief A method that binds a vertex buffer to the pipeline
		/// @param vertexBuffer A pointer to the vertex buffer to bind
		void SetVertexBuffer(VertexBufferView vertexBuffer, uint32_t slot);

		/// @brief A method that binds an index buffer to the pipeline
		/// @param indexBuffer A pointer to the index buffer to bind
		void SetIndexBuffer(IndexBufferView indexBuffer);

		/// @brief A method to bind a pipeline to a command list
		/// @param pipeline The pointer to the pipeline to bind
		void SetPipeline(Ref<Pipeline> pipeline);

		/// @brief A method that submits an instanced draw call using bound vertex
		/// buffers
		/// @param vertexCount The number of vertices to draw
		/// @param instanceCount The number of instances to draw
		/// @param vertexStart An offset into the vertex buffer to start rendering at
		/// @param instanceStart An offset into the instance buffer to start rendering
		/// at
		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart);

		/// @brief A method that submits an instanced draw call using bound vertex
		/// buffers and an index buffer
		/// @param indexCount The number of indices in the primitive
		/// @param instanceCount The number of instances to draw
		/// @param vertexStart An offset into the vertex buffer to start rendering at
		/// @param indexStart An offset into the index buffer to start rendering at
		/// @param instanceStart An offset into the instance buffer to start rendering
		/// at
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart);

		void DrawIndirect(Ref<DeviceBuffer> indirectBuffer, uint32_t offset, uint32_t drawCount);

		void DrawIndexedIndirect(Ref<DeviceBuffer> indirectBuffer, uint32_t offset, uint32_t drawCount);

		void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

		void DispatchIndirect(Ref<DeviceBuffer> indirectBuffer, uint32_t offset);

		/// @brief A method that updates the resources bound within a pipeline
		/// @param resources A reference counted pointer to a ResourceSet
		void SetResourceSet(Ref<ResourceSet> resources);

		void ClearColorTarget(uint32_t index, const ClearColorValue &color);

		void ClearDepthTarget(const ClearDepthStencilValue &value);

		void SetRenderTarget(RenderTarget target);

		void SetViewport(const Viewport &viewport);

		void SetScissor(const Scissor &scissor);

		void ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Ref<Swapchain> target);

		void StartTimingQuery(Ref<TimingQuery> query);

		void StopTimingQuery(Ref<TimingQuery> query);

		void SetStencilRef(uint32_t stencil);

		void SetDepthBounds(float min, float max);

		void SetBlendFactor(float r, float g, float b, float a);

		void Barrier(const BarrierDesc &barrier);

		void CopyBufferToBuffer(const BufferCopyDescription &bufferCopy);

		void CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy);

		void CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy);

		void CopyTextureToTexture(const TextureCopyDescription &textureCopy);

		const std::vector<RenderCommandData> &GetCommandData() const;
		const CommandListSpecification		 &GetSpecification();

	  private:
		CommandListSpecification	   m_Specification = {};
		std::vector<RenderCommandData> m_Commands;
		bool						   m_Started = false;
	};

	/// @brief A typedef to simplify creating function pointers to render commands
	typedef void (*RenderCommand)(Ref<CommandList> commandList);
}	 // namespace Nexus::Graphics