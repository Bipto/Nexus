#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "DeviceBufferOpenGL.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{
	class PipelineOpenGL
	{
	  public:
		virtual ~PipelineOpenGL()
		{
		}

		virtual void	 Bind()					 = 0;
		virtual uint32_t GetShaderHandle() const = 0;
	};

	class GraphicsPipelineOpenGL : public GraphicsPipeline
	{
	  public:
		GraphicsPipelineOpenGL(const GraphicsPipelineDescription &description);
		virtual ~GraphicsPipelineOpenGL();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;

		void BindBuffers(const std::map<uint32_t, VertexBufferView> &vertexBuffers,
						 std::optional<IndexBufferView>				 indexBuffer,
						 uint32_t									 vertexOffset,
						 uint32_t									 instanceOffset);

		virtual void	 Bind() final;
		virtual uint32_t GetShaderHandle() const final;

		void CreateVAO();
		void DestroyVAO();

	  private:
		void SetupDepthStencil();
		void SetupRasterizer();
		void SetupBlending();
		void SetShader();
		void CreateShader();

	  private:
		uint32_t m_VAO			= 0;
		uint32_t m_ShaderHandle = 0;
	};

	class ComputePipelineOpenGL : public ComputePipeline
	{
	  public:
		ComputePipelineOpenGL(const ComputePipelineDescription &description);
		virtual ~ComputePipelineOpenGL();
		virtual void	 Bind() final;
		virtual uint32_t GetShaderHandle() const final;

	  private:
		void CreateShader();

	  private:
		uint32_t m_ShaderHandle = 0;
	};
}	 // namespace Nexus::Graphics

#endif