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
	struct PipelineDescription
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
		SampleCount ColourTargetSampleCount = SampleCount::SampleCount1;

		/// @brief The depth format that will be used with the pipeline
		PixelFormat DepthFormat = PixelFormat::D24_UNorm_S8_UInt;

		/// @brief The layout of data within bound vertex buffers
		std::vector<VertexBufferLayout> Layouts;

		/// @brief The fragment shader that will be used with the pipeline
		Ref<ShaderModule> FragmentModule			  = nullptr;

		/// @brief The geometry shader to use with the pipeline (optional)
		Ref<ShaderModule> GeometryModule			  = nullptr;

		/// @brief The tesselation control shader to use with the pipeline (optional)
		Ref<ShaderModule> TesselationControlModule	  = nullptr;

		/// @brief The tesselation evaluation shader to use with the pipeline (optional)
		Ref<ShaderModule> TesselationEvaluationModule = nullptr;

		/// @brief The vertex shader to use with the pipeline (optional)
		Ref<ShaderModule> VertexModule				  = nullptr;
	};

	/// @brief A pure virtual class representing an API specific pipeline
	class Pipeline
	{
	  public:
		/// @brief A constructor that takes in a PipelineDescription object to use for
		/// creation
		Pipeline(const PipelineDescription &description)
		{
			m_Description = description;
		}

		/// @brief An empty pipeline cannot be created
		Pipeline() = delete;

		/// @brief Virtual destructor allowing API specific resources to be destroyed
		 virtual ~Pipeline() {};

		 /// @brief A pure virtual method returning a const reference to a pipeline
		 /// description
		 /// @return A const reference to a pipelinedescription
		 virtual const PipelineDescription &GetPipelineDescription() const = 0;

		 bool HasResources() const
		 {
			 return m_Description.ResourceSetSpec.SampledImages.size() > 0 || m_Description.ResourceSetSpec.UniformBuffers.size() > 0;
		}

	  protected:
		/// @brief The pipeline description used to create the pipeline
		PipelineDescription m_Description;

	  private:
		/// @brief This allows the GraphicsDevice to access all data stored within a
		/// pipeline
		friend class GraphicsDevice;
	};
}	 // namespace Nexus::Graphics