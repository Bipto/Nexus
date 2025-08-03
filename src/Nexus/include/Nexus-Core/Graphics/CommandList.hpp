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
		ClearColorValue			 Color = {};
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
		Ref<Graphics::Texture> Texture			= nullptr;
		BarrierStage		   BeforeStage		= BarrierStage::All;
		BarrierStage		   AfterStage		= BarrierStage::All;
		BarrierLayout		   BeforeLayout		= BarrierLayout::General;
		BarrierLayout		   AfterLayout		= BarrierLayout::General;
		BarrierAccess		   BeforeAccess		= BarrierAccess::All;
		BarrierAccess		   AfterAccess		= BarrierAccess::All;
		SubresourceDescription SubresourceRange = {};
	};

	struct BufferBarrierDesc
	{
		Ref<Graphics::DeviceBuffer> Buffer		 = nullptr;
		BarrierStage				BeforeStage	 = BarrierStage::All;
		BarrierStage				AfterStage	 = BarrierStage::All;
		BarrierAccess				BeforeAccess = BarrierAccess::All;
		BarrierAccess				AfterAccess	 = BarrierAccess::All;
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
						 DeviceBufferAccelerationStructureCopyDescription>
		RenderCommandData;

	struct CommandListDescription
	{
		std::string DebugName = "CommandList";
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

		void ClearColorTarget(uint32_t index, const ClearColorValue &color, ClearRect clearRect);

		void ClearColorTarget(uint32_t index, const ClearColorValue &color);

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

		const std::vector<RenderCommandData> &GetCommandData() const;
		const CommandListDescription		 &GetDescription();

		bool IsRecording() const;

	  private:
		CommandListDescription		   m_Description = {};
		std::vector<RenderCommandData> m_Commands;
		bool						   m_Started = false;
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