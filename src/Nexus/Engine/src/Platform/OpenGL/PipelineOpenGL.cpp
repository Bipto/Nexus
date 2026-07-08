#if defined(NX_PLATFORM_OPENGL)

#include <ranges>

#include "PipelineOpenGL.hpp"

#include "DeviceBufferOpenGL.hpp"
#include "GL.hpp"
#include "ShaderModuleOpenGL.hpp"

#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
    GraphicsPipelineOpenGL::GraphicsPipelineOpenGL(
        const GraphicsPipelineDescription &description, GraphicsDeviceOpenGL *device
    )
        : IGraphicsPipeline(description), m_Device(device)
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

    void GraphicsPipelineOpenGL::BindBuffers(
        const std::map<uint32_t, VertexBufferView> &vertexBuffers, std::optional<IndexBufferView> indexBuffer,
        uint32_t firstVertex, uint32_t firstInstance, GL::IOffscreenContext *context
    )
    {
        uint32_t index = 0;
        for (const auto &[slot, vertexBufferView] : vertexBuffers)
        {
            if (slot >= m_Description.Layouts.size())
            {
                std::string message =
                    "Attempted to bind a vertex buffer to an invalid slot: (" + std::to_string(slot) + ")";
                NX_ERROR(message);
            }

            // this allows us to specify an offset into a vertex buffer without
            // requiring OpenGL 4.5 functionality i.e. is cross platform
            const auto &layout = m_Description.Layouts.at(slot);

            const DeviceBufferOpenGL *vertexBufferOpenGL =
                vertexBufferView.BufferHandle.AsDerived<const DeviceBufferOpenGL>();

            uint32_t offset = 0;
            size_t stride = layout.GetStride();

            // type is an instance buffer, offset to the first instance requested
            if (layout.IsInstanceBuffer())
            {
                offset = firstInstance * stride;
            }
            // otherwise, the buffer is a vertex buffer, offset to the first vertex
            // requested
            else
            {
                offset = firstVertex * stride;
            }

            // offset by the amount specified in the vertex buffer view
            offset += vertexBufferView.Offset;

            for (auto &element : layout)
            {
                GLenum baseType;
                uint32_t componentCount;
                GLboolean normalized;
                GL::GLPrimitiveType primitiveType = GL::GLPrimitiveType::Unknown;
                GL::GetBaseType(element, baseType, componentCount, normalized, primitiveType);

                context->EnableVertexAttribArray(m_VAO, index);

                // glCall(context.EnableVertexAttribArray(index));
                // context.BindBuffer(GL_ARRAY_BUFFER,
                // vertexBufferOpenGL->GetHandle());

                if (primitiveType == GL::GLPrimitiveType::Float)
                {
                    // glCall(context.VertexAttribPointer(index, componentCount,
                    // baseType, normalized, stride, (void *)(element.Offset +
                    // offset)));

                    context->SetVertexAttribPointer(
                        m_VAO, vertexBufferOpenGL->GetHandle(), index, componentCount, baseType, normalized, stride,
                        static_cast<uint32_t>(element.Offset + offset)
                    );
                }
                else if (primitiveType == GL::GLPrimitiveType::Int)
                {
                    // glCall(context.VertexAttribIPointer(index, componentCount,
                    // baseType, stride, (void *)(element.Offset + offset)));

                    context->SetVertexAttribIPointer(
                        m_VAO, vertexBufferOpenGL->GetHandle(), index, componentCount, baseType, stride,
                        static_cast<uint32_t>(element.Offset + offset)
                    );
                }
                else
                {
                    throw std::runtime_error("Failed to find valid primitive type");
                }

                if (layout.IsInstanceBuffer())
                {
                    context->SetVertexAttribDivisor(m_VAO, index, layout.GetInstanceStepRate());
                }

                index++;
            }

            if (indexBuffer)
            {
                IndexBufferView &view = indexBuffer.value();
                const DeviceBufferOpenGL *indexBufferOpenGL = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>();
                context->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOpenGL->GetHandle());
            }
        }
    }

    void GraphicsPipelineOpenGL::Bind(GL::IOffscreenContext *context)
    {
        context->BindVertexArray(m_VAO);

        SetupDepthStencil(context, m_Description.DepthStencilDesc.StencilReference);
        SetupRasterizer(context);
        SetupBlending(context);
        SetShader(context);
    }

    uint32_t GraphicsPipelineOpenGL::GetShaderHandle() const
    {
        return m_ShaderHandle;
    }

    void GraphicsPipelineOpenGL::CreateVAO(GL::IOffscreenContext *context)
    {
        m_VAO = context->CreateVertexArray();
    }

    void GraphicsPipelineOpenGL::DestroyVAO(GL::IOffscreenContext *context)
    {
        context->DestroyVertexArray(m_VAO);
    }

    void GraphicsPipelineOpenGL::SetStencilReference(GL::IOffscreenContext *context, uint32_t stencilReference)
    {
        // front face
        {
            GLenum sfail = GL::GetStencilOperation(m_Description.DepthStencilDesc.Front.StencilFailOperation);
            GLenum dpfail =
                GL::GetStencilOperation(m_Description.DepthStencilDesc.Front.StencilSuccessDepthFailOperation);
            GLenum dppass =
                GL::GetStencilOperation(m_Description.DepthStencilDesc.Front.StencilSuccessDepthSuccessOperation);
            context->SetStencilOp(GL_FRONT, sfail, dpfail, dppass);

            GLenum stencilCompareFunc =
                GL::GetComparisonFunction(m_Description.DepthStencilDesc.Front.StencilComparisonFunction);
            context->SetStencilFunc(
                GL_FRONT, stencilCompareFunc, stencilReference, m_Description.DepthStencilDesc.StencilCompareMask
            );
        }

        // back face
        {
            GLenum sfail = GL::GetStencilOperation(m_Description.DepthStencilDesc.Back.StencilFailOperation);
            GLenum dpfail =
                GL::GetStencilOperation(m_Description.DepthStencilDesc.Back.StencilSuccessDepthFailOperation);
            GLenum dppass =
                GL::GetStencilOperation(m_Description.DepthStencilDesc.Back.StencilSuccessDepthSuccessOperation);

            context->SetStencilOp(GL_BACK, sfail, dpfail, dppass);

            GLenum stencilCompareFunc =
                GL::GetComparisonFunction(m_Description.DepthStencilDesc.Back.StencilComparisonFunction);
            context->SetStencilFunc(
                GL_BACK, stencilCompareFunc, stencilReference, m_Description.DepthStencilDesc.StencilCompareMask
            );
        }
    }

    void GraphicsPipelineOpenGL::SetupDepthStencil(GL::IOffscreenContext *context, uint32_t stencilReference)
    {
        // enable/disable depth testing
        context->EnableCapability(GL_DEPTH_TEST, m_Description.DepthStencilDesc.EnableDepthTest);

        // enable/disable depth writing
        context->SetDepthMask(m_Description.DepthStencilDesc.EnableDepthWrite);

        // set up stencil options
        context->EnableCapability(GL_STENCIL_TEST, m_Description.DepthStencilDesc.EnableStencilTest);
        context->SetStencilMask(m_Description.DepthStencilDesc.StencilWriteMask);

        SetStencilReference(context, m_Description.DepthStencilDesc.StencilReference);

        // depth comparison
        GLenum depthFunction = GL::GetComparisonFunction(m_Description.DepthStencilDesc.DepthComparisonFunction);
        // glCall(context.DepthFunc(depthFunction));
        context->SetDepthFunction(depthFunction);

        // depths bounds testing
        // if (context.DepthBoundsEXT)
        if (context->IsDepthBoundsSupported())
        {
            context->EnableCapability(GL_DEPTH_BOUNDS_TEST_EXT, m_Description.DepthStencilDesc.EnableDepthsBoundsTest);
            context->SetDepthBounds(m_Description.DepthStencilDesc.MinDepth, m_Description.DepthStencilDesc.MaxDepth);

            // context.DepthBoundsEXT(m_Description.DepthStencilDesc.MinDepth,
            // m_Description.DepthStencilDesc.MaxDepth);
        }
    }

    void GraphicsPipelineOpenGL::SetupRasterizer(GL::IOffscreenContext *context)
    {
        context->EnableCapability(
            GL_CULL_FACE, m_Description.RasterizerStateDesc.TriangleCullMode != CullMode::CullNone
        );

        switch (m_Description.RasterizerStateDesc.TriangleCullMode)
        {
        case CullMode::Back:
            context->SetFaceCulling(GL_BACK);
            break;
        case CullMode::Front:
            context->SetFaceCulling(GL_FRONT);
            break;
        default:
            context->SetFaceCulling(GL_FRONT_AND_BACK);
            break;
        }

        if (context->IsDepthClampSupported())
        {
            context->EnableCapability(GL_DEPTH_CLAMP, m_Description.RasterizerStateDesc.DepthClipEnabled);
        }

        switch (m_Description.RasterizerStateDesc.TriangleFillMode)
        {
        case FillMode::Solid:
            context->SetPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case FillMode::Wireframe:
            context->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        default:
            throw std::runtime_error("Failed to find a valid triangle fill mode");
        }

        switch (m_Description.RasterizerStateDesc.TriangleFrontFace)
        {
        case FrontFace::Clockwise:
            context->SetFrontFace(GL_CW);
            break;
        case FrontFace::CounterClockwise:
            context->SetFrontFace(GL_CCW);
            break;
        default:
            throw std::runtime_error("Failed to find a valid front face");
        }

        if (context->SupportsPerTargetColourMask())
        {
            for (const auto &[index, blendState] : std::views::enumerate(m_Description.ColourBlendStates))
            {
                context->SetColourMask(
                    blendState.PixelWriteMask.Red, blendState.PixelWriteMask.Green, blendState.PixelWriteMask.Blue,
                    blendState.PixelWriteMask.Alpha
                );
            }
        }
        else
        {
            const auto &colourBlendState = m_Description.ColourBlendStates[0];
            context->SetColourMask(
                colourBlendState.PixelWriteMask.Red, colourBlendState.PixelWriteMask.Green,
                colourBlendState.PixelWriteMask.Blue, colourBlendState.PixelWriteMask.Alpha
            );
        }

        // vulkan requires scissor test to be enabled, for compatibility this is
        // forced on
        context->EnableCapability(GL_SCISSOR_TEST, true);
    }

    void GraphicsPipelineOpenGL::SetupBlending(GL::IOffscreenContext *context)
    {
        context->EnableCapability(GL_BLEND, true);

        if (context->SupportsPerTargetBlendFunction())
        {
            for (const auto &[index, blendState] : std::views::enumerate(m_Description.ColourBlendStates))
            {
                // enable blending for this attachment
                if (blendState.EnableBlending)
                {
                    auto sourceColourFunction = GL::GetBlendFactor(blendState.SourceColourBlend);
                    auto sourceAlphaFunction = GL::GetBlendFactor(blendState.SourceAlphaBlend);

                    auto destinationColourFunction = GL::GetBlendFactor(blendState.DestinationColourBlend);
                    auto destinationAlphaFunction = GL::GetBlendFactor(blendState.DestinationAlphaBlend);

                    context->SetBlendFunctionSeparatei(
                        index, sourceColourFunction, destinationColourFunction, sourceAlphaFunction,
                        destinationAlphaFunction
                    );

                    auto colourBlendFunction = GL::GetBlendFunction(blendState.ColorBlendFunction);
                    auto alphaBlendFunction = GL::GetBlendFunction(blendState.AlphaBlendFunction);

                    context->SetBlendEquationSeparatei(index, colourBlendFunction, alphaBlendFunction);
                }
                // disable blending for this attachment
                else
                {
                    context->SetBlendFunctioni(index, GL_ONE, GL_ZERO);
                    context->SetBlendEquationi(index, GL_FUNC_ADD);
                }
            }
        }
        else
        {
            const auto &blendState = m_Description.ColourBlendStates[0];

            auto sourceColourFunction = GL::GetBlendFactor(blendState.SourceColourBlend);
            auto sourceAlphaFunction = GL::GetBlendFactor(blendState.SourceAlphaBlend);

            auto destinationColourFunction = GL::GetBlendFactor(blendState.DestinationColourBlend);
            auto destinationAlphaFunction = GL::GetBlendFactor(blendState.DestinationAlphaBlend);

            context->SetBlendFunctionSeparate(
                sourceColourFunction, destinationColourFunction, sourceAlphaFunction, destinationAlphaFunction
            );

            auto colourBlendFunction = GL::GetBlendFunction(blendState.ColorBlendFunction);
            auto alphaBlendFunction = GL::GetBlendFunction(blendState.AlphaBlendFunction);

            context->SetBlendEquationSeparate(colourBlendFunction, alphaBlendFunction);
        }
    }

    void GraphicsPipelineOpenGL::SetShader(GL::IOffscreenContext *context)
    {
        context->UseShader(m_ShaderHandle);
    }

    void GraphicsPipelineOpenGL::CreateShader()
    {
        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        m_ShaderHandle = context->CreateProgram();

        std::vector<const ShaderModuleOpenGL *> modules;

        if (m_Description.FragmentModule.IsValid())
        {
            auto glFragmentModule = m_Description.FragmentModule.AsDerived<ShaderModuleOpenGL>();
            NX_VALIDATE(
                glFragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader"
            );
            modules.push_back(glFragmentModule);
        }

        if (m_Description.GeometryModule.IsValid())
        {
            auto glGeometryModule = m_Description.GeometryModule.AsDerived<ShaderModuleOpenGL>();
            NX_VALIDATE(
                glGeometryModule->GetShaderStage() == ShaderStage::Geometry, "Shader module is not a geometry shader"
            );
            modules.push_back(glGeometryModule);
        }

        if (m_Description.TesselationControlModule.IsValid())
        {
            auto glTesselationControlModule = m_Description.TesselationControlModule.AsDerived<ShaderModuleOpenGL>();
            NX_VALIDATE(
                glTesselationControlModule->GetShaderStage() == ShaderStage::TessellationControl,
                "Shader module is not a tesselation control shader"
            );
            modules.push_back(glTesselationControlModule);
        }

        if (m_Description.TesselationEvaluationModule.IsValid())
        {
            auto glEvaluationModule = m_Description.TesselationEvaluationModule.AsDerived<ShaderModuleOpenGL>();
            NX_VALIDATE(
                glEvaluationModule->GetShaderStage() == ShaderStage::TessellationEvaluation,
                "Shader module is not a tesselation evaluation shader"
            );
            modules.push_back(glEvaluationModule);
        }

        if (m_Description.VertexModule.IsValid())
        {
            auto glVertexModule = m_Description.VertexModule.AsDerived<ShaderModuleOpenGL>();
            NX_VALIDATE(
                glVertexModule->GetShaderStage() == ShaderStage::Vertex, "Shader module is not a vertex shader"
            );
            modules.push_back(glVertexModule);
        }

        for (const auto &module : modules)
        {
            context->AttachShaderModule(m_ShaderHandle, module->GetHandle());
        }

        context->LinkProgram(m_ShaderHandle);

        int success = context->GetProgramiv(m_ShaderHandle, GL_LINK_STATUS);
        if (!success)
        {
            GLint logLength = context->GetProgramiv(m_ShaderHandle, GL_INFO_LOG_LENGTH);

            std::string log(logLength, '\0');
            context->GetProgramInfoLog(m_ShaderHandle, logLength, nullptr, log.data());
            NX_ERROR(log);
        }

        for (const auto &module : modules)
        {
            context->DetachShader(m_ShaderHandle, module->GetHandle());
        }
    }

    ComputePipelineOpenGL::ComputePipelineOpenGL(
        const ComputePipelineDescription &description, GraphicsDeviceOpenGL *device
    )
        : IComputePipeline(description), m_Device(device)
    {
        CreateShader();
    }

    ComputePipelineOpenGL::~ComputePipelineOpenGL()
    {
    }

    void ComputePipelineOpenGL::Bind(GL::IOffscreenContext *context)
    {
        context->UseShader(m_ShaderHandle);
    }

    uint32_t ComputePipelineOpenGL::GetShaderHandle() const
    {
        return m_ShaderHandle;
    }

    void ComputePipelineOpenGL::CreateShader()
    {
        NX_VALIDATE(
            m_Description.ComputeShader->GetShaderStage() == ShaderStage::Compute,
            "Compute Pipeline shader must be ShaderStage::Compute"
        );

        const Nexus::Graphics::ShaderModuleOpenGL *computeShader =
            m_Description.ComputeShader.AsDerived<const Nexus::Graphics::ShaderModuleOpenGL>();

        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();

        m_ShaderHandle = context->CreateProgram();
        context->AttachShaderModule(m_ShaderHandle, computeShader->GetHandle());
        context->LinkProgram(m_ShaderHandle);

        int success = context->GetProgramiv(m_ShaderHandle, GL_LINK_STATUS);

        if (!success)
        {
            GLint logLength = context->GetProgramiv(m_ShaderHandle, GL_INFO_LOG_LENGTH);

            std::string log(logLength, '\0');
            context->GetProgramInfoLog(m_ShaderHandle, logLength, nullptr, log.data());
            NX_ERROR(log);
        }

        context->DetachShader(m_ShaderHandle, computeShader->GetHandle());
    }
} // namespace Nexus::Graphics

#endif