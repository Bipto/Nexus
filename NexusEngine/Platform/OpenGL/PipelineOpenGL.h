#pragma once

#include "Core/Graphics/Pipeline.h"

namespace Nexus::Graphics
{
    class PipelineOpenGL : public Pipeline
    {
    public:
        PipelineOpenGL(const PipelineDescription &description)
            : Pipeline(description)
        {
        }

        virtual ~PipelineOpenGL()
        {
        }

        virtual const PipelineDescription &GetPipelineDescription() const override;
        void Bind();

    private:
        void SetupDepthStencil();
        void SetupRasterizer();
        void SetupBlending();
        void SetShader();
    };
}