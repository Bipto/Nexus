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

		/// @brief How the pipeline should handle blending
		BlendStateDescription BlendStateDesc;

		/// @brief How the pipeline should handle the vertex buffer data
		Topology PrimitiveTopology = Topology::TriangleList;

		/// @brief A resource set specification describing how resources are allocated
		/// in the pipeline
		ResourceSetSpecification ResourceSetSpec;

		std::array<PixelFormat, 8> ColourFormats;
		uint32_t				   ColourTargetCount	   = 1;
		SampleCount				   ColourTargetSampleCount = SampleCount::SampleCount1;
		PixelFormat				   DepthFormat			   = PixelFormat::D24_UNorm_S8_UInt;

		/// @brief The layout of data within bound vertex buffers
		std::vector<VertexBufferLayout> Layouts;

		Ref<ShaderModule> FragmentModule			  = nullptr;
		Ref<ShaderModule> GeometryModule			  = nullptr;
		Ref<ShaderModule> TesselationControlModule	  = nullptr;
		Ref<ShaderModule> TesselationEvaluationModule = nullptr;
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

	  protected:
		/// @brief The pipeline description used to create the pipeline
		PipelineDescription m_Description;

	  private:
		/// @brief This allows the GraphicsDevice to access all data stored within a
		/// pipeline
		friend class GraphicsDevice;
	};
}	 // namespace Nexus::Graphics