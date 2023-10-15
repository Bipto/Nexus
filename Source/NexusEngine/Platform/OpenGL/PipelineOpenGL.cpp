#include "PipelineOpenGL.hpp"

#include "ShaderOpenGL.hpp"
#include "BufferOpenGL.hpp"

#include "GL.hpp"

namespace Nexus::Graphics
{
    PipelineOpenGL::PipelineOpenGL(const PipelineDescription &description)
        : Pipeline(description)
    {
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

    GLenum GetBlendFunction(BlendFunction function)
    {
        switch (function)
        {
        case BlendFunction::Zero:
            return GL_ZERO;
        case BlendFunction::One:
            return GL_ONE;
        case BlendFunction::SourceColor:
            return GL_SRC_COLOR;
        case BlendFunction::OneMinusSourceColor:
            return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFunction::DestinationColor:
            return GL_DST_COLOR;
        case BlendFunction::OneMinusDestinationColor:
            return GL_ONE_MINUS_DST_COLOR;
        case BlendFunction::SourceAlpha:
            return GL_SRC_ALPHA;
        case BlendFunction::OneMinusSourceAlpha:
            return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFunction::DestinationAlpha:
            return GL_DST_ALPHA;
        case BlendFunction::OneMinusDestinationAlpha:
            return GL_ONE_MINUS_DST_ALPHA;
        }
    }

    GLenum GetBlendEquation(BlendEquation equation)
    {
        switch (equation)
        {
        case BlendEquation::Add:
            return GL_FUNC_ADD;
        case BlendEquation::Subtract:
            return GL_FUNC_SUBTRACT;
        case BlendEquation::ReverseSubtract:
            return GL_FUNC_REVERSE_SUBTRACT;
        case BlendEquation::Min:
            return GL_MIN;
        case BlendEquation::Max:
            return GL_MAX;
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

#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
        if (m_Description.RasterizerStateDescription.DepthClipEnabled)
        {
            glEnable(GL_DEPTH_CLAMP);
        }
        else
        {
            glDisable(GL_DEPTH_CLAMP);
        }
#endif

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
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

        auto &viewport = m_Description.Viewport;
        glViewport(
            viewport.X,
            viewport.Y,
            viewport.Width,
            viewport.Height);
        glDepthRangef(viewport.MinDepth, viewport.MaxDepth);

        auto &scissorRectangle = m_Description.RasterizerStateDescription.ScissorRectangle;
        glScissor(
            scissorRectangle.X,
            scissorRectangle.Y,
            scissorRectangle.Width,
            scissorRectangle.Height);
    }

    void PipelineOpenGL::SetupBlending()
    {
        if (m_Description.BlendStateDescription.EnableBlending)
        {
            glEnable(GL_BLEND);

            auto sourceColourFunction = GetBlendFunction(m_Description.BlendStateDescription.SourceColourBlend);
            auto sourceAlphaFunction = GetBlendFunction(m_Description.BlendStateDescription.SourceAlphaBlend);
            glBlendFuncSeparate(sourceColourFunction, sourceColourFunction, sourceColourFunction, sourceAlphaFunction);

            auto destinationColourFunction = GetBlendFunction(m_Description.BlendStateDescription.DestinationColourBlend);
            auto destinationAlphaFunction = GetBlendFunction(m_Description.BlendStateDescription.DestinationAlphaBlend);
            glBlendFuncSeparate(destinationColourFunction, destinationColourFunction, destinationColourFunction, destinationAlphaFunction);

            auto blendEquation = GetBlendEquation(m_Description.BlendStateDescription.BlendEquation);
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
}
