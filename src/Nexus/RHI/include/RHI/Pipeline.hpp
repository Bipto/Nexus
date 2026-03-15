#pragma once

#include <array>
#include <vector>

#include "RHI/ResourceSet.hpp"
#include "RHI/Scissor.hpp"
#include "RHI/ShaderModule.hpp"
#include "RHI/ShaderResources.hpp"
#include "RHI/Structures.hpp"
#include "RHI/VertexBufferLayout.hpp"
#include "RHI/Viewport.hpp"

namespace Nexus::Graphics
{
	constexpr size_t MaxVertexBufferBindings = 8;
	constexpr size_t MaxViewports			 = 8;
	constexpr size_t MaxScissors			 = 8;

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
		uint32_t Samples = 1;

		/// @brief Controls which MSAA samples are active or written
		uint32_t SampleMask = 0xFFFFFFFF;

		/// @brief The depth format that will be used with the pipeline
		PixelFormat DepthFormat = PixelFormat::D24_UNorm_S8_UInt;

		/// @brief The layout of data within bound vertex buffers
		std::vector<VertexBufferLayout> Layouts;

		/// @brief The fragment shader that will be used with the pipeline
		Ref<IShaderModule> FragmentModule = nullptr;

		/// @brief The geometry shader to use with the pipeline (optional)
		Ref<IShaderModule> GeometryModule = nullptr;

		/// @brief The tesselation control shader to use with the pipeline (optional)
		Ref<IShaderModule> TesselationControlModule = nullptr;

		/// @brief The tesselation evaluation shader to use with the pipeline (optional)
		Ref<IShaderModule> TesselationEvaluationModule = nullptr;

		/// @brief The vertex shader to use with the pipeline
		Ref<IShaderModule> VertexModule = nullptr;

		/// @brief The debug name of the pipeline, shows in graphics debuggers
		std::string DebugName = "Graphics Pipeline";

		/// @brief A struct representing the layout of shader resources
		ResourceSetDescription ResourceDescription = {};
	};

	struct ComputePipelineDescription
	{
		Ref<IShaderModule> ComputeShader = nullptr;

		/// @brief The debug name of the pipeline, shows in graphics debuggers
		std::string DebugName = "Compute Pipeline";

		/// @brief A struct representing the layout of shader resources
		ResourceSetDescription ResourceDescription = {};
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
		uint32_t Samples = 1;

		/// @brief Controls which MSAA samples are active or written
		uint32_t SampleMask = 0xFFFFFFFF;

		/// @brief The depth format that will be used with the pipeline
		PixelFormat DepthFormat = PixelFormat::D24_UNorm_S8_UInt;

		/// @brief The fragment shader that will be used with the pipeline
		Ref<IShaderModule> FragmentModule = nullptr;

		/// @brief The mesh shader to use with the pipeline
		Ref<IShaderModule> MeshModule = nullptr;

		/// @brief The task shader to use with the pipeline (optional)
		Ref<IShaderModule> TaskModule = nullptr;

		/// @brief The debug name of the pipeline, shows in graphics debuggers
		std::string DebugName = "Meshlet Pipeline";

		/// @brief A struct representing the layout of shader resources
		ResourceSetDescription ResourceDescription = {};
	};

	enum class ShaderGroupType
	{
		General,
		Triangles,
		Procedural
	};

