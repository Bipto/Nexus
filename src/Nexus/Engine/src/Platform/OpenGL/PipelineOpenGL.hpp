#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Platform/OpenGL/Context/IOffscreenContext.hpp"

	#include "DeviceBufferOpenGL.hpp"
	#include "RHI/Pipeline.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class PipelineOpenGL
	{
	  public:
		virtual ~PipelineOpenGL()
		{
		}
		virtual void	 Bind(GL::IOffscreenContext *context) = 0;
		virtual uint32_t GetShaderHandle() const			  = 0;
	};

	class GraphicsPipelineOpenGL : public IGraphicsPipeline, public PipelineOpenGL
	{
	  public:
		GraphicsPipelineOpenGL(const GraphicsPipelineDescription &description, GraphicsDeviceOpenGL *device);
		virtual ~GraphicsPipelineOpenGL();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;

		void BindBuffers(const std::map<uint32_t, VertexBufferView> &vertexBuffers,
						 std::optional<IndexBufferView>				 indexBuffer,
						 uint32_t									 firstVertex,
						 uint32_t									 firstInstance,
						 GL::IOffscreenContext						*context);

		void	 Bind(GL::IOffscreenContext *context) final;
		uint32_t GetShaderHandle() const final;

		void CreateVAO(GL::IOffscreenContext *context);
		void DestroyVAO(GL::IOffscreenContext *context);

		void SetStencilReference(GL::IOffscreenContext *context, uint32_t stencilReference);

	  private:
		void SetupDepthStencil(GL::IOffscreenContext *context, uint32_t stencilReference);
		void SetupRasterizer(GL::IOffscreenContext *context);
		void SetupBlending(GL::IOffscreenContext *context);
		void SetShader(GL::IOffscreenContext *context);
		void CreateShader();

	  private:
		GraphicsDeviceOpenGL *m_Device		 = nullptr;
		uint32_t			  m_VAO			 = 0;
		uint32_t			  m_ShaderHandle = 0;
	};

	class ComputePipelineOpenGL : public IComputePipeline, public PipelineOpenGL
	{
	  public:
		ComputePipelineOpenGL(const ComputePipelineDescription &description, GraphicsDeviceOpenGL *device);
		virtual ~ComputePipelineOpenGL();
		void	 Bind(GL::IOffscreenContext *context) final;
		uint32_t GetShaderHandle() const final;

	  private:
		void CreateShader();

	  private:
		GraphicsDeviceOpenGL *m_Device		 = nullptr;
		uint32_t			  m_ShaderHandle = 0;
	};
}	 // namespace Nexus::Graphics

#endif