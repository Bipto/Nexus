#pragma once

#include "AccelerationStructure.hpp"
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
	struct ClearRect
	{
		int32_t	 X		= 0;
		int32_t	 Y		= 0;
		uint32_t Width	= 0;
		uint32_t Height = 0;
	};

	struct BufferCopy
	{
		uint64_t ReadOffset	 = 0;
		uint64_t WriteOffset = 0;
		uint64_t Size		 = 0;
	};

	struct BufferCopyDescription
	{
		Ref<DeviceBuffer>		Source		= nullptr;
		Ref<DeviceBuffer>		Destination = nullptr;
		std::vector<BufferCopy> Copies		= {};
	};

	struct Offset3D
	{
		int32_t X = 0;
		int32_t Y = 0;
		int32_t Z = 0;
	};

	struct Extent3D
	{
		uint32_t Width	= 0;
		uint32_t Height = 0;
		uint32_t Depth	= 0;
	};

	struct SubresourceRange
	{
		uint32_t BaseMipLevel	= 0;
		uint32_t LevelCount		= 1;
		uint32_t BaseArrayLayer = 0;
		uint32_t LayerCount		= 1;
	};

	struct SubresourceLayers
	{
		uint32_t MipLevel		= 0;
		uint32_t BaseArrayLayer = 0;
		uint32_t LayerCount		= 0;
	};

	struct BufferTextureCopyDescription
	{
		Ref<DeviceBuffer> BufferHandle		 = nullptr;
		uint64_t		  BufferOffset		 = 0;
		uint64_t		  BufferRowLength	 = 0;
		uint64_t		  BufferImageHeight	 = 0;
		Ref<Texture>	  TextureHandle		 = nullptr;
		SubresourceLayers TextureSubresource = {};
		Offset3D		  TextureOffset		 = {};
		Extent3D		  TextureExtent		 = {};
	};

	struct TextureCopyDescription
	{
		Ref<Texture>	  Source				 = nullptr;
		Ref<Texture>	  Destination			 = nullptr;
		SubresourceLayers SourceSubresource		 = {};
		SubresourceLayers DestinationSubresource = {};
		Offset3D		  SourceOffset			 = {};
		Offset3D		  DestinationOffset		 = {};
		Extent3D		  Extent				 = {};
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

	struct BeginDebugGroupCommand
	{
		std::string GroupName = {};
	};

	struct EndDebugGroupCommand
	{
	};

	struct InsertDebugMarkerCommand
	{
		std::string MarkerName = {};
	};

	/// @brief A struct representing a set of values to use  to clear the colour
	/// buffer
	struct ClearColourValue
	{
		/// @brief The red channel as a value between 0.0f and 1.0f
		float Red = 1.0f;

		/// @brief The green channel as a value between 0.0f and 1.0f
		float Green = 1.0f;

		/// @brief The blue channel as a value between 0.0f and 1.0f
		float Blue = 1.0f;

		/// @brief The alpha channel as a value between 0.0f and 1.0f
		float Alpha = 1.0f;

		/// @brief An optional parameter controlling which area of the texture to clear
		std::optional<ClearRect> Rect = {};
	};

	/// @brief A struct representing a set of values to use to clear the
	/// depth/stencil buffer
	struct ClearDepthStencilValue
	{
		/// @brief The value to use to clear the depth buffer
		float Depth = 1.0f;

		/// @brief The value to use to clear the stencil buffer
		uint8_t Stencil = 0;

		/// @brief An optional parameter controlling which area of the texture to clear
		std::optional<ClearRect> Rect = {};
	};

	struct BlendFactorDesc
	{
		float Red	= 0.0f;
		float Green = 0.0f;
		float Blue	= 0.0f;
		float Alpha = 0.0f;
	};

	/// @brief A struct representing a draw command to be executed using a vertex
	/// buffer
	struct DrawDescription
	{
		uint32_t VertexCount   = 0;
		uint32_t InstanceCount = 0;
		uint32_t VertexStart   = 0;
		uint32_t InstanceStart = 0;
	};

	struct DrawIndexedDescription
	{
		uint32_t IndexCount	   = 0;
		uint32_t InstanceCount = 0;
		uint32_t VertexStart   = 0;
		uint32_t IndexStart	   = 0;
		uint32_t InstanceStart = 0;
	};

	struct DrawIndirectDescription
	{
		Ref<DeviceBuffer> IndirectBuffer = nullptr;
		size_t			  Offset		 = 0;
		size_t			  Stride		 = 0;
		size_t			  DrawCount		 = 0;
	};

	struct DrawIndirectIndexedDescription
	{
		Ref<DeviceBuffer> IndirectBuffer = nullptr;
		size_t			  Offset		 = 0;
		size_t			  Stride		 = 0;
		size_t			  DrawCount		 = 0;
	};

	struct DispatchDescription
	{
		uint32_t WorkGroupCountX = 0;
		uint32_t WorkGroupCountY = 0;
		uint32_t WorkGroupCountZ = 0;
	};

	struct DispatchIndirectDescription
	{
		Ref<DeviceBuffer> IndirectBuffer = {};
		size_t			  Offset		 = 0;
		size_t			  Stride		 = 0;
	};

	struct DrawMeshDescription
	{
		uint32_t WorkGroupCountX = 0;
		uint32_t WorkGroupCountY = 0;
		uint32_t WorkGroupCountZ = 0;
	};

	struct DrawMeshIndirectDescription
	{
		Ref<DeviceBuffer> IndirectBuffer = {};
		size_t			  Offset		 = 0;
		size_t			  Stride		 = 0;
		size_t			  DrawCount		 = 0;
	};

	struct ClearColorTargetCommand
	{
		uint32_t				 Index = {};
		ClearColourValue		 Color = {};
		std::optional<ClearRect> Rect  = {};
	};

	struct ClearDepthStencilTargetCommand
	{
		ClearDepthStencilValue	 Value = {};
		std::optional<ClearRect> Rect  = {};
	};

	struct ResolveSamplesToSwapchainCommand
	{
		Ref<Framebuffer> Source		 = {};
		uint32_t		 SourceIndex = {};
		Ref<Swapchain>	 Target		 = {};
	};

	struct StartTimingQueryCommand
	{
		Ref<TimingQuery> Query = {};
	};

	struct StopTimingQueryCommand
	{
		Ref<TimingQuery> Query = {};
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

	struct SetBlendFactorCommand
	{
		BlendFactorDesc BlendFactorDesc = {};
	};

	struct SetStencilReferenceCommand
	{
		uint32_t StencilReference = {};
	};

	struct BuildAccelerationStructuresCommand
	{
		std::vector<AccelerationStructureBuildDescription> BuildDescriptions = {};
	};

	enum class BarrierPipelineStage
	{
		None,
		DrawIndirect,
		VertexInput,
		VertexShader,
		TessellationControlShader,
		TessellationEvaluationShader,
		GeometryShader,
		FragmentShader,
		EarlyFragmentTests,
		LateFragmentTests,
		ColourAttachmentOutput,
		ComputeShader,
		AllTransfers,
		Transfer,
		Host,
		AllGraphics,
		AllCommands,
		Copy,
		Resolve,
		IndexInput,
		VertexAttributeInput,
		PreRasterizationShaders,
		TransformFeedback,
		AccelerationStructure,
		RayTracingShader,
		TaskShader,
		MeshShader
	};

	enum class BarrierAccess
	{
		None,
		IndirectCommandRead,
		IndexRead,
		VertexAttributeRead,
		UniformRead,
		InputAttachmentRead,
		ShaderRead,
		ShaderWrite,
		ColourAttachmentRead,
		ColourAttachmentWrite,
		DepthStencilAttachmentRead,
		DepthStencilAttachmentWrite,
		TransferRead,
		TransferWrite,
		HostRead,
		HostWrite,
		MemoryRead,
		MemoryWrite,
		TransformFeedbackWrite,
		AccelerationStructureRead,
		AccelerationStructureWrite,
		VideoDecode,
		VideoEncode
	};

	struct MemoryBarrierDesc
	{
		BarrierAccess		 BeforeAccess = {};
		BarrierAccess		 AfterAccess  = {};
		BarrierPipelineStage BeforeStage  = {};
		BarrierPipelineStage AfterStage	  = {};
	};

	struct TextureBarrierDesc
	{
		Ref<Graphics::Texture> Texture			= nullptr;
		TextureLayout		   Layout			= {};
		BarrierAccess		   BeforeAccess		= {};
		BarrierAccess		   AfterAccess		= {};
		BarrierPipelineStage   BeforeStage		= {};
		BarrierPipelineStage   AfterStage		= {};
		SubresourceRange	   SubresourceRange = {};
	};

	struct BufferBarrierDesc
	{
		Ref<Graphics::DeviceBuffer> Buffer		 = nullptr;
		BarrierAccess				BeforeAccess = {};
		BarrierAccess				AfterAccess	 = {};
		BarrierPipelineStage		BeforeStage	 = {};
		BarrierPipelineStage		AfterStage	 = {};
		size_t						Offset		 = 0;
		size_t						Size		 = 0;
	};

	struct PushConstantsDesc
	{
		std::string			 Name	= {};
		size_t				 Offset = 0;
		std::vector<uint8_t> Data	= {};
	};

	typedef std::variant<SetVertexBufferCommand,
						 SetIndexBufferCommand,
						 WeakRef<Pipeline>,
						 DrawDescription,
						 DrawIndexedDescription,
						 DrawIndirectDescription,
						 DrawIndirectIndexedDescription,
						 DispatchDescription,
						 DispatchIndirectDescription,
						 DrawMeshDescription,
						 DrawMeshIndirectDescription,
						 Ref<ResourceSet>,
						 ClearColorTargetCommand,
						 ClearDepthStencilTargetCommand,
						 RenderTarget,
						 Viewport,
						 Scissor,
						 ResolveSamplesToSwapchainCommand,
						 StartTimingQueryCommand,
						 StopTimingQueryCommand,
						 CopyBufferToBufferCommand,
						 CopyBufferToTextureCommand,
						 CopyTextureToBufferCommand,
						 CopyTextureToTextureCommand,
						 BeginDebugGroupCommand,
						 EndDebugGroupCommand,
						 InsertDebugMarkerCommand,
						 SetBlendFactorCommand,
						 SetStencilReferenceCommand,
						 BuildAccelerationStructuresCommand,
						 AccelerationStructureCopyDescription,
						 AccelerationStructureDeviceBufferCopyDescription,
						 DeviceBufferAccelerationStructureCopyDescription,
						 PushConstantsDesc,
						 MemoryBarrierDesc,
						 TextureBarrierDesc,
						 BufferBarrierDesc>
		RenderCommandData;

	struct CommandListDescription
	{
		std::string DebugName = "CommandList";
		bool AutomaticBarrierTransitions = true;
	};

	/// @brief A class representing a command list
	class NX_API CommandList
	{
	  public:
		/// @brief A constructor creating a new command list
		CommandList(const CommandListDescription &spec);

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

		void Draw(const DrawDescription &desc);

		void DrawIndexed(const DrawIndexedDescription &desc);

		void DrawIndirect(const DrawIndirectDescription &desc);

		void DrawIndexedIndirect(const DrawIndirectIndexedDescription &desc);

		void Dispatch(const DispatchDescription &desc);

		void DispatchIndirect(const DispatchIndirectDescription &desc);

		void DrawMesh(const DrawMeshDescription &desc);

		void DrawMeshIndirect(const DrawMeshIndirectDescription &desc);

		/// @brief A method that updates the resources bound within a pipeline
		/// @param resources A reference counted pointer to a ResourceSet
		void SetResourceSet(Ref<ResourceSet> resources);

		void ClearColourTarget(uint32_t index, const ClearColourValue &color, ClearRect clearRect);

		void ClearColourTarget(uint32_t index, const ClearColourValue &color);

		void ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect);

		void ClearDepthTarget(const ClearDepthStencilValue &value);

		void SetRenderTarget(RenderTarget target);

		void SetViewport(const Viewport &viewport);

		void SetScissor(const Scissor &scissor);

		void ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Ref<Swapchain> target);

		void StartTimingQuery(Ref<TimingQuery> query);

		void StopTimingQuery(Ref<TimingQuery> query);

		void CopyBufferToBuffer(const BufferCopyDescription &bufferCopy);

		void CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy);

		void CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy);

		void CopyTextureToTexture(const TextureCopyDescription &textureCopy);

		void BeginDebugGroup(const std::string &name);

		void EndDebugGroup();

		void InsertDebugMarker(const std::string &name);

		void SetBlendFactor(const BlendFactorDesc &blendFactor);

		void SetStencilReference(uint32_t stencilReference);

		void BuildAccelerationStructures(const std::vector<AccelerationStructureBuildDescription> &description);

		void CopyAccelerationStructure(const AccelerationStructureCopyDescription &description);

		void CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description);

		void CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description);

		void WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset);

		void SubmitMemoryBarrier(const MemoryBarrierDesc &desc);

		void SubmitTextureBarrier(const TextureBarrierDesc &desc);

		void SubmitBufferBarrier(const BufferBarrierDesc &desc);

		const std::vector<RenderCommandData> &GetCommandData() const;
		const CommandListDescription		 &GetDescription();

		bool IsRecording() const;

	  private:
		CommandListDescription		   m_Description = {};
		std::vector<RenderCommandData> m_Commands;
		bool						   m_Started	 = false;
		uint32_t					   m_DebugGroups = 0;
	};

	/// @brief A typedef to simplify creating function pointers to render commands
	typedef void (*RenderCommand)(Ref<CommandList> commandList);

	class ScopedDebugGroup
	{
	  public:
		ScopedDebugGroup(const std::string &name, Ref<CommandList> commandList) : m_CommandList(commandList)
		{
			if (m_CommandList->IsRecording())
			{
				m_CommandList->BeginDebugGroup(name);
			}
		}

		~ScopedDebugGroup()
		{
			if (m_CommandList->IsRecording())
			{
				m_CommandList->EndDebugGroup();
			}
		}

		ScopedDebugGroup(const ScopedDebugGroup &)			  = delete;
		ScopedDebugGroup &operator=(const ScopedDebugGroup &) = delete;

	  private:
		Ref<CommandList> m_CommandList = nullptr;
	};
}	 // namespace Nexus::Graphics