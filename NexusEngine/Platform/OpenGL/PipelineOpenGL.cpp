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

        // set up depth comparison function
        switch (m_Description.DepthStencilDescription.ComparisonFunction)
        {
        case ComparisonFunction::Always:
            glDepthFunc(GL_ALWAYS);
            break;
        case ComparisonFunction::Equal:
            glDepthFunc(GL_EQUAL);
            break;
        case ComparisonFunction::Greater:
            glDepthFunc(GL_GREATER);
            break;
        case ComparisonFunction::GreaterEqual:
            glDepthFunc(GL_GEQUAL);
            break;
        case ComparisonFunction::Less:
            glDepthFunc(GL_LESS);
            break;
        case ComparisonFunction::LessEqual:
            glDepthFunc(GL_LEQUAL);
            break;
        case ComparisonFunction::Never:
            glDepthFunc(GL_NEVER);
            break;
        case ComparisonFunction::NotEqual:
            glDepthFunc(GL_NOTEQUAL);
            break;
        }
    }

    void PipelineOpenGL::SetupRasterizer()
    {
        if (m_Description.RasterizerStateDescription.CullingEnabled)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }

        switch (m_Description.RasterizerStateDescription.CullMode)
        {
        case CullMode::Back:
            glCullFace(GL_BACK);
            break;
        case CullMode::Front:
            glCullFace(GL_FRONT);
            break;
        case CullMode::Both:
            glCullFace(GL_FRONT_AND_BACK);
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

        switch (m_Description.RasterizerStateDescription.FillMode)
        {
        case FillMode::Solid:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case FillMode::Wireframe:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        }

        switch (m_Description.RasterizerStateDescription.FrontFace)
        {
        case FrontFace::Clockwise:
            glFrontFace(GL_CW);
            break;
        case FrontFace::CounterClockwise:
            glFrontFace(GL_CCW);
            break;
        }
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
