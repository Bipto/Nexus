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
        void BindVertexBuffers(const std::map<uint32_t, Nexus::Graphics::VertexBufferOpenGL *> vertexBuffers, uint32_t vertexOffset, uint32_t instanceOffset);
        void Bind();

    private:
        void SetupDepthStencil();
        void SetupRasterizer();
        void SetupBlending();
        void SetShader();
        void BindVertexArray();
        void SetupVertexElements(uint32_t bufferIndex, uint32_t vertexOffset, uint32_t instanceOffset);

    private:
        unsigned int m_VAO = 0;
    };
}

#endif