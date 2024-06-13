#if defined(NX_PLATFORM_OPENGL)

#include "PipelineOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "ShaderModuleOpenGL.hpp"

#include "GL.hpp"

namespace Nexus::Graphics
{
    PipelineOpenGL::PipelineOpenGL(const PipelineDescription &description)
        : Pipeline(description)
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

    void PipelineOpenGL::Bind()
    {
        BindVertexArray();
        SetupDepthStencil();
        SetupRasterizer();
        SetupBlending();
        SetShader();
    }

    void PipelineOpenGL::Unbind()
    {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &m_VAO);
    }

    uint32_t PipelineOpenGL::GetShaderHandle() const
    {
        return m_ShaderHandle;
    }

    void PipelineOpenGL::SetupDepthStencil()
    {
        // enable/disable depth testing
        if (m_Description.DepthStencilDesc.EnableDepthTest)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        // enable/disable depth writing
        if (m_Description.DepthStencilDesc.EnableDepthWrite)
        {
            glDepthMask(GL_TRUE);
        }
        else
        {
            glDepthMask(GL_FALSE);
        }

        // set up stencil options
        if (m_Description.DepthStencilDesc.EnableStencilTest)
        {
            glEnable(GL_STENCIL_TEST);
        }
        else
        {
            glDisable(GL_STENCIL_TEST);
        }

        GLenum sfail = GL::GetStencilOperation(m_Description.DepthStencilDesc.StencilFailOperation);
        GLenum dpfail = GL::GetStencilOperation(m_Description.DepthStencilDesc.StencilSuccessDepthFailOperation);
        GLenum dppass = GL::GetStencilOperation(m_Description.DepthStencilDesc.StencilSuccessDepthSuccessOperation);

        glStencilOp(sfail, dpfail, dppass);
        GLenum stencilFunction = GL::GetComparisonFunction(m_Description.DepthStencilDesc.StencilComparisonFunction);
        glStencilFunc(stencilFunction, 1, m_Description.DepthStencilDesc.StencilMask);
        glStencilMask(m_Description.DepthStencilDesc.StencilMask);

        GLenum depthFunction = GL::GetComparisonFunction(m_Description.DepthStencilDesc.DepthComparisonFunction);
        glDepthFunc(depthFunction);

        glDepthRangef(m_Description.DepthStencilDesc.MinDepth, m_Description.DepthStencilDesc.MaxDepth);
    }

    void PipelineOpenGL::SetupRasterizer()
    {
        if (m_Description.RasterizerStateDesc.TriangleCullMode == CullMode::None)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
        }

        switch (m_Description.RasterizerStateDesc.TriangleCullMode)
        {
        case CullMode::Back:
            glCullFace(GL_BACK);
            break;
        case CullMode::Front:
            glCullFace(GL_FRONT);
            break;
        }

#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)
        if (m_Description.RasterizerStateDesc.DepthClipEnabled)
        {
            glEnable(GL_DEPTH_CLAMP);
        }
        else
        {
            glDisable(GL_DEPTH_CLAMP);
        }
#endif

#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)
        switch (m_Description.RasterizerStateDesc.TriangleFillMode)
        {
        case FillMode::Solid:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case FillMode::Wireframe:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        }
