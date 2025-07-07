#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "DeviceBufferOpenGL.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class PipelineOpenGL
	{
	  public:
		virtual ~PipelineOpenGL()
		{
		}
		virtual void	 Bind(const GladGLContext &context) = 0;
		virtual uint32_t GetShaderHandle() const = 0;
	};

	class GraphicsPipelineOpenGL : public GraphicsPipeline, public PipelineOpenGL
	{
	  public:
		GraphicsPipelineOpenGL(const GraphicsPipelineDescription &description, GraphicsDeviceOpenGL *device);
		virtual ~GraphicsPipelineOpenGL();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;

		void BindBuffers(const std::map<uint32_t, VertexBufferView> &vertexBuffers,
						 std::optional<IndexBufferView>				 indexBuffer,
						 uint32_t									 firstVertex,
						 uint32_t									 firstInstance,
						 const GladGLContext						&context);

		void	 Bind(const GladGLContext &context) final;
		uint32_t GetShaderHandle() const final;

		void CreateVAO(const GladGLContext &context);
		void DestroyVAO(const GladGLContext &context);

	  private:
		void SetupDepthStencil(const GladGLContext &context);
		void SetupRasterizer(const GladGLContext &context);
		void SetupBlending(const GladGLContext &context);
		void SetShader(const GladGLContext &context);
		void CreateShader();

	  private:
		GraphicsDeviceOpenGL *m_Device		 = nullptr;
		uint32_t			  m_VAO			 = 0;
		uint32_t			  m_ShaderHandle = 0;
	};

	class ComputePipelineOpenGL : public ComputePipeline, public PipelineOpenGL
	{
	  public:
		ComputePipelineOpenGL(const ComputePipelineDescription &description, GraphicsDeviceOpenGL *device);
		virtual ~ComputePipelineOpenGL();
		void	 Bind(const GladGLContext &context) final;
		uint32_t GetShaderHandle() const final;

	  private:
		void CreateShader();

	  private:
		GraphicsDeviceOpenGL *m_Device		 = nullptr;
		uint32_t			  m_ShaderHandle = 0;
	};
}	 // namespace Nexus::Graphics

#endif