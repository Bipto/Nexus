#pragma once

#include "Nexus/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{
    class PipelineOpenGL : public Pipeline
    {
    public:
        PipelineOpenGL(const PipelineDescription &description);
        virtual ~PipelineOpenGL();
        virtual const PipelineDescription &GetPipelineDescription() const override;
        void Bind();

    private:
        void SetupDepthStencil();
        void SetupRasterizer();
        void SetupBlending();
        void SetShader();
    };
}