#endif

        switch (m_Description.RasterizerStateDesc.TriangleFrontFace)
        {
        case FrontFace::Clockwise:
            glFrontFace(GL_CW);
            break;
        case FrontFace::CounterClockwise:
            glFrontFace(GL_CCW);
            break;
        }

        // vulkan requires scissor test to be enabled
        glEnable(GL_SCISSOR_TEST);
    }

    void PipelineOpenGL::SetupBlending()
    {
        if (m_Description.BlendStateDescription.EnableBlending)
        {
            glEnable(GL_BLEND);

            auto sourceColourFunction = GL::GetBlendFactor(m_Description.BlendStateDescription.SourceColourBlend);
            auto sourceAlphaFunction = GL::GetBlendFactor(m_Description.BlendStateDescription.SourceAlphaBlend);

            auto destinationColourFunction = GL::GetBlendFactor(m_Description.BlendStateDescription.DestinationColourBlend);
            auto destinationAlphaFunction = GL::GetBlendFactor(m_Description.BlendStateDescription.DestinationAlphaBlend);
            // glBlendFuncSeparate(destinationColourFunction, destinationColourFunction, destinationColourFunction, destinationAlphaFunction);
            glBlendFuncSeparate(sourceColourFunction, destinationColourFunction, sourceAlphaFunction, destinationAlphaFunction);

            auto colorBlendFunction = GL::GetBlendFunction(m_Description.BlendStateDescription.ColorBlendFunction);
            auto alphaBlendFunction = GL::GetBlendFunction(m_Description.BlendStateDescription.AlphaBlendFunction);
            // glBlendEquation(blendEquation);

            glBlendEquationSeparate(colorBlendFunction, alphaBlendFunction);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    void PipelineOpenGL::SetShader()
    {
        glUseProgram(m_ShaderHandle);
    }

    void PipelineOpenGL::BindVertexArray()
    {
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);
    }

    void PipelineOpenGL::SetupVertexElements(uint32_t bufferIndex, uint32_t vertexOffset, uint32_t instanceOffset)
    {
        // this allows us to specify an offset into a vertex buffer without requiring OpenGL 4.5 functionality i.e. is cross platform
        const auto &layout = m_Description.Layouts.at(bufferIndex);

        uint32_t offset = vertexOffset;
        if (layout.IsInstanceBuffer())
        {
            offset = instanceOffset;
        }

        offset *= layout.GetStride();

        glBindVertexArray(m_VAO);
        int index = 0;
        for (auto &element : layout)
        {
            GLenum baseType;
            uint32_t componentCount;
            GLboolean normalized;
            GL::GetBaseType(element, baseType, componentCount, normalized);

            glVertexAttribPointer(
                index,
                componentCount,
                baseType,
                normalized,
                layout.GetStride(),
                (void *)(element.Offset + offset));

            glVertexAttribDivisor(index, layout.GetInstanceStepRate());

            glEnableVertexAttribArray(index);
            index++;
        }
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
            // glAttachShader(m_ShaderHandle, glFragmentModule->GetHandle());
        }

        if (m_Description.GeometryModule)
        {
            auto glGeometryModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.GeometryModule);
            NX_ASSERT(glGeometryModule->GetShaderStage() == ShaderStage::Geometry, "Shader module is not a geometry shader");
            modules.push_back(glGeometryModule);
            // glAttachShader(m_ShaderHandle, glGeometryModule->GetHandle());
        }

        if (m_Description.TesselationControlModule)
        {
            auto glTesselationControlModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.TesselationControlModule);
            NX_ASSERT(glTesselationControlModule->GetShaderStage() == ShaderStage::TesselationControl, "Shader module is not a tesselation control shader");
            modules.push_back(glTesselationControlModule);
            // glAttachShader(m_ShaderHandle, glTesselationControlModule->GetHandle());
        }

        if (m_Description.TesselationEvaluationModule)
        {
            auto glEvaluationModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.TesselationEvaluationModule);
            NX_ASSERT(glEvaluationModule->GetShaderStage() == ShaderStage::TesselationEvaluation, "Shader module is not a tesselation evaluation shader");
            modules.push_back(glEvaluationModule);
            // glAttachShader(m_ShaderHandle, glEvaluationModule->GetHandle());
        }

        if (m_Description.VertexModule)
        {
            auto glVertexModule = std::dynamic_pointer_cast<ShaderModuleOpenGL>(m_Description.VertexModule);
            NX_ASSERT(glVertexModule->GetShaderStage() == ShaderStage::Vertex, "Shader module is not a vertex shader");
            modules.push_back(glVertexModule);
            // glAttachShader(m_ShaderHandle, glVertexModule->GetHandle());
        }

        for (const auto &module : modules)
        {
            glAttachShader(m_ShaderHandle, module->GetHandle());
        }

        glLinkProgram(m_ShaderHandle);

        char infoLog[512];
        int success;
        glGetProgramiv(m_ShaderHandle, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_ShaderHandle, 512, nullptr, infoLog);
            std::string errorMessage = "Error: Shader Program - " + std::string(infoLog);
            NX_ERROR(errorMessage);
        }

        for (const auto &module : modules)
        {
            glDetachShader(m_ShaderHandle, module->GetHandle());
        }
    }

    void PipelineOpenGL::BindVertexBuffers(const std::map<uint32_t, Nexus::Ref<Nexus::Graphics::VertexBufferOpenGL>> &vertexBuffers, uint32_t vertexOffset, uint32_t instanceOffset)
    {
        glBindVertexArray(m_VAO);

        uint32_t index = 0;
        for (const auto &vertexBufferBinding : vertexBuffers)
        {
            if (vertexBufferBinding.first < m_Description.Layouts.size())
            {
                // this allows us to specify an offset into a vertex buffer without requiring OpenGL 4.5 functionality i.e. is cross platform
                const auto &layout = m_Description.Layouts.at(vertexBufferBinding.first);
                const auto &vertexBuffer = vertexBuffers.at(vertexBufferBinding.first);

                uint32_t offset = vertexOffset;
                if (layout.IsInstanceBuffer())
                {
                    offset = instanceOffset;
                }

                offset *= layout.GetStride();

                for (auto &element : layout)
                {
                    GLenum baseType;
                    uint32_t componentCount;
                    GLboolean normalized;
                    GL::GetBaseType(element, baseType, componentCount, normalized);

                    glEnableVertexAttribArray(index);
                    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetHandle());
                    glVertexAttribPointer(
                        index,
                        componentCount,
                        baseType,
                        normalized,
                        layout.GetStride(),
                        (void *)(element.Offset + offset));
                    glVertexAttribDivisor(index, layout.GetInstanceStepRate());

                    index++;
                }
            }
        }
    }
}

#endif