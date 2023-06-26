#include "PipelineOpenGL.h"

#include "ShaderOpenGL.h"

#include "GL.h"

namespace Nexus
{
    const PipelineDescription &PipelineOpenGL::GetPipelineDescription() const
    {
        return m_Description;
    }

    void PipelineOpenGL::Bind()
    {
        SetupDepthStencil();
        SetupRasterizer();
        SetShader();
    }

    GLenum GetStencilOperation(StencilOperation operation)
    {
        switch (operation)
        {
        case StencilOperation::Keep:
            return GL_KEEP;
        case StencilOperation::Zero:
            return GL_ZERO;
        case StencilOperation::Replace:
            return GL_REPLACE;
        case StencilOperation::Increment:
            return GL_INCR;
        case StencilOperation::Decrement:
            return GL_DECR;
        case StencilOperation::Invert:
            return GL_INVERT;
        }
    }

    GLenum GetComparisonFunction(ComparisonFunction function)
    {
        switch (function)
        {
        case ComparisonFunction::Always:
            return GL_ALWAYS;
        case ComparisonFunction::Equal:
            return GL_EQUAL;
        case ComparisonFunction::Greater:
            return GL_GREATER;
        case ComparisonFunction::GreaterEqual:
            return GL_GEQUAL;
        case ComparisonFunction::Less:
            return GL_LESS;
        case ComparisonFunction::LessEqual:
            return GL_LEQUAL;
        case ComparisonFunction::Never:
            return GL_NEVER;
        case ComparisonFunction::NotEqual:
            return GL_NOTEQUAL;
        }
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

        GLenum sfail = GetStencilOperation(m_Description.DepthStencilDescription.StencilFailOperation);
        GLenum dpfail = GetStencilOperation(m_Description.DepthStencilDescription.StencilSuccessDepthFailOperation);
        GLenum dppass = GetStencilOperation(m_Description.DepthStencilDescription.StencilSuccessDepthSuccessOperation);

        glStencilOp(sfail, dpfail, dppass);
        GLenum stencilFunction = GetComparisonFunction(m_Description.DepthStencilDescription.StencilComparisonFunction);
        glStencilFunc(stencilFunction, 1, m_Description.DepthStencilDescription.StencilMask);
        glStencilMask(m_Description.DepthStencilDescription.StencilMask);

        GLenum depthFunction = GetComparisonFunction(m_Description.DepthStencilDescription.DepthComparisonFunction);
        glDepthFunc(depthFunction);
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

        if (m_Description.RasterizerStateDescription.DepthClipEnabled)
        {
            glEnable(GL_DEPTH_CLAMP);
        }
        else
        {
            glDisable(GL_DEPTH_CLAMP);
        }

#if !defined(EMSCRIPTEN)
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

        if (m_Description.RasterizerStateDescription.ScissorTestEnabled)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }

        auto &scissorRectangle = m_Description.RasterizerStateDescription.ScissorRectangle;
        glScissor(
            scissorRectangle.X,
            scissorRectangle.Y,
            scissorRectangle.Width,
            scissorRectangle.Height);
    }

    void PipelineOpenGL::SetShader()
    {
        if (!m_Description.Shader)
        {
            throw std::runtime_error("A shader has not been assigned to this pipeline!");
        }

        Ref<ShaderOpenGL> shaderGL = std::dynamic_pointer_cast<ShaderOpenGL>(m_Description.Shader);
        shaderGL->Bind();
    }
}
