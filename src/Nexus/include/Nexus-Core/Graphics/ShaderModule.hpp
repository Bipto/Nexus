#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "ResourceSet.hpp"
#include "ShaderDataType.hpp"

namespace Nexus::Graphics
{
	enum class ResourceDimension
	{
		None,
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		Texture2DMS,
		Texture2DMSArray,
		Texture3D,
		TextureCube,
		TextureCubeArray,
		TextureRectangle
	};

	enum class StorageResourceAccess
	{
		None,
		Read,
		Write,
		ReadWrite,
		ReadByteAddress,
		ReadWriteByteAddress,
		ReadStructured,
		ReadWriteStructured,
		AppendStructured,
		ConsumeStructured,
		ReadWriteStructuredWithCounter,
		FeedbackTexture
	};

	enum class ReflectedShaderDataType
	{
		Bool,
		Bool2,
		Bool3,
		Bool4,

		Int,
		Int2,
		Int3,
		Int4,

		AtomicUint,

		UInt,
		UInt2,
		UInt3,
		UInt4,

		Half,
		Half2,
		Half3,
		Half4,

		Float,
		Float2,
		Float3,
		Float4,

		Double,
		Double2,
		Double3,
		Double4,

		Mat2x2,
		Mat2x3,
		Mat2x4,
		Mat3x2,
		Mat3x3,
		Mat3x4,
		Mat4x2,
		Mat4x3,
		Mat4x4,

		DMat2x2,
		DMat2x3,
		DMat2x4,
		DMat3x2,
		DMat3x3,
		DMat3x4,
		DMat4x2,
		DMat4x3,
		DMat4x4,

		FeedbackTexture,
		StorageImage,
		Texture,
		TextureBuffer,
		Sampler,
		ComparisonSampler,
		CombinedImageSampler,
		UniformBuffer,
		StorageBuffer,
		Struct,
		AccelerationStructure,
		Shared,
		InputAttachment,
		PushConstant,
		ShaderRecord
	};

	struct ReflectedResource
	{
		ReflectedShaderDataType Type				  = ReflectedShaderDataType::UniformBuffer;
		std::string				Name				  = {};
		ResourceDimension		Dimension			  = ResourceDimension::None;
		StorageResourceAccess	StorageResourceAccess = StorageResourceAccess::None;
		uint32_t				DescriptorSet		  = 0;
		uint32_t				BindingPoint		  = 0;
		uint32_t				BindingCount		  = 0;
		uint32_t				RegisterSpace		  = 0;
	};

	struct Attribute
	{
		std::string				Name		= {};
		ReflectedShaderDataType Type		= {};
		uint32_t				Binding		= {};
		uint32_t				StreamIndex = {};
	};

	struct ReflectedBufferMember
	{
		std::string				Name	  = {};
		size_t					Offset	  = 0;
		size_t					Size	  = 0;
		std::optional<uint32_t> ArraySize = {};
		ShaderDataType			Type	  = {};
	};

	struct ReflectedUniformBuffer
	{
		std::string						   Name	   = {};
		std::vector<ReflectedBufferMember> Members = {};
	};

	struct ReflectedStorageBuffer
	{
		std::string						   Name	   = {};
		std::vector<ReflectedBufferMember> Members = {};
	};

	struct ShaderReflectionData
	{
		std::vector<Attribute>				Inputs;
		std::vector<Attribute>				Outputs;
		std::vector<ReflectedUniformBuffer> UniformBuffers = {};
		std::vector<ReflectedStorageBuffer> StorageBuffers = {};
		std::vector<ReflectedResource>		Resources	   = {};
	};

	enum class ShaderStage
	{
		Invalid = 0,
		Compute,
		Fragment,
		Geometry,
		TesselationControl,
		TesselationEvaluation,
		Vertex,
		RayGeneration,
		RayMiss,
		RayClosestHit,
		RayAnyHit,
		RayIntersection,
		Mesh,
		Task
	};

	struct ShaderAttribute
	{
		std::string	   Name;
		ShaderDataType DataType;
	};

	struct ShaderModuleSpecification
	{
		std::string			  Name = "ShaderModule";
		std::string			  Source;
		ShaderStage			  ShadingStage = ShaderStage::Invalid;
		std::vector<uint32_t> SpirvBinary;

		std::vector<ShaderAttribute> InputAttributes;
		std::vector<ShaderAttribute> OutputAttributes;
	};

	class ShaderModule
	{
	  public:
		ShaderModule(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec)
			: m_ModuleSpecification(shaderModuleSpec),
			  m_ResourceSetSpecification(resourceSpec)
		{
		}

		virtual ~ShaderModule() = default;

		ShaderStage GetShaderStage() const
		{
			return m_ModuleSpecification.ShadingStage;
		}

		const ShaderModuleSpecification &GetModuleSpecification() const
		{
			return m_ModuleSpecification;
		}

		const ResourceSetSpecification &GetResourceSetSpecification() const
		{
			return m_ResourceSetSpecification;
		}

		virtual ShaderReflectionData Reflect() const = 0;

	  protected:
		ShaderModuleSpecification m_ModuleSpecification;
		ResourceSetSpecification  m_ResourceSetSpecification;
	};
}	 // namespace Nexus::Graphics