#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "BufferOpenGL.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{
	class PipelineOpenGL : public Pipeline
	{
	  public:
		PipelineOpenGL(const PipelineDescription &description);
		virtual ~PipelineOpenGL();
		virtual const PipelineDescription &GetPipelineDescription() const override;
		void							   BindBuffers(const std::map<uint32_t, Nexus::WeakRef<Nexus::Graphics::VertexBufferOpenGL>> &vertexBuffers,
													   Nexus::WeakRef<Nexus::Graphics::IndexBufferOpenGL>							  indexBuffer,
													   uint32_t																		  vertexOffset,
													   uint32_t																		  instanceOffset);
		void	 Bind();
		void	 Unbind();
		uint32_t GetShaderHandle() const;

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
}	 // namespace Nexus::Graphics

#endif