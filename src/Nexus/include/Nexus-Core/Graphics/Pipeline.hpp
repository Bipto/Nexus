#pragma once

#include "RenderTarget.hpp"
#include "ResourceSet.hpp"
#include "Scissor.hpp"
#include "ShaderModule.hpp"
#include "ShaderResources.hpp"
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

		/// @brief The vertex shader to use with the pipeline
		Ref<ShaderModule> VertexModule = nullptr;

		/// @brief The debug name of the pipeline, shows in graphics debuggers
		std::string DebugName = "Graphics Pipeline";
	};

	struct ComputePipelineDescription
	{
		Ref<ShaderModule> ComputeShader = nullptr;

		/// @brief The debug name of the pipeline, shows in graphics debuggers
		std::string DebugName = "Compute Pipeline";
	};

	struct MeshletPipelineDescription
	{
		/// @brief How the pipeline should handle depth and stencil testing
		DepthStencilDescription DepthStencilDesc;

		/// @brief How the pipeline should handle rasterization
		RasterizerStateDescription RasterizerStateDesc;

		/// @brief How the pipeline should handle the vertex buffer data
		Topology PrimitiveTopology = Topology::TriangleList;

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

		/// @brief The fragment shader that will be used with the pipeline
		Ref<ShaderModule> FragmentModule = nullptr;

		/// @brief The mesh shader to use with the pipeline
		Ref<ShaderModule> MeshModule = nullptr;

		/// @brief The task shader to use with the pipeline (optional)
		Ref<ShaderModule> TaskModule = nullptr;

		/// @brief The debug name of the pipeline, shows in graphics debuggers
		std::string DebugName = "Meshlet Pipeline";
	};

	enum class ShaderGroupType
	{
		General,
		Triangles,
		Procedural
	};

	struct ShaderGroup
	{
		ShaderGroupType Type			   = ShaderGroupType::General;
		uint32_t		GeneralShader	   = ~0U;
		uint32_t		ClosestHitShader   = ~0U;
		uint32_t		AnyHitShader	   = ~0U;
		uint32_t		IntersectionShader = ~0U;
	};

	struct RayTracingPipelineDescription
	{
		std::vector<Ref<ShaderModule>> Shaders			 = {};
		std::vector<ShaderGroup>	   ShaderGroups		 = {};
		uint32_t					   MaxRecursionDepth = 0;
		/// @brief The debug name of the pipeline, shows in graphics debuggers
		std::string DebugName = "Ray Tracing Pipeline";
	};

	enum class PipelineType
	{
		Graphics,
		Compute,
		Meshlet,
		RayTracing
	};

	inline ShaderResource ReflectedShaderResourceToShaderResource(const ReflectedResource &resource, ShaderStage stage)
	{
		ShaderResource output;

		switch (resource.Type)
		{
			case ReflectedShaderDataType::StorageImage:
			{
				output.Type = ResourceType::StorageImage;
				break;
			}
			case ReflectedShaderDataType::Texture:
			{
				output.Type = ResourceType::Texture;
				break;
			}
			case ReflectedShaderDataType::UniformTextureBuffer:
			{
				output.Type = ResourceType::UniformTextureBuffer;
				break;
			}
			case ReflectedShaderDataType::StorageTextureBuffer:
			{
				output.Type = ResourceType::StorageTextureBuffer;
				break;
			}
			case ReflectedShaderDataType::Sampler:
			{
				output.Type = ResourceType::Sampler;
				break;
			}
			case ReflectedShaderDataType::ComparisonSampler:
			{
				output.Type = ResourceType::ComparisonSampler;
				break;
			}
			case ReflectedShaderDataType::CombinedImageSampler:
			{
				output.Type = ResourceType::CombinedImageSampler;
				break;
			}
			case ReflectedShaderDataType::UniformBuffer:
			{
				output.Type = ResourceType::UniformBuffer;
				break;
			}
			case ReflectedShaderDataType::StorageBuffer:
			{
				output.Type = ResourceType::StorageBuffer;
				break;
			}
			case ReflectedShaderDataType::AccelerationStructure:
			{
				output.Type = ResourceType::AccelerationStructure;
				break;
			}
			default: throw std::runtime_error("Failed to find a valid resource type");
		}

		output.Access		 = resource.StorageResourceAccess;
		output.Name			 = resource.Name;
		output.Set			 = resource.DescriptorSet;
		output.Binding		 = resource.BindingPoint;
		output.ResourceCount = resource.BindingCount;
		output.RegisterSpace = resource.RegisterSpace;
		output.Stage.AddFlag(stage);
		output.Dimension = resource.Dimension;

		return output;
	}

	class Pipeline
	{
	  public:
		virtual ~Pipeline()
		{
		}

		virtual PipelineType				   GetType() const		   = 0;
		virtual std::vector<Ref<ShaderModule>> GetShaderStages() const = 0;

		std::map<std::string, ShaderResource> GetRequiredShaderResources() const
		{
			std::map<std::string, ShaderResource> requiredResources;

			std::vector<Ref<ShaderModule>> shaderStages = GetShaderStages();
			for (Ref<ShaderModule> module : shaderStages)
			{
				ShaderReflectionData reflectionData = module->Reflect();
				for (const auto &resource : reflectionData.Resources)
				{
					if (requiredResources.find(resource.Name) != requiredResources.end())
					{
						ShaderResource &requiredResource = requiredResources.at(resource.Name);
						ShaderResource	newResource		 = ReflectedShaderResourceToShaderResource(resource, module->GetShaderStage());
						NX_ASSERT(newResource == requiredResource, "Shader resources that share the same name must match across stages");
						requiredResource.Stage.AddFlag(module->GetShaderStage());
					}
					else
					{
						ShaderResource newResource		 = ReflectedShaderResourceToShaderResource(resource, module->GetShaderStage());
						requiredResources[resource.Name] = newResource;
					}
				}
			}

			return requiredResources;
		}
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

		virtual PipelineType GetType() const final
		{
			return PipelineType::Graphics;
		}

		std::vector<Ref<ShaderModule>> GetShaderStages() const final
		{
			std::vector<Ref<ShaderModule>> stages;

			if (m_Description.VertexModule)
			{
				stages.push_back(m_Description.VertexModule);
			}

			if (m_Description.FragmentModule)
			{
				stages.push_back(m_Description.FragmentModule);
			}

			if (m_Description.GeometryModule)
			{
				stages.push_back(m_Description.GeometryModule);
			}

			if (m_Description.TesselationControlModule)
			{
				stages.push_back(m_Description.TesselationControlModule);
			}

			if (m_Description.TesselationEvaluationModule)
			{
				stages.push_back(m_Description.TesselationEvaluationModule);
			}

			return stages;
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

		std::vector<Ref<ShaderModule>> GetShaderStages() const final
		{
			std::vector<Ref<ShaderModule>> stages;

			if (m_Description.ComputeShader)
			{
				stages.push_back(m_Description.ComputeShader);
			}

			return stages;
		}

	  protected:
		ComputePipelineDescription m_Description = {};
	};

	class MeshletPipeline : public Pipeline
	{
	  public:
		MeshletPipeline(const MeshletPipelineDescription &description) : m_Description(description)
		{
		}

		virtual ~MeshletPipeline()
		{
		}

		const MeshletPipelineDescription &GetPipelineDescription()
		{
			return m_Description;
		}

		virtual PipelineType GetType() const final
		{
			return PipelineType::Compute;
		}

		std::vector<Ref<ShaderModule>> GetShaderStages() const final
		{
			std::vector<Ref<ShaderModule>> stages;

			if (m_Description.TaskModule)
			{
				stages.push_back(m_Description.TaskModule);
			}

			if (m_Description.MeshModule)
			{
				stages.push_back(m_Description.MeshModule);
			}

			if (m_Description.FragmentModule)
			{
				stages.push_back(m_Description.FragmentModule);
			}

			return stages;
		}

	  protected:
		MeshletPipelineDescription m_Description = {};
	};

	class RayTracingPipeline : public Pipeline
	{
	  public:
		RayTracingPipeline(const RayTracingPipelineDescription &description) : m_Description(description)
		{
		}

		virtual ~RayTracingPipeline()
		{
		}

		const RayTracingPipelineDescription &GetPipelineDescription()
		{
			return m_Description;
		}

		virtual PipelineType GetType() const final
		{
			return PipelineType::RayTracing;
		}

		std::vector<Ref<ShaderModule>> GetShaderStages() const final
		{
			return {};
		}

	  protected:
		RayTracingPipelineDescription m_Description = {};
	};
}	 // namespace Nexus::Graphics