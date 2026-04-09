#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include "RHI/AccelerationStructure.hpp"
#include "RHI/ComputeState.hpp"
#include "RHI/DeviceBuffer.hpp"
#include "RHI/Framebuffer.hpp"
#include "RHI/Pipeline.hpp"
#include "RHI/RHI-Core.hpp"
#include "RHI/Scissor.hpp"
#include "RHI/Texture.hpp"
#include "RHI/TimingQuery.hpp"
#include "RHI/Types.hpp"
#include "RHI/Viewport.hpp"

#include <glm/glm.hpp>

namespace Nexus::Graphics
{
	class CommandExecutor;
	class IGraphicsDevice;

	/// @brief A structure representing parameters to set a clear rectangle
	struct ClearRect
	{
		/// @brief The X coordinate of the rectangle
		int32_t X = 0;

		/// @brief The Y coordinate of the rectangle
		int32_t Y = 0;

		/// @brief The width of the rectangle
		uint32_t Width = 0;

		/// @brief The height of the rectangle
		uint32_t Height = 0;
	};

	/// @brief A structure representing a copy operation between two buffers
	struct BufferCopy
	{
		/// @brief An integer representing the offset into the buffer that is being read from
		uint64_t ReadOffset = 0;

		/// @brief An integer representing the offset into the buffer that is being written to
		uint64_t WriteOffset = 0;

		/// @brief The size in bytes of the copy operation
		uint64_t Size = 0;
	};

	/// @brief A structure containing a set of buffer copy operations
	struct BufferCopyDescription
	{
		/// @brief A pointer to the buffer to be used as a source
		IDeviceBuffer *Source = nullptr;

		/// @brief A pointer to the buffer to be used as the destination
		IDeviceBuffer *Destination = nullptr;

		/// @brief A vector of BufferCopy objects representing the areas of the buffers to be copied
		std::vector<BufferCopy> Copies = {};
	};

	/// @brief A structure representing a copy operation between a buffer and a texture
	struct BufferTextureCopyDescription
	{
		/// @brief A pointer to the buffer to use in the copy operation
		IDeviceBuffer *BufferHandle = nullptr;

		/// @brief An integer representing the offset to read to/write from in the buffer
		uint64_t BufferOffset = 0;

		/// @brief An integer representing the row length of the data
		uint64_t BufferRowLength = 0;

		/// @brief An integer representing the number of rows in the data
		uint64_t BufferImageHeight = 0;

		/// @brief A pointer to the texture to use in the copy operation
		Ref<ITexture> TextureHandle = nullptr;

		/// @brief A structure containing parameters specifying the offset into the textures
		Offset3D TextureOffset = {};

		/// @brief A structure containing parameters specifying the size of the texture area to copy
		Extent2D TextureExtent = {};

		/// @brief An integer containing which mip level of the texture to copy to/from
		uint32_t MipLevel = 0;
	};

	/// @brief A structure representing a copy operation between two textures
	struct TextureCopyDescription
	{
		/// @brief A pointer to the source texture for the copy operation
		Ref<ITexture> Source = nullptr;

		/// @brief A pointer to the destination texture for the copy operation
		Ref<ITexture> Destination = nullptr;

		/// @brief A set of parameters specifying the offset into the source texture
		Offset3D SourceOffset = {};

		/// @brief A set of parameters specifying the offset into the destination texture
		Offset3D DestinationOffset = {};

		/// @brief A set of parameters specifying the size of the texture area to be copied
		Extent2D Extent = {};

		/// @brief An integer representing which level of the source texture to copy to/from
		uint32_t SourceMipLevel = 0;

		/// @brief An integer representing which level of the destination texture to copy to/from
		uint32_t DestinationMipLevel = 0;
	};

	/// @brief A structure representing a command binding a vertex buffer to a slot
	struct SetVertexBufferCommand
	{
		/// @brief An integer representing the slot that the vertex buffer should be bound to
		uint32_t Slot = 0;

		/// @brief A structure containing which sections of the buffer should be bound
		VertexBufferView View = {};
	};

