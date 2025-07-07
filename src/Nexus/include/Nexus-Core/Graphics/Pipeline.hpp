#pragma once

#include "RenderTarget.hpp"
#include "ResourceSet.hpp"
#include "Scissor.hpp"
#include "ShaderModule.hpp"
#include "Structures.hpp"
#include "Viewport.hpp"

namespace Nexus::Graphics
{
	/// @brief A struct representing how all of the settings required to create a
	/// pipeline
	struct GraphicsPipelineDescription
	{
		/// @brief How the pipeline should handle depth and stencil testing
		DepthStencilDescription DepthStencilDesc;

		/// @brief How the pipeline should handle rasterization
		RasterizerStateDescription RasterizerStateDesc;

		/// @brief How the pipeline should handle the vertex buffer data
		Topology PrimitiveTopology = Topology::TriangleList;

		/// @brief A resource set specification describing how resources are allocated
		/// in the pipeline
		ResourceSetSpecification ResourceSetSpec;

		/// @brief An array containing the colour formats that will be used with the pipeline
		std::array<PixelFormat, 8> ColourFormats;

		/// @brief An unsigned 32 bit integer representing the number of attachments that will be used with the pipeline
		uint32_t ColourTargetCount = 1;

		/// @brief An array containing the blend state for each attachment to be used with the pipelin
		std::array<BlendStateDescription, 8> ColourBlendStates;

		/// @brief How many samples will be used with the pipeline
		uint32_t ColourTargetSampleCount = 1;

		/// @brief The depth format that will be used with the pipeline
		PixelFormat DepthFormat = PixelFormat::D24_UNorm_S8_UInt;

		/// @brief The layout of data within bound vertex buffers
		std::vector<VertexBufferLayout> Layouts;

		/// @brief The fragment shader that will be used with the pipeline
		Ref<ShaderModule> FragmentModule = nullptr;

		/// @brief The geometry shader to use with the pipeline (optional)
		Ref<ShaderModule> GeometryModule = nullptr;

		/// @brief The tesselation control shader to use with the pipeline (optional)
		Ref<ShaderModule> TesselationControlModule = nullptr;

		/// @brief The tesselation evaluation shader to use with the pipeline (optional)
		Ref<ShaderModule> TesselationEvaluationModule = nullptr;

		/// @brief The vertex shader to use with the pipeline (optional)
		Ref<ShaderModule> VertexModule = nullptr;

		/// @brief The mesh shader to use with the pipeline (optional)
		Ref<ShaderModule> MeshModule = nullptr;

		/// @brief The task shader to use with the pipeline (optional)
		Ref<ShaderModule> TaskModule = nullptr;
	};

	struct ComputePipelineDescription
	{
		ResourceSetSpecification ResourceSetSpec = {};
		Ref<ShaderModule>		 ComputeShader	 = nullptr;
	};

	struct RayTracingPipelineDescription
	{
		ResourceSetSpecification ResourceSetSpec	   = {};
		Ref<ShaderModule>		 RayGenShader		   = nullptr;
		Ref<ShaderModule>		 RayMissShader		   = nullptr;
		Ref<ShaderModule>		 RayClosestHitShader   = nullptr;
		Ref<ShaderModule>		 RayAnyHitShader	   = nullptr;
		Ref<ShaderModule>		 RayIntersectionShader = nullptr;
		uint32_t				 MaxRecursionDepth	   = 0;
	};

	enum class PipelineType
	{
		Graphics,
		Compute
	};

	class Pipeline
	{
	  public:
		virtual ~Pipeline()
		{
		}

		virtual PipelineType			 GetType() const					 = 0;
		virtual ResourceSetSpecification GetResourceSetSpecification() const = 0;
	};

	/// @brief A pure virtual class representing an API specific pipeline
	class GraphicsPipeline : public Pipeline
	{
	  public:
		/// @brief A constructor that takes in a PipelineDescription object to use for
		/// creation
		GraphicsPipeline(const GraphicsPipelineDescription &description) : m_Description(description)
		{
		}

		/// @brief An empty pipeline cannot be created
		GraphicsPipeline() = delete;

		/// @brief Virtual destructor allowing API specific resources to be destroyed
		virtual ~GraphicsPipeline()
		{
		}

		/// @brief A pure virtual method returning a const reference to a pipeline
		/// description
		/// @return A const reference to a pipelinedescription
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const = 0;

		bool HasResources() const
		{
			return m_Description.ResourceSetSpec.SampledImages.size() > 0 || m_Description.ResourceSetSpec.UniformBuffers.size() > 0;
		}

		virtual PipelineType GetType() const final
		{
			return PipelineType::Graphics;
		}

		virtual ResourceSetSpecification GetResourceSetSpecification() const final
		{
			return m_Description.ResourceSetSpec;
		}

	  protected:
		/// @brief The pipeline description used to create the pipeline
		GraphicsPipelineDescription m_Description = {};
	};

	class ComputePipeline : public Pipeline
	{
	  public:
		ComputePipeline(const ComputePipelineDescription &description) : m_Description(description)
		{
		}

		ComputePipeline() = delete;

		virtual ~ComputePipeline()
		{
		}

		const ComputePipelineDescription &GetPipelineDescription() const
		{
			return m_Description;
		}

		virtual PipelineType GetType() const final
		{
			return PipelineType::Compute;
		}

		virtual ResourceSetSpecification GetResourceSetSpecification() const final
		{
			return m_Description.ResourceSetSpec;
		}

	  protected:
		ComputePipelineDescription m_Description = {};
	};
}	 // namespace Nexus::Graphics