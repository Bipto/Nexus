#if defined(NX_PLATFORM_OPENGL)

	#include "PipelineOpenGL.hpp"

	#include "DeviceBufferOpenGL.hpp"
	#include "GL.hpp"
	#include "ShaderModuleOpenGL.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	GraphicsPipelineOpenGL::GraphicsPipelineOpenGL(const GraphicsPipelineDescription &description, GraphicsDeviceOpenGL *device)
		: GraphicsPipeline(description),
		  m_Device(device)
	{
		CreateShader();
	}

	GraphicsPipelineOpenGL::~GraphicsPipelineOpenGL()
	{
	}

	const GraphicsPipelineDescription &GraphicsPipelineOpenGL::GetPipelineDescription() const
	{
		return m_Description;
	}

	void GraphicsPipelineOpenGL::BindBuffers(const std::map<uint32_t, VertexBufferView> &vertexBuffers,
											 std::optional<IndexBufferView>				 indexBuffer,
											 uint32_t									 firstVertex,
											 uint32_t									 firstInstance,
											 const GladGLContext						&context)
	{
		glCall(context.BindVertexArray(m_VAO));

		uint32_t index = 0;
		for (const auto &[slot, vertexBufferView] : vertexBuffers)
		{
			if (slot >= m_Description.Layouts.size())
			{
				std::string message = "Attempted to bind a vertex buffer to an invalid slot: (" + std::to_string(slot) + ")";
				NX_ERROR(message);
			}

			// this allows us to specify an offset into a vertex buffer without
			// requiring OpenGL 4.5 functionality i.e. is cross platform
			const auto &layout = m_Description.Layouts.at(slot);

			Ref<DeviceBufferOpenGL> vertexBufferOpenGL = std::dynamic_pointer_cast<DeviceBufferOpenGL>(vertexBufferView.BufferHandle);

			uint32_t offset = 0;
			size_t	 stride = layout.GetStride();

			// type is an instance buffer, offset to the first instance requested
			if (layout.IsInstanceBuffer())
			{
				offset = firstInstance * stride;
			}
			// otherwise, the buffer is a vertex buffer, offset to the first vertex requested
			else
			{
				offset = firstVertex * stride;
			}

			// offset by the amount specified in the vertex buffer view
			offset += vertexBufferView.Offset;

			for (auto &element : layout)
			{
				GLenum				baseType;
				uint32_t			componentCount;
				GLboolean			normalized;
				GL::GLPrimitiveType primitiveType = GL::GLPrimitiveType::Unknown;
				GL::GetBaseType(element, baseType, componentCount, normalized, primitiveType);

				glCall(context.EnableVertexAttribArray(index));
				context.BindBuffer(GL_ARRAY_BUFFER, vertexBufferOpenGL->GetHandle());

				if (primitiveType == GL::GLPrimitiveType::Float)
				{
					glCall(context.VertexAttribPointer(index, componentCount, baseType, normalized, stride, (void *)(element.Offset + offset)));
				}
				else if (primitiveType == GL::GLPrimitiveType::Int)
				{
					glCall(context.VertexAttribIPointer(index, componentCount, baseType, stride, (void *)(element.Offset + offset)));
				}
				else
				{
					throw std::runtime_error("Failed to find valid primitive type");
				}

				if (layout.IsInstanceBuffer())
				{
					glCall(context.VertexAttribDivisor(index, layout.GetInstanceStepRate()));
				}

				index++;
			}

			if (indexBuffer)
			{
				IndexBufferView		   &view			   = indexBuffer.value();
				Ref<DeviceBufferOpenGL> deviceBufferOpenGL = std::dynamic_pointer_cast<DeviceBufferOpenGL>(view.BufferHandle);
				context.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, deviceBufferOpenGL->GetHandle());
			}
		}
	}

	void GraphicsPipelineOpenGL::Bind(const GladGLContext &context)
	{
		glCall(context.BindVertexArray(m_VAO));

		SetupDepthStencil(context);
		SetupRasterizer(context);
		SetupBlending(context);
		SetShader(context);
	}

	uint32_t GraphicsPipelineOpenGL::GetShaderHandle() const
	{
		return m_ShaderHandle;
	}

	void GraphicsPipelineOpenGL::CreateVAO(const GladGLContext &context)
	{
		glCall(context.GenVertexArrays(1, &m_VAO));
		glCall(context.BindVertexArray(m_VAO));
	}

	void GraphicsPipelineOpenGL::DestroyVAO(const GladGLContext &context)
	{
		glCall(context.BindVertexArray(0));
		glCall(context.DeleteVertexArrays(1, &m_VAO));
	}

	void GraphicsPipelineOpenGL::SetupDepthStencil(const GladGLContext &context)
	{
		// enable/disable depth testing
		if (m_Description.DepthStencilDesc.EnableDepthTest)
		{
			glCall(context.Enable(GL_DEPTH_TEST));
		}
		else
		{
			glCall(context.Disable(GL_DEPTH_TEST));
		}

		// enable/disable depth writing
		if (m_Description.DepthStencilDesc.EnableDepthWrite)
		{
			glCall(context.DepthMask(GL_TRUE));
		}
		else
		{
			glCall(context.DepthMask(GL_FALSE));
		}

		// set up stencil options
		if (m_Description.DepthStencilDesc.EnableStencilTest)
		{
			glCall(context.Enable(GL_STENCIL_TEST));
		}
		else
		{
			glCall(context.Disable(GL_STENCIL_TEST));
		}

		glCall(context.StencilMask(m_Description.DepthStencilDesc.StencilWriteMask));

		// front face
		{
			GLenum sfail  = GL::GetStencilOperation(m_Description.DepthStencilDesc.Front.StencilFailOperation);
			GLenum dpfail = GL::GetStencilOperation(m_Description.DepthStencilDesc.Front.StencilSuccessDepthFailOperation);
			GLenum dppass = GL::GetStencilOperation(m_Description.DepthStencilDesc.Front.StencilSuccessDepthSuccessOperation);

			glCall(context.StencilOpSeparate(GL_FRONT, sfail, dpfail, dppass));

			GLenum stencilCompareFunc = GL::GetComparisonFunction(m_Description.DepthStencilDesc.Front.StencilComparisonFunction);
			glCall(context.StencilFuncSeparate(GL_FRONT,
											   stencilCompareFunc,
											   m_Description.DepthStencilDesc.StencilReference,
											   m_Description.DepthStencilDesc.StencilCompareMask));
		}

		// back face
		{
			GLenum sfail  = GL::GetStencilOperation(m_Description.DepthStencilDesc.Back.StencilFailOperation);
			GLenum dpfail = GL::GetStencilOperation(m_Description.DepthStencilDesc.Back.StencilSuccessDepthFailOperation);
			GLenum dppass = GL::GetStencilOperation(m_Description.DepthStencilDesc.Back.StencilSuccessDepthSuccessOperation);

			glCall(context.StencilOpSeparate(GL_BACK, sfail, dpfail, dppass));

			GLenum stencilCompareFunc = GL::GetComparisonFunction(m_Description.DepthStencilDesc.Back.StencilComparisonFunction);
			glCall(context.StencilFuncSeparate(GL_BACK,
											   stencilCompareFunc,
											   m_Description.DepthStencilDesc.StencilReference,
											   m_Description.DepthStencilDesc.StencilCompareMask));
		}

		GLenum depthFunction = GL::GetComparisonFunction(m_Description.DepthStencilDesc.DepthComparisonFunction);
		glCall(context.DepthFunc(depthFunction));
	}

	void GraphicsPipelineOpenGL::SetupRasterizer(const GladGLContext &context)
	{
		if (m_Description.RasterizerStateDesc.TriangleCullMode == CullMode::CullNone)
		{
			glCall(context.Disable(GL_CULL_FACE));
		}
		else
		{
			glCall(context.Enable(GL_CULL_FACE));
		}

		switch (m_Description.RasterizerStateDesc.TriangleCullMode)
		{
			case CullMode::Back: glCall(context.CullFace(GL_BACK)); break;
			case CullMode::Front: glCall(context.CullFace(GL_FRONT)); break;
			default: glCall(context.CullFace(GL_FRONT_AND_BACK)); break;
		}

	#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)
		if (m_Description.RasterizerStateDesc.DepthClipEnabled)
		{
			glCall(context.Enable(GL_DEPTH_CLAMP));
		}
		else
		{
			glCall(context.Disable(GL_DEPTH_CLAMP));
		}
	#endif

	#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)
		switch (m_Description.RasterizerStateDesc.TriangleFillMode)
		{
			case FillMode::Solid: glCall(context.PolygonMode(GL_FRONT_AND_BACK, GL_FILL)); break;
			case FillMode::Wireframe: glCall(context.PolygonMode(GL_FRONT_AND_BACK, GL_LINE)); break;
		}
	#endif

		switch (m_Description.RasterizerStateDesc.TriangleFrontFace)
		{
			case FrontFace::Clockwise: glCall(context.FrontFace(GL_CW)); break;
			case FrontFace::CounterClockwise: glCall(context.FrontFace(GL_CCW)); break;
		}

	#if !defined(__EMSCRIPTEN__)
		for (size_t i = 0; i < m_Description.ColourBlendStates.size(); i++)
		{
			const auto &colourBlendState = m_Description.ColourBlendStates[i];
			context.ColorMaski(i,
							   colourBlendState.PixelWriteMask.Red,
							   colourBlendState.PixelWriteMask.Green,
							   colourBlendState.PixelWriteMask.Blue,
							   colourBlendState.PixelWriteMask.Alpha);
		}
	#else
		const auto &colourBlendState = m_Description.ColourBlendStates[0];
		context.ColorMask(colourBlendState.PixelWriteMask.Red,
						  colourBlendState.PixelWriteMask.Green,
						  colourBlendState.PixelWriteMask.Blue,
						  colourBlendState.PixelWriteMask.Alpha);
	#endif

		// vulkan requires scissor test to be enabled
		glCall(context.Enable(GL_SCISSOR_TEST));
	}

	void GraphicsPipelineOpenGL::SetupBlending(const GladGLContext &context)
	{
		glCall(context.Enable(GL_BLEND));

	#if defined(__EMSCRIPTEN__)
		if (m_Description.ColourBlendStates[0].EnableBlending)
		{
			auto sourceColourFunction = GL::GetBlendFactor(m_Description.ColourBlendStates[0].SourceColourBlend);
			auto sourceAlphaFunction  = GL::GetBlendFactor(m_Description.ColourBlendStates[0].SourceAlphaBlend);

			auto destinationColourFunction = GL::GetBlendFactor(m_Description.ColourBlendStates[0].DestinationColourBlend);
			auto destinationAlphaFunction  = GL::GetBlendFactor(m_Description.ColourBlendStates[0].DestinationAlphaBlend);
			glCall(context.BlendFuncSeparate(sourceColourFunction, destinationColourFunction, sourceAlphaFunction, destinationAlphaFunction));

			auto colorBlendFunction = GL::GetBlendFunction(m_Description.ColourBlendStates[0].ColorBlendFunction);
			auto alphaBlendFunction = GL::GetBlendFunction(m_Description.ColourBlendStates[0].AlphaBlendFunction);
			glCall(context.BlendEquationSeparate(colorBlendFunction, alphaBlendFunction));
		}
		else
		{
			context.BlendFunc(GL_ONE, GL_ZERO);
			context.BlendEquation(GL_FUNC_ADD);
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
				glCall(context.BlendFuncSeparatei(i, sourceColourFunction, destinationColourFunction, sourceAlphaFunction, destinationAlphaFunction));

				auto colorBlendFunction = GL::GetBlendFunction(m_Description.ColourBlendStates[i].ColorBlendFunction);
				auto alphaBlendFunction = GL::GetBlendFunction(m_Description.ColourBlendStates[i].AlphaBlendFunction);
				glCall(context.BlendEquationSeparatei(i, colorBlendFunction, alphaBlendFunction));
			}
			// disable blending for this attachment
			else
			{
				context.BlendFunci(i, GL_ONE, GL_ZERO);
				context.BlendEquationi(i, GL_FUNC_ADD);
			}
		}
	#endif
	}

	void GraphicsPipelineOpenGL::SetShader(const GladGLContext &context)
	{
		glCall(context.UseProgram(m_ShaderHandle));
	}

	void GraphicsPipelineOpenGL::CreateShader()
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				m_ShaderHandle = context.CreateProgram();

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

				for (const auto &module : modules) { glCall(context.AttachShader(m_ShaderHandle, module->GetHandle())); }

				glCall(context.LinkProgram(m_ShaderHandle));

				int success;
				glCall(context.GetProgramiv(m_ShaderHandle, GL_LINK_STATUS, &success));
				if (!success)
				{
					char infoLog[512];
					glCall(context.GetProgramInfoLog(m_ShaderHandle, 512, nullptr, infoLog));
					std::string errorMessage = "Error: Shader Program - " + std::string(infoLog);
					NX_ERROR(errorMessage);
				}

				for (const auto &module : modules) { glCall(context.DetachShader(m_ShaderHandle, module->GetHandle())); }
			});
	}

	ComputePipelineOpenGL::ComputePipelineOpenGL(const ComputePipelineDescription &description, GraphicsDeviceOpenGL *device)
		: ComputePipeline(description),
		  m_Device(device)
	{
		CreateShader();
	}

	ComputePipelineOpenGL::~ComputePipelineOpenGL()
	{
	}

	void ComputePipelineOpenGL::Bind(const GladGLContext &context)
	{
		context.UseProgram(m_ShaderHandle);
	}

	uint32_t ComputePipelineOpenGL::GetShaderHandle() const
	{
		return m_ShaderHandle;
	}

	void ComputePipelineOpenGL::CreateShader()
	{
		NX_ASSERT(m_Description.ComputeShader->GetShaderStage() == ShaderStage::Compute, "Compute Pipeline shader must be ShaderStage::Compute");

		Nexus::Ref<Nexus::Graphics::ShaderModuleOpenGL> computeShader =
			std::dynamic_pointer_cast<Nexus::Graphics::ShaderModuleOpenGL>(m_Description.ComputeShader);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				m_ShaderHandle = context.CreateProgram();
				glCall(context.AttachShader(m_ShaderHandle, computeShader->GetHandle()));
				glCall(context.LinkProgram(m_ShaderHandle));

				int success;
				glCall(context.GetProgramiv(m_ShaderHandle, GL_LINK_STATUS, &success));
				if (!success)
				{
					char infoLog[512];
					glCall(context.GetProgramInfoLog(m_ShaderHandle, 512, nullptr, infoLog));
					std::string errorMessage = "Error: Shader Program - " + std::string(infoLog);
					NX_ERROR(errorMessage);
				}

				glCall(context.DetachShader(m_ShaderHandle, computeShader->GetHandle()));
			});
	}
}	 // namespace Nexus::Graphics

#endif