#define NX_SHADER_UNUSED (~0U)

	struct ShaderGroup
	{
		ShaderGroupType Type			   = ShaderGroupType::General;
		uint32_t		GeneralShader	   = NX_SHADER_UNUSED;
		uint32_t		ClosestHitShader   = NX_SHADER_UNUSED;
		uint32_t		AnyHitShader	   = NX_SHADER_UNUSED;
		uint32_t		IntersectionShader = NX_SHADER_UNUSED;
	};

	struct RayTracingPipelineDescription
	{
		std::vector<Ref<IShaderModule>> Shaders			  = {};
		std::vector<ShaderGroup>		ShaderGroups	  = {};
		uint32_t						MaxRecursionDepth = 0;

		/// @brief The debug name of the pipeline, shows in graphics debuggers
		std::string DebugName = "Ray Tracing Pipeline";

		/// @brief A struct representing the layout of shader resources
		ResourceSetDescription ResourceDescription = {};
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
			case ReflectedShaderDataType::PushConstants:
			{
				output.Type = ResourceType::PushConstants;
				break;
			}
			default: throw std::runtime_error("Failed to find a valid resource type");
		}

		output.Access		 = resource.ResourceAccess;
		output.Name			 = !resource.BlockName.empty() ? resource.BlockName : resource.InstanceName;
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

		virtual PipelineType					GetType() const					  = 0;
		virtual std::vector<Ref<IShaderModule>> GetShaderStages() const			  = 0;
		virtual const ResourceSetDescription   &GetResourceSetDescription() const = 0;

		std::map<std::string, ShaderResource> GetRequiredShaderResources() const
		{
			std::map<std::string, ShaderResource> requiredResources;

			std::vector<Ref<IShaderModule>> shaderStages = GetShaderStages();
			for (Ref<IShaderModule> module : shaderStages)
			{
				ShaderReflectionData reflectionData = module->Reflect();
				for (const auto &resource : reflectionData.Resources)
				{
					std::string resourceName = !resource.BlockName.empty() ? resource.BlockName : resource.InstanceName;

					if (requiredResources.find(resourceName) != requiredResources.end())
					{
						ShaderResource &requiredResource = requiredResources.at(resourceName);
						ShaderResource	newResource		 = ReflectedShaderResourceToShaderResource(resource, module->GetShaderStage());

						if (newResource == requiredResource)
						{
							throw std::runtime_error("Shader resources that share the same name must match across stages");
						}

						requiredResource.Stage.AddFlag(module->GetShaderStage());
					}
					else
					{
						ShaderResource newResource		= ReflectedShaderResourceToShaderResource(resource, module->GetShaderStage());
						requiredResources[resourceName] = newResource;
					}
				}
			}

			return requiredResources;
		}
	};

	/// @brief A pure virtual class representing an API specific pipeline
	class IGraphicsPipeline : public Pipeline
	{
	  public:
		/// @brief A constructor that takes in a PipelineDescription object to use for
		/// creation
		IGraphicsPipeline(const GraphicsPipelineDescription &description) : m_Description(description)
		{
		}

		/// @brief An empty pipeline cannot be created
		IGraphicsPipeline() = delete;

		/// @brief Virtual destructor allowing API specific resources to be destroyed
		virtual ~IGraphicsPipeline()
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

		const ResourceSetDescription &GetResourceSetDescription() const final
		{
			const GraphicsPipelineDescription &desc = GetPipelineDescription();
			return desc.ResourceDescription;
		}

		std::vector<Ref<IShaderModule>> GetShaderStages() const final
		{
			std::vector<Ref<IShaderModule>> stages;

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

	class IComputePipeline : public Pipeline
	{
	  public:
		IComputePipeline(const ComputePipelineDescription &description) : m_Description(description)
		{
		}

		IComputePipeline() = delete;

		virtual ~IComputePipeline()
		{
		}

		const ComputePipelineDescription &GetPipelineDescription() const
		{
			return m_Description;
		}

		const ResourceSetDescription &GetResourceSetDescription() const final
		{
			const ComputePipelineDescription &desc = GetPipelineDescription();
			return desc.ResourceDescription;
		}

		virtual PipelineType GetType() const final
		{
			return PipelineType::Compute;
		}

		std::vector<Ref<IShaderModule>> GetShaderStages() const final
		{
			std::vector<Ref<IShaderModule>> stages;

			if (m_Description.ComputeShader)
			{
				stages.push_back(m_Description.ComputeShader);
			}

			return stages;
		}

	  protected:
		ComputePipelineDescription m_Description = {};
	};

	class IMeshletPipeline : public Pipeline
	{
	  public:
		IMeshletPipeline(const MeshletPipelineDescription &description) : m_Description(description)
		{
		}

		virtual ~IMeshletPipeline()
		{
		}

		const MeshletPipelineDescription &GetPipelineDescription() const
		{
			return m_Description;
		}

		const ResourceSetDescription &GetResourceSetDescription() const final
		{
			const MeshletPipelineDescription &desc = GetPipelineDescription();
			return desc.ResourceDescription;
		}

		virtual PipelineType GetType() const final
		{
			return PipelineType::Compute;
		}

		std::vector<Ref<IShaderModule>> GetShaderStages() const final
		{
			std::vector<Ref<IShaderModule>> stages;

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

	class IRayTracingPipeline : public Pipeline
	{
	  public:
		IRayTracingPipeline(const RayTracingPipelineDescription &description) : m_Description(description)
		{
		}

		virtual ~IRayTracingPipeline()
		{
		}

		const RayTracingPipelineDescription &GetPipelineDescription() const
		{
			return m_Description;
		}

		const ResourceSetDescription &GetResourceSetDescription() const final
		{
			const RayTracingPipelineDescription &desc = GetPipelineDescription();
			return desc.ResourceDescription;
		}

		virtual PipelineType GetType() const final
		{
			return PipelineType::RayTracing;
		}

		virtual std::vector<uint8_t> GetRayTracingShaderGroupHandles() const = 0;

		std::vector<Ref<IShaderModule>> GetShaderStages() const final
		{
			return m_Description.Shaders;
		}

	  protected:
		RayTracingPipelineDescription m_Description = {};
	};
}	 // namespace Nexus::Graphics