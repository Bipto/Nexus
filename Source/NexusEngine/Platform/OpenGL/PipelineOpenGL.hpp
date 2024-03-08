#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/Pipeline.hpp"
#include "BufferOpenGL.hpp"

namespace Nexus::Graphics
{
    class PipelineOpenGL : public Pipeline
    {
    public:
        PipelineOpenGL(const PipelineDescription &description);
        virtual ~PipelineOpenGL();
        virtual const PipelineDescription &GetPipelineDescription() const override;
        void BindVertexBuffer(VertexBufferOpenGL *vertexBuffer, uint32_t index, uint32_t offset);
        void Bind();

    private:
        void SetupDepthStencil();
        void SetupRasterizer();
        void SetupBlending();
        void SetShader();
        void BindVertexArray();
        void SetupVertexElements(uint32_t offset);

    private:
        unsigned int m_VAO = 0;
    };
}

#endif