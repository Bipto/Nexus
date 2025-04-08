#if defined(NX_PLATFORM_OPENGL)

	#include "PipelineOpenGL.hpp"

	#include "BufferOpenGL.hpp"
	#include "GL.hpp"
	#include "ShaderModuleOpenGL.hpp"

namespace Nexus::Graphics
{
	PipelineOpenGL::PipelineOpenGL(const PipelineDescription &description) : Pipeline(description)
	{
		CreateShader();
	}

	PipelineOpenGL::~PipelineOpenGL()
	{
	}

	const PipelineDescription &PipelineOpenGL::GetPipelineDescription() const
	{
		return m_Description;
	}

	void PipelineOpenGL::BindBuffers(const std::map<uint32_t, Nexus::WeakRef<Nexus::Graphics::VertexBufferOpenGL>> &vertexBuffers,
									 Nexus::WeakRef<Nexus::Graphics::IndexBufferOpenGL>								indexBuffer,
									 uint32_t																		vertexOffset,
									 uint32_t																		instanceOffset)
	{
		glCall(glBindVertexArray(m_VAO));

		uint32_t index = 0;
		for (const auto &vertexBufferBinding : vertexBuffers)
		{
			if (vertexBufferBinding.first >= m_Description.Layouts.size())
			{
				std::string message = "Attempted to bind a vertex buffer to an invalid slot: (" + std::to_string(vertexBufferBinding.first) + ")";
				NX_ERROR(message);
			}

			// this allows us to specify an offset into a vertex buffer without
			// requiring OpenGL 4.5 functionality i.e. is cross platform
			const auto &layout = m_Description.Layouts.at(vertexBufferBinding.first);

			// check that the vertex buffer is still valid
			if (const auto vertexBuffer = vertexBuffers.at(vertexBufferBinding.first).lock())
			{
				vertexBuffer->Bind();

				uint32_t offset = vertexOffset;
				if (layout.IsInstanceBuffer())
				{
					offset = instanceOffset;
				}

				offset *= layout.GetStride();

				for (auto &element : layout)
				{
					GLenum	  baseType;
					uint32_t  componentCount;
					GLboolean normalized;
					GL::GLPrimitiveType primitiveType = GL::GLPrimitiveType::Unknown;
					GL::GetBaseType(element, baseType, componentCount, normalized, primitiveType);

					glCall(glEnableVertexAttribArray(index));
					glCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetHandle()));

					if (primitiveType == GL::GLPrimitiveType::Float)
					{
						glCall(glVertexAttribPointer(index,
													 componentCount,
													 baseType,
													 normalized,
													 layout.GetStride(),
													 (void *)(element.Offset + offset)));
					}
					else if (primitiveType == GL::GLPrimitiveType::Int)
					{
						glCall(glVertexAttribIPointer(index, componentCount, baseType, layout.GetStride(), (void *)(element.Offset + offset)));
					}
					else
					{
						throw std::runtime_error("Failed to find valid primitive type");
					}

					glCall(glVertexAttribDivisor(index, layout.GetInstanceStepRate()));

					index++;
				}
			}

