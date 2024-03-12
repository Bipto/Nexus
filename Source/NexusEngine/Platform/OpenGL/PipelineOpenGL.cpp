#if defined(NX_PLATFORM_OPENGL)

#include "PipelineOpenGL.hpp"

#include "ShaderOpenGL.hpp"
#include "BufferOpenGL.hpp"

#include "GL.hpp"

namespace Nexus::Graphics
{
    PipelineOpenGL::PipelineOpenGL(const PipelineDescription &description)
        : Pipeline(description)
    {
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(0);
    }

    PipelineOpenGL::~PipelineOpenGL()
    {
        glDeleteVertexArrays(1, &m_VAO);
    }

    const PipelineDescription &PipelineOpenGL::GetPipelineDescription() const
    {
        return m_Description;
    }

    void PipelineOpenGL::Bind()
    {
        SetupDepthStencil();
        SetupRasterizer();
        SetupBlending();
        SetShader();
        BindVertexArray();
    }

    void PipelineOpenGL::SetupDepthStencil()
    {
        // enable/disable depth testing
        if (m_Description.DepthStencilDescription.EnableDepthTest)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        // enable/disable depth writing
        if (m_Description.DepthStencilDescription.EnableDepthWrite)
        {
            glDepthMask(GL_TRUE);
        }
        else
        {
            glDepthMask(GL_FALSE);
        }

        // set up stencil options
        if (m_Description.DepthStencilDescription.EnableStencilTest)
        {
            glEnable(GL_STENCIL_TEST);
        }
        else
        {
            glDisable(GL_STENCIL_TEST);
        }

        GLenum sfail = GL::GetStencilOperation(m_Description.DepthStencilDescription.StencilFailOperation);
        GLenum dpfail = GL::GetStencilOperation(m_Description.DepthStencilDescription.StencilSuccessDepthFailOperation);
        GLenum dppass = GL::GetStencilOperation(m_Description.DepthStencilDescription.StencilSuccessDepthSuccessOperation);

        glStencilOp(sfail, dpfail, dppass);
        GLenum stencilFunction = GL::GetComparisonFunction(m_Description.DepthStencilDescription.StencilComparisonFunction);
        glStencilFunc(stencilFunction, 1, m_Description.DepthStencilDescription.StencilMask);
        glStencilMask(m_Description.DepthStencilDescription.StencilMask);

        GLenum depthFunction = GL::GetComparisonFunction(m_Description.DepthStencilDescription.DepthComparisonFunction);
        glDepthFunc(depthFunction);

        glDepthRangef(m_Description.DepthStencilDescription.MinDepth, m_Description.DepthStencilDescription.MaxDepth);
    }

    void PipelineOpenGL::SetupRasterizer()
    {
        if (m_Description.RasterizerStateDescription.CullMode == CullMode::None)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
        }

        switch (m_Description.RasterizerStateDescription.CullMode)
        {
        case CullMode::Back:
            glCullFace(GL_BACK);
            break;
        case CullMode::Front:
            glCullFace(GL_FRONT);
            break;
        }

#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)
        if (m_Description.RasterizerStateDescription.DepthClipEnabled)
        {
            glEnable(GL_DEPTH_CLAMP);
        }
        else
        {
            glDisable(GL_DEPTH_CLAMP);
        }
#endif

#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)
        switch (m_Description.RasterizerStateDescription.FillMode)
        {
        case FillMode::Solid:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case FillMode::Wireframe:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        }
#endif

        switch (m_Description.RasterizerStateDescription.FrontFace)
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

            auto sourceColourFunction = GL::GetBlendFunction(m_Description.BlendStateDescription.SourceColourBlend);
            auto sourceAlphaFunction = GL::GetBlendFunction(m_Description.BlendStateDescription.SourceAlphaBlend);

            auto destinationColourFunction = GL::GetBlendFunction(m_Description.BlendStateDescription.DestinationColourBlend);
            auto destinationAlphaFunction = GL::GetBlendFunction(m_Description.BlendStateDescription.DestinationAlphaBlend);
            // glBlendFuncSeparate(destinationColourFunction, destinationColourFunction, destinationColourFunction, destinationAlphaFunction);
            glBlendFuncSeparate(sourceColourFunction, destinationColourFunction, sourceAlphaFunction, destinationAlphaFunction);

            auto blendEquation = GL::GetBlendEquation(m_Description.BlendStateDescription.BlendEquation);
            glBlendEquation(blendEquation);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    void PipelineOpenGL::SetShader()
    {
        if (!m_Description.Shader)
        {
            throw std::runtime_error("A shader has not been assigned to this pipeline!");
        }

        auto shaderGL = (ShaderOpenGL *)m_Description.Shader;
        shaderGL->Bind();
    }

    void PipelineOpenGL::BindVertexArray()
    {
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

    void PipelineOpenGL::BindVertexBuffers(const std::map<uint32_t, Nexus::Graphics::VertexBufferOpenGL *> vertexBuffers, uint32_t vertexOffset, uint32_t instanceOffset)
    {
        BindVertexArray();

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