	/// @brief A structure representing a command binding an index buffer to a command list
	struct SetIndexBufferCommand
	{
		/// @brief A structure containing how the index buffer should be bound
		IndexBufferView View = {};
	};

	/// @brief A structure representing a command to mark a debug group in a command list
	struct BeginDebugGroupCommand
	{
		/// @brief A string containing the debug group name
		std::string GroupName = {};

		/// @brief Four floats representing the colour of the displayed text
		glm::vec4 Colour = {};
	};

	/// @brief A structure representing a command to mark the end of a debug group
	struct EndDebugGroupCommand
	{
	};

	/// @brief A structure representing a command to insert a debug marker into a command list
	struct InsertDebugMarkerCommand
	{
		/// @brief A string containing the marker name
		std::string MarkerName = {};

		/// @brief Four floats representing the colour of the displayed text
		glm::vec4 Colour = {};
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

	/// @brief A structure representing a set of floats to use when blending colours
	struct BlendFactorDesc
	{
		/// @brief A value between 0.0 and 1.0 representing the blend factor in the red channel
		float Red = 0.0f;

		/// @brief A value between 0.0 and 1.0 representing the blend factor in the green channel
		float Green = 0.0f;

		/// @brief A value between 0.0 and 1.0 representing the blend factor in the blue channel
		float Blue = 0.0f;

		/// @brief A value between 0.0 and 1.0 representing the blend factor in the alpha channel
		float Alpha = 0.0f;
	};

	/// @brief A struct representing a draw command to be executed using a vertex
	/// buffer
	struct DrawDescription
	{
		/// @brief An integer containing the vertex count used by the draw call
		uint32_t VertexCount = 0;

		/// @brief An integer containing the instance count used by the draw call
		uint32_t InstanceCount = 0;

		/// @brief An integer containing the vertex start index used by the draw call
		uint32_t VertexStart = 0;

		/// @brief An integer containing the instance start index used by the draw call
		uint32_t InstanceStart = 0;
	};

	/// @brief A struct representing a draw command to be executed using a vertex
	/// buffer and an index buffer
	struct DrawIndexedDescription
	{
		/// @brief An integer containing the index count used by the draw call
		uint32_t IndexCount = 0;

		/// @brief An integer containing the instance count used by the draw call
		uint32_t InstanceCount = 0;

		/// @brief An integer containing the vertex start index used by the draw call
		uint32_t VertexStart = 0;

		/// @brief An integer containing the index start count used by the draw call
		uint32_t IndexStart = 0;

		/// @brief An integer containing the instance start count used by the draw call
		uint32_t InstanceStart = 0;
	};

	/// @brief A structure representing a draw indirect command using a vertex buffer and an indirect buffer
	struct DrawIndirectDescription
	{
		/// @brief A pointer to the indirect buffer to use for the draw call
		IDeviceBuffer *IndirectBuffer = nullptr;

		/// @brief An integer representing the offset into the indirect buffer to read from
		size_t Offset = 0;

		/// @brief An integer representing the stride between successive draw commands in the indirect buffer
		size_t Stride = 0;

		/// @brief An integer representing the number of draw commands to execute
		size_t DrawCount = 0;
	};

	/// @brief A structure representing a draw indirect command using a vertex buffer, index buffer and an indirect buffer
	struct DrawIndirectIndexedDescription
	{
		/// @brief A pointer to the indirect buffer to use for the draw call
		IDeviceBuffer *IndirectBuffer = nullptr;

		/// @brief An integer representing the offset into the indirect buffer to read from
		size_t Offset = 0;

		/// @brief An integer representing the stride between successive draw commands in the indirect buffer
		size_t Stride = 0;

		/// @brief An integer representing the number of draw commands to execute
		size_t DrawCount = 0;
	};

	/// @brief A structure representing a dispatch command to be executed using a compute shader
	struct DispatchDescription
	{
		/// @brief An integer representing the size of the workgroup in the X dimension
		uint32_t WorkGroupCountX = 0;