			if (auto lockedIndexBuffer = indexBuffer.lock())
			{
				lockedIndexBuffer->Bind();
			}
		}
	}

	void PipelineOpenGL::Bind()
	{
		glCall(glBindVertexArray(m_VAO));

		SetupDepthStencil();
		SetupRasterizer();
		SetupBlending();
		SetShader();
	}

	void PipelineOpenGL::Unbind()
	{
	}

	uint32_t PipelineOpenGL::GetShaderHandle() const
	{
		return m_ShaderHandle;
	}

	void PipelineOpenGL::CreateVAO()
	{
		glCall(glGenVertexArrays(1, &m_VAO));
		glCall(glBindVertexArray(m_VAO));
	}

	void PipelineOpenGL::DestroyVAO()
	{
		glCall(glBindVertexArray(0));
		glCall(glDeleteVertexArrays(1, &m_VAO));
	}

	void PipelineOpenGL::SetupDepthStencil()
	{
		// enable/disable depth testing
		if (m_Description.DepthStencilDesc.EnableDepthTest)
		{
			glCall(glEnable(GL_DEPTH_TEST));
		}
		else
		{
			glCall(glDisable(GL_DEPTH_TEST));
		}

		// enable/disable depth writing
		if (m_Description.DepthStencilDesc.EnableDepthWrite)
		{
			glCall(glDepthMask(GL_TRUE));
		}
		else
		{
			glCall(glDepthMask(GL_FALSE));
		}

		// set up stencil options
		if (m_Description.DepthStencilDesc.EnableStencilTest)
		{
			glCall(glEnable(GL_STENCIL_TEST));
		}
		else
		{
			glCall(glDisable(GL_STENCIL_TEST));
		}

		GLenum sfail  = GL::GetStencilOperation(m_Description.DepthStencilDesc.StencilFailOperation);
		GLenum dpfail = GL::GetStencilOperation(m_Description.DepthStencilDesc.StencilSuccessDepthFailOperation);
		GLenum dppass = GL::GetStencilOperation(m_Description.DepthStencilDesc.StencilSuccessDepthSuccessOperation);

		glCall(glStencilOp(sfail, dpfail, dppass));
		GLenum stencilFunction = GL::GetComparisonFunction(m_Description.DepthStencilDesc.StencilComparisonFunction);
		glCall(glStencilFunc(stencilFunction, 1, m_Description.DepthStencilDesc.StencilMask));
		glCall(glStencilMask(m_Description.DepthStencilDesc.StencilMask));

		GLenum depthFunction = GL::GetComparisonFunction(m_Description.DepthStencilDesc.DepthComparisonFunction);
		glCall(glDepthFunc(depthFunction));

		glCall(glDepthRangef(m_Description.DepthStencilDesc.MinDepth, m_Description.DepthStencilDesc.MaxDepth));
	}

	void PipelineOpenGL::SetupRasterizer()
	{
		if (m_Description.RasterizerStateDesc.TriangleCullMode == CullMode::CullNone)
		{
			glCall(glDisable(GL_CULL_FACE));
		}
		else
		{
			glCall(glEnable(GL_CULL_FACE));
		}

		switch (m_Description.RasterizerStateDesc.TriangleCullMode)
		{
			case CullMode::Back: glCall(glCullFace(GL_BACK)); break;
			case CullMode::Front: glCall(glCullFace(GL_FRONT)); break;
			default: glCall(glCullFace(GL_FRONT_AND_BACK)); break;
		}

	#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)
		if (m_Description.RasterizerStateDesc.DepthClipEnabled)
		{
			glCall(glEnable(GL_DEPTH_CLAMP));
		}
		else
		{
			glCall(glDisable(GL_DEPTH_CLAMP));
		}
	#endif

	#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)
		switch (m_Description.RasterizerStateDesc.TriangleFillMode)
		{
			case FillMode::Solid: glCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)); break;
			case FillMode::Wireframe: glCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)); break;
		}
	#endif

		switch (m_Description.RasterizerStateDesc.TriangleFrontFace)
		{
			case FrontFace::Clockwise: glCall(glFrontFace(GL_CW)); break;
			case FrontFace::CounterClockwise: glCall(glFrontFace(GL_CCW)); break;
		}

	#if !defined(__EMSCRIPTEN__)
		for (size_t i = 0; i < m_Description.ColourBlendStates.size(); i++)
		{
			const auto &colourBlendState = m_Description.ColourBlendStates[i];
			glColorMaski(i,
						 colourBlendState.PixelWriteMask.Red,
						 colourBlendState.PixelWriteMask.Green,
						 colourBlendState.PixelWriteMask.Blue,
						 colourBlendState.PixelWriteMask.Alpha);
		}
	#else
		const auto &colourBlendState = m_Description.ColourBlendStates[0];
		glColorMask(colourBlendState.PixelWriteMask.Red,
					colourBlendState.PixelWriteMask.Green,
					colourBlendState.PixelWriteMask.Blue,
					colourBlendState.PixelWriteMask.Alpha);
	#endif

		// vulkan requires scissor test to be enabled
		glCall(glEnable(GL_SCISSOR_TEST));
	}

	void PipelineOpenGL::SetupBlending()
	{
		glCall(glEnable(GL_BLEND));

	#if defined(__EMSCRIPTEN__)
		if (m_Description.ColourBlendStates[0].EnableBlending)
		{
			auto sourceColourFunction = GL::GetBlendFactor(m_Description.ColourBlendStates[0].SourceColourBlend);
			auto sourceAlphaFunction  = GL::GetBlendFactor(m_Description.ColourBlendStates[0].SourceAlphaBlend);

			auto destinationColourFunction = GL::GetBlendFactor(m_Description.ColourBlendStates[0].DestinationColourBlend);
			auto destinationAlphaFunction  = GL::GetBlendFactor(m_Description.ColourBlendStates[0].DestinationAlphaBlend);
			glCall(glBlendFuncSeparate(sourceColourFunction, destinationColourFunction, sourceAlphaFunction, destinationAlphaFunction));

			auto colorBlendFunction = GL::GetBlendFunction(m_Description.ColourBlendStates[0].ColorBlendFunction);
			auto alphaBlendFunction = GL::GetBlendFunction(m_Description.ColourBlendStates[0].AlphaBlendFunction);
			glCall(glBlendEquationSeparate(colorBlendFunction, alphaBlendFunction));
		}
		else
		{
			glBlendFunc(GL_ONE, GL_ZERO);
			glBlendEquation(GL_FUNC_ADD);
		}
	#else
		for (size_t i = 0; i < m_Description.ColourBlendStates.size(); i++)
		{
			if (m_Description.ColourBlendStates[i].EnableBlending)
			{
				auto sourceColourFunction = GL::GetBlendFactor(m_Description.ColourBlendStates[i].SourceColourBlend);
				auto sourceAlphaFunction  = GL::GetBlendFactor(m_Description.ColourBlendStates[i].SourceAlphaBlend);

				auto destinationColourFunction = GL::GetBlendFactor(m_Description.ColourBlendStates[i].DestinationColourBlend);
				auto destinationAlphaFunction  = GL::GetBlendFactor(m_Description.ColourBlendStates[i].DestinationAlphaBlend);
				glCall(glBlendFuncSeparatei(i, sourceColourFunction, destinationColourFunction, sourceAlphaFunction, destinationAlphaFunction));

				auto colorBlendFunction = GL::GetBlendFunction(m_Description.ColourBlendStates[i].ColorBlendFunction);
				auto alphaBlendFunction = GL::GetBlendFunction(m_Description.ColourBlendStates[i].AlphaBlendFunction);
				glCall(glBlendEquationSeparatei(i, colorBlendFunction, alphaBlendFunction));
			}
			// disable blending for this attachment
			else
			{
				glBlendFunci(i, GL_ONE, GL_ZERO);
				glBlendEquationi(i, GL_FUNC_ADD);
			}
		}
	#endif
	}

	void PipelineOpenGL::SetShader()
	{
		glCall(glUseProgram(m_ShaderHandle));
	}

	void PipelineOpenGL::CreateShader()
	{
		m_ShaderHandle = glCreateProgram();

		std::vector<Ref<ShaderModuleOpenGL>> modules;

		if (m_Description.FragmentModule)
		{
			auto glFragmentModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.FragmentModule);
			NX_ASSERT(glFragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader");
			modules.push_back(glFragmentModule);
		}

		if (m_Description.GeometryModule)
		{
			auto glGeometryModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.GeometryModule);
			NX_ASSERT(glGeometryModule->GetShaderStage() == ShaderStage::Geometry, "Shader module is not a geometry shader");
			modules.push_back(glGeometryModule);
		}

		if (m_Description.TesselationControlModule)
		{
			auto glTesselationControlModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.TesselationControlModule);
			NX_ASSERT(glTesselationControlModule->GetShaderStage() == ShaderStage::TesselationControl,
					  "Shader module is not a tesselation control shader");
			modules.push_back(glTesselationControlModule);
		}

		if (m_Description.TesselationEvaluationModule)
		{
			auto glEvaluationModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.TesselationEvaluationModule);
			NX_ASSERT(glEvaluationModule->GetShaderStage() == ShaderStage::TesselationEvaluation,
					  "Shader module is not a tesselation evaluation shader");
			modules.push_back(glEvaluationModule);
		}

		if (m_Description.VertexModule)
		{
			auto glVertexModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.VertexModule);
			NX_ASSERT(glVertexModule->GetShaderStage() == ShaderStage::Vertex, "Shader module is not a vertex shader");
			modules.push_back(glVertexModule);
		}

		for (const auto &module : modules) { glCall(glAttachShader(m_ShaderHandle, module->GetHandle())); }

		glCall(glLinkProgram(m_ShaderHandle));

		int success;
		glCall(glGetProgramiv(m_ShaderHandle, GL_LINK_STATUS, &success));
		if (!success)
		{
			char infoLog[512];
			glCall(glGetProgramInfoLog(m_ShaderHandle, 512, nullptr, infoLog));
			std::string errorMessage = "Error: Shader Program - " + std::string(infoLog);
			NX_ERROR(errorMessage);
		}

		for (const auto &module : modules) { glCall(glDetachShader(m_ShaderHandle, module->GetHandle())); }
	}
}	 // namespace Nexus::Graphics

#endif