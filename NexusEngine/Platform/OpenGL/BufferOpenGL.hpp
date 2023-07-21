#pragma once

#include "GL.hpp"
#include "Core/Graphics/Buffer.hpp"

#include "Platform/OpenGL/ShaderOpenGL.hpp"

namespace Nexus::Graphics
{
    class VertexBufferOpenGL : public VertexBuffer
    {
    public:
        VertexBufferOpenGL(const BufferDescription &description, const void *data, const VertexBufferLayout &layout);

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        void Bind();
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
    };

    class IndexBufferOpenGL : public IndexBuffer
    {
    public:
        IndexBufferOpenGL(const BufferDescription &description, const void *data);

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        void Bind();
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
    };

    class UniformBufferOpenGL : public UniformBuffer
    {
    public:
        UniformBufferOpenGL(const BufferDescription &description, const void *data);

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
    };
}