		/// @brief An integer representing the size of the workgroup in the Y dimension
		uint32_t WorkGroupCountY = 0;

		/// @brief An integer representing the size of the workgroup in the Z dimension
		uint32_t WorkGroupCountZ = 0;
	};

	/// @brief A structure representing a dispatch indirect command using an indirect buffer
	struct DispatchIndirectDescription
	{
		/// @brief A pointer to the indirect buffer to use
		IDeviceBuffer *IndirectBuffer = {};

		/// @brief An integer representing the offset into the indirect buffer to read from
		size_t Offset = 0;

		/// @brief An integer representing the stride between successive draw commands in the indirect buffer
		size_t Stride = 0;
	};

	/// @brief A structure representing a mesh draw command to be executed using a mesh shader
	struct DrawMeshDescription
	{
		/// @brief An integer representing the size of the workgroup in the X dimension
		uint32_t WorkGroupCountX = 0;

		/// @brief An integer representing the size of the workgroup in the Y dimension
		uint32_t WorkGroupCountY = 0;

		/// @brief An integer representing the size of the workgroup in the Z dimension
		uint32_t WorkGroupCountZ = 0;
	};

	/// @brief A structure representing a mesh draw indirect command using an indirect buffer
	struct DrawMeshIndirectDescription
	{
		/// @brief A pointer to the indirect buffer to use
		IDeviceBuffer *IndirectBuffer = {};

		/// @brief An integer representing the offset into the indirect buffer to read from
		size_t Offset = 0;

		/// @brief An integer representing the stride between successive draw commands in the indirect buffer
		size_t Stride = 0;

		/// @brief An integer representing the number of draw commands to execute
		size_t DrawCount = 0;
	};

	/// @brief A structure representing a command to clear a colour target
	struct ClearColorTargetCommand
	{
		/// @brief An integer representing the index of the colour attachment to clear
		uint32_t Index = {};

		/// @brief A structure containing the colour values to use when clearing the target
		ClearColourValue Colour = {};

		/// @brief An optional parameter controlling which area of the texture to clear
		std::optional<ClearRect> Rect = {};
	};

	/// @brief A structure representing a command to clear a depth/stencil target
	struct ClearDepthStencilTargetCommand
	{
		/// @brief A structure containing the depth/stencil values to use when clearing the target
		ClearDepthStencilValue Value = {};

		/// @brief An optional parameter controlling which area of the texture to clear
		std::optional<ClearRect> Rect = {};
	};

	/// @brief A structure representing a command to resolve a multisampled texture
	struct ResolveTextureDescription
	{
		/// @brief A pointer to the source texture for the resolve operation
		Ref<ITexture> Source = nullptr;

		/// @brief A pointer to the destination texture for the resolve operation
		Ref<ITexture> Destination = nullptr;

		/// @brief An integer representing which array layer of the source texture to resolve from
		uint32_t SourceArrayLayer = 0;

		/// @brief An integer representing which mip level of the source texture to resolve from
		uint32_t SourceMipLevel = 0;

		/// @brief An integer representing which array layer of the destination texture to resolve to
		uint32_t DestinationArrayLayer = 0;

		/// @brief An integer representing which mip level of the destination texture to resolve to
		uint32_t DestinationMipLevel = 0;
	};

	/// @brief A structure representing a command to start a timing query
	struct StartTimingQueryCommand
	{
		/// @brief A pointer to the timing query to start
		ITimingQuery *Query = nullptr;
	};

	/// @brief A structure representing a command to stop a timing query
	struct StopTimingQueryCommand
	{
		/// @brief A pointer to the timing query to stop
		ITimingQuery *Query = nullptr;
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
		BlendFactorDesc BlendFactor = {};
	};

	struct SetStencilReferenceCommand
	{
		uint32_t StencilReference = {};
	};

	struct BuildAccelerationStructuresCommand
	{
		std::vector<AccelerationStructureGeometryBuildDescription> BuildDescriptions = {};
	};

	struct TraceRaysDescription
	{
		DeviceAddressRegion		   RaygenRegion	  = {};
		StridedDeviceAddressRegion MissRegion	  = {};
		StridedDeviceAddressRegion HitRegion	  = {};
		StridedDeviceAddressRegion CallableRegion = {};
		uint32_t				   Width		  = 0;
		uint32_t				   Height		  = 0;
		uint32_t				   Depth		  = 0;
	};

	struct ResourceSetBindingDescription
	{
		Ref<IResourceSet>							 TargetResourceSet = nullptr;
		std::map<std::string, std::vector<uint32_t>> DynamicOffsets	   = {};
	};

	enum class BarrierPipelineStage
	{
		NoStage,
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
		NoAccess,
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
		Ref<Graphics::ITexture> Texture					= nullptr;
		TextureLayout			Layout					= {};
		BarrierAccess			BeforeAccess			= {};
		BarrierAccess			AfterAccess				= {};
		BarrierPipelineStage	BeforeStage				= {};
		BarrierPipelineStage	AfterStage				= {};
		SubresourceRange		TextureSubresourceRange = {};
	};

	struct BufferBarrierDesc
	{
		Ref<Graphics::IDeviceBuffer> Buffer		  = nullptr;
		BarrierAccess				 BeforeAccess = {};
		BarrierAccess				 AfterAccess  = {};
		BarrierPipelineStage		 BeforeStage  = {};
		BarrierPipelineStage		 AfterStage	  = {};
		size_t						 Offset		  = 0;
		size_t						 Size		  = 0;
	};

	struct BarrierGroupDescription
	{
		std::vector<MemoryBarrierDesc>	MemoryBarriers	= {};
		std::vector<TextureBarrierDesc> TextureBarriers = {};
		std::vector<BufferBarrierDesc>	BufferBarriers	= {};

		void Clear()
		{
			MemoryBarriers.clear();
			TextureBarriers.clear();
			BufferBarriers.clear();
		}
	};

	struct PushConstantsDesc
	{
		std::string			 Name	= {};
		size_t				 Offset = 0;
		std::vector<uint8_t> Data	= {};
	};

	struct EndRenderingCommand
	{
		Ref<IFramebuffer> TargetFramebuffer = nullptr;
	};

	class IGraphicsCommand
	{
	  public:
		virtual ~IGraphicsCommand()													   = default;
		virtual void Execute(CommandExecutor *executor, IGraphicsDevice *device) const = 0;
	};

	class SetVertexBufferCommandImpl final : public IGraphicsCommand
	{
	  public:
		SetVertexBufferCommandImpl(const SetVertexBufferCommand &command);
		~SetVertexBufferCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		SetVertexBufferCommand m_Command = {};
	};

	class SetIndexBufferCommandImpl final : public IGraphicsCommand
	{
	  public:
		SetIndexBufferCommandImpl(const IndexBufferView &view);
		~SetIndexBufferCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		IndexBufferView m_BufferView = {};
	};

	class SetPipelineCommandImpl final : public IGraphicsCommand
	{
	  public:
		SetPipelineCommandImpl(Ref<Pipeline> pipeline);
		~SetPipelineCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		Ref<Pipeline> m_Pipeline = nullptr;
	};

	class DrawCommandImpl final : public IGraphicsCommand
	{
	  public:
		DrawCommandImpl(const DrawDescription &desc);
		~DrawCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DrawDescription m_DrawDesc = {};
	};

	class DrawIndexedCommandImpl final : public IGraphicsCommand
	{
	  public:
		DrawIndexedCommandImpl(const DrawIndexedDescription &desc);
		~DrawIndexedCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DrawIndexedDescription m_DrawDesc = {};
	};

	class DrawIndirectCommandImpl final : public IGraphicsCommand
	{
	  public:
		DrawIndirectCommandImpl(const DrawIndirectDescription &desc);
		~DrawIndirectCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DrawIndirectDescription m_DrawDesc = {};
	};

	class DrawIndexedIndirectCommandImpl final : public IGraphicsCommand
	{
	  public:
		DrawIndexedIndirectCommandImpl(const DrawIndirectIndexedDescription &desc);
		~DrawIndexedIndirectCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DrawIndirectIndexedDescription m_DrawDesc = {};
	};

	class DrawMeshCommandImpl final : public IGraphicsCommand
	{
	  public:
		DrawMeshCommandImpl(const DrawMeshDescription &desc);
		~DrawMeshCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DrawMeshDescription m_DrawDesc = {};
	};

	class DrawMeshIndirectCommandImpl final : public IGraphicsCommand
	{
	  public:
		DrawMeshIndirectCommandImpl(const DrawMeshIndirectDescription &desc);
		~DrawMeshIndirectCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DrawMeshIndirectDescription m_DrawDesc = {};
	};

	class DispatchCommandImpl : public IGraphicsCommand
	{
	  public:
		DispatchCommandImpl(const DispatchDescription &desc);
		~DispatchCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DispatchDescription m_DispatchDesc = {};
	};

	class DispatchIndirectCommandImpl : public IGraphicsCommand
	{
	  public:
		DispatchIndirectCommandImpl(const DispatchIndirectDescription &desc);
		~DispatchIndirectCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DispatchIndirectDescription m_DispatchDesc = {};
	};

	class TraceRaysCommandImpl : public IGraphicsCommand
	{
	  public:
		TraceRaysCommandImpl(const TraceRaysDescription &desc);
		~TraceRaysCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		TraceRaysDescription m_TraceRaysDesc = {};
	};

	class SetResourceSetCommandImpl : public IGraphicsCommand
	{
	  public:
		SetResourceSetCommandImpl(const ResourceSetBindingDescription &desc);
		~SetResourceSetCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		ResourceSetBindingDescription m_ResourceSetBindings = {};
	};

	class ClearColourTargetCommandImpl : public IGraphicsCommand
	{
	  public:
		ClearColourTargetCommandImpl(const ClearColorTargetCommand &desc);
		~ClearColourTargetCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		ClearColorTargetCommand m_CommandData = {};
	};

	class ClearDepthStencilTargetCommandImpl : public IGraphicsCommand
	{
	  public:
		ClearDepthStencilTargetCommandImpl(const ClearDepthStencilTargetCommand &desc);
		~ClearDepthStencilTargetCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		ClearDepthStencilTargetCommand m_CommandData = {};
	};

	class SetFramebufferCommandImpl : public IGraphicsCommand
	{
	  public:
		SetFramebufferCommandImpl(Ref<IFramebuffer> framebuffer);
		~SetFramebufferCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		Ref<IFramebuffer> m_Framebuffer = {};
	};

	class SetViewportCommandImpl : public IGraphicsCommand
	{
	  public:
		SetViewportCommandImpl(const Viewport &viewport);
		~SetViewportCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		Viewport m_Viewport = {};
	};

	class SetScissorCommandImpl : public IGraphicsCommand
	{
	  public:
		SetScissorCommandImpl(const Scissor &scissor);
		~SetScissorCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		Scissor m_Scissor = {};
	};

	class ResolveFramebufferCommandImpl : public IGraphicsCommand
	{
	  public:
		ResolveFramebufferCommandImpl(const ResolveTextureDescription &desc);
		~ResolveFramebufferCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		ResolveTextureDescription m_CommandData = {};
	};

	class StartTimingQueryCommandImpl : public IGraphicsCommand
	{
	  public:
		StartTimingQueryCommandImpl(ITimingQuery *query);
		~StartTimingQueryCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		ITimingQuery *m_Query = nullptr;
	};

	class EndTimingQueryCommandImpl : public IGraphicsCommand
	{
	  public:
		EndTimingQueryCommandImpl(ITimingQuery *query);
		~EndTimingQueryCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		ITimingQuery *m_Query = nullptr;
	};

	class CopyBufferToBufferCommandImpl : public IGraphicsCommand
	{
	  public:
		CopyBufferToBufferCommandImpl(const BufferCopyDescription &desc);
		~CopyBufferToBufferCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		BufferCopyDescription m_Desc = {};
	};

	class CopyBufferToTextureCommandImpl : public IGraphicsCommand
	{
	  public:
		CopyBufferToTextureCommandImpl(const BufferTextureCopyDescription &desc);
		~CopyBufferToTextureCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		BufferTextureCopyDescription m_Desc = {};
	};

	class CopyTextureToBufferCommandImpl : public IGraphicsCommand
	{
	  public:
		CopyTextureToBufferCommandImpl(const BufferTextureCopyDescription &desc);
		~CopyTextureToBufferCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		BufferTextureCopyDescription m_Desc = {};
	};

	class CopyTextureToTextureCommandImpl : public IGraphicsCommand
	{
	  public:
		CopyTextureToTextureCommandImpl(const TextureCopyDescription &desc);
		~CopyTextureToTextureCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		TextureCopyDescription m_Desc = {};
	};

	class BeginDebugGroupCommandImpl : public IGraphicsCommand
	{
	  public:
		BeginDebugGroupCommandImpl(const std::string &name);
		~BeginDebugGroupCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		std::string m_Name = {};
	};

	class EndDebugGroupCommandImpl : public IGraphicsCommand
	{
	  public:
		~EndDebugGroupCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;
	};

	class InsertDebugMarkerCommandImpl : public IGraphicsCommand
	{
	  public:
		InsertDebugMarkerCommandImpl(const std::string &name);
		~InsertDebugMarkerCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		std::string m_Name = {};
	};

	class SetBlendFactorCommandImpl : public IGraphicsCommand
	{
	  public:
		SetBlendFactorCommandImpl(const BlendFactorDesc &desc);
		~SetBlendFactorCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		BlendFactorDesc m_CommandData = {};
	};

	class SetStencilReferenceCommandImpl : public IGraphicsCommand
	{
	  public:
		SetStencilReferenceCommandImpl(uint32_t stencilReference);
		~SetStencilReferenceCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		uint32_t m_StencilReference = {};
	};

	class BuildAccelerationStructuresCommandImpl : public IGraphicsCommand
	{
	  public:
		BuildAccelerationStructuresCommandImpl(const std::vector<AccelerationStructureGeometryBuildDescription> &description);
		~BuildAccelerationStructuresCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		std::vector<AccelerationStructureGeometryBuildDescription> m_Description = {};
	};

	class CopyAccelerationStructuresCommandImpl : public IGraphicsCommand
	{
	  public:
		CopyAccelerationStructuresCommandImpl(const AccelerationStructureCopyDescription &description);
		~CopyAccelerationStructuresCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		AccelerationStructureCopyDescription m_Description = {};
	};

	class CopyAccelerationStructureToDeviceBufferCommandImpl : public IGraphicsCommand
	{
	  public:
		CopyAccelerationStructureToDeviceBufferCommandImpl(const AccelerationStructureDeviceBufferCopyDescription &description);
		~CopyAccelerationStructureToDeviceBufferCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		AccelerationStructureDeviceBufferCopyDescription m_Description = {};
	};

	class CopyDeviceBufferToAccelerationStructureCommandImpl : public IGraphicsCommand
	{
	  public:
		CopyDeviceBufferToAccelerationStructureCommandImpl(const DeviceBufferAccelerationStructureCopyDescription &description);
		~CopyDeviceBufferToAccelerationStructureCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		DeviceBufferAccelerationStructureCopyDescription m_Description = {};
	};

	class PushConstantsCommandImpl : public IGraphicsCommand
	{
	  public:
		PushConstantsCommandImpl(const PushConstantsDesc &description);
		~PushConstantsCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		PushConstantsDesc m_Description = {};
	};

	class SubmitBarriersCommandImpl : public IGraphicsCommand
	{
	  public:
		SubmitBarriersCommandImpl(const BarrierGroupDescription &description);
		~SubmitBarriersCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		BarrierGroupDescription m_Description = {};
	};

	class EndRenderingCommandImpl : public IGraphicsCommand
	{
	  public:
		EndRenderingCommandImpl(const EndRenderingCommand &command);
		~EndRenderingCommandImpl() final = default;
		void Execute(CommandExecutor *executor, IGraphicsDevice *device) const final;

	  private:
		EndRenderingCommand m_CommandData = {};
	};

	struct CommandListDescription
	{
		std::string DebugName					= "CommandList";
		bool		AutomaticBarrierTransitions = true;
	};

	/// @brief A class representing a command list
	class NX_RHI_API ICommandList
	{
	  public:
		/// @brief A constructor creating a new command list
		ICommandList(const CommandListDescription &spec);

		/// @brief A virtual destructor allowing resources to be cleaned up
		virtual ~ICommandList();

		/// @brief A method that begins a command list
		/// @param beginInfo A parameter containing information about how to begin the
		/// command list
		void Begin();

		/// @brief A method that ends a command list
		void End();

		void BeginAutomaticBarrierManagement();

		void EndAutomaticBarrierManagement();

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

		void TraceRays(const TraceRaysDescription &desc);

		void SetResourceSet(const ResourceSetBindingDescription &desc);

		void ClearColourTarget(uint32_t index, const ClearColourValue &color, ClearRect clearRect);

		void ClearColourTarget(uint32_t index, const ClearColourValue &color);

		void ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect);

		void ClearDepthTarget(const ClearDepthStencilValue &value);

		void SetFramebuffer(Ref<IFramebuffer> framebuffer);

		void SetViewport(const Viewport &viewport);

		void SetScissor(const Scissor &scissor);

		void ResolveFramebuffer(const ResolveTextureDescription &desc);

		void StartTimingQuery(ITimingQuery *query);

		void StopTimingQuery(ITimingQuery *query);

		void CopyBufferToBuffer(const BufferCopyDescription &bufferCopy);

		void CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy);

		void CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy);

		void CopyTextureToTexture(const TextureCopyDescription &textureCopy);

		void BeginDebugGroup(const std::string &name);

		void EndDebugGroup();

		void InsertDebugMarker(const std::string &name);

		void SetBlendFactor(const BlendFactorDesc &blendFactor);

		void SetStencilReference(uint32_t stencilReference);

		void BuildAccelerationStructures(const std::vector<AccelerationStructureGeometryBuildDescription> &description);

		void CopyAccelerationStructure(const AccelerationStructureCopyDescription &description);

		void CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description);

		void CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description);

		void WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset);

		void SubmitMemoryBarrier(const MemoryBarrierDesc &desc);

		void SubmitTextureBarrier(const TextureBarrierDesc &desc);

		void SubmitBufferBarrier(const BufferBarrierDesc &desc);

		void FlushBarriers();

		const CommandListDescription &GetDescription();

		const std::vector<std::unique_ptr<IGraphicsCommand>> &GetCommands();

		bool IsRecording() const;

	  private:
		void EndRendering();
		void PushError(const std::string &message);

	  private:
		CommandListDescription			 m_Description				= {};
		mutable std::mutex				 m_Mutex					= {};
		std::atomic<bool>				 m_Started					= false;
		std::atomic<uint32_t>			 m_DebugGroups				= 0;
		Ref<IFramebuffer>				 m_CurrentFramebuffer		= nullptr;
		bool							 m_AutomaticBarrierTracking = false;
		BarrierGroupDescription			 m_Barriers					= {};
		std::function<void(std::string)> m_CallbackFunction			= {};

		std::vector<std::unique_ptr<IGraphicsCommand>> m_CommandImpls = {};
	};

	/// @brief A typedef to simplify creating function pointers to render commands
	typedef void (*RenderCommand)(Ref<ICommandList> commandList);

	class ScopedDebugGroup
	{
	  public:
		ScopedDebugGroup(const std::string &name, Ref<ICommandList> commandList) : m_CommandList(commandList)
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
		Ref<ICommandList> m_CommandList = nullptr;
	};
}	 // namespace Nexus::Graphics