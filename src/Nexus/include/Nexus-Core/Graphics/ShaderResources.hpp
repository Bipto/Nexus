#pragma once

#include "Nexus-Core/nxpch.hpp"

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

		// GLSL
		Read,
		Write,
		ReadWrite,

		// HLSL
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
		UniformTextureBuffer,
		StorageTextureBuffer,
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

	enum class ResourceType
	{
		StorageImage,
		Texture,
		UniformTextureBuffer,
		StorageTextureBuffer,
		Sampler,
		ComparisonSampler,
		CombinedImageSampler,
		UniformBuffer,
		StorageBuffer,
		AccelerationStructure
	};

	enum class ShaderStage
	{
		Invalid = 0,
		Compute,
		Fragment,
		Geometry,
		TessellationControl,
		TessellationEvaluation,
		Vertex,
		RayGeneration,
		RayMiss,
		RayClosestHit,
		RayAnyHit,
		RayIntersection,
		Mesh,
		Task
	};

	class ShaderStageFlags
	{
	  public:
		ShaderStageFlags()			= default;
		virtual ~ShaderStageFlags() = default;

		inline void AddFlag(ShaderStage stage)
		{
			m_ShaderStages.push_back(stage);
		}

		inline bool HasFlag(ShaderStage stage) const
		{
			return std::find(m_ShaderStages.begin(), m_ShaderStages.end(), stage) != m_ShaderStages.end();
		}

		inline bool IsEmpty() const
		{
			return m_ShaderStages.empty();
		}

		inline size_t GetCount() const
		{
			return m_ShaderStages.size();
		}

	  private:
		std::vector<ShaderStage> m_ShaderStages = {};
	};

	struct ShaderResource
	{
		ResourceType	 Type		   = ResourceType::StorageImage;
		std::string		 Name		   = "Resource";
		uint32_t		 Set		   = 0;
		uint32_t		 Binding	   = 0;
		uint32_t		 RegisterSpace = 0;
		uint32_t		 ResourceCount = 0;
		ShaderStageFlags Stage		   = {};
		StorageResourceAccess Access		= StorageResourceAccess::None;
		ResourceDimension	  Dimension		= ResourceDimension::None;
	};

	inline bool operator==(const ShaderResource &lhs, const ShaderResource &rhs)
	{
		return std::tie(lhs.Type, lhs.Name, lhs.Set, lhs.Binding, lhs.RegisterSpace, lhs.ResourceCount) ==
			   std::tie(rhs.Type, rhs.Name, rhs.Set, rhs.Binding, rhs.RegisterSpace, rhs.ResourceCount);
	}

	inline bool operator<(const ShaderResource &lhs, const ShaderResource &rhs)
	{
		return std::tie(lhs.Type, lhs.Name, lhs.Set, lhs.Binding, lhs.RegisterSpace, lhs.ResourceCount) <
			   std::tie(rhs.Type, rhs.Name, rhs.Set, rhs.Binding, rhs.RegisterSpace, rhs.ResourceCount);
	}
}	 // namespace Nexus::Graphics
