#pragma once

#if defined(WIN32)

#include "Core/Graphics/Buffer.hpp"
#include "DX11.hpp"

namespace Nexus::Graphics
{
    class VertexBufferDX11 : public VertexBuffer
    {
    public:
        VertexBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data, const VertexBufferLayout &layout);

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        ID3D11Buffer *GetHandle();

    private:
        ID3D11Buffer *m_Buffer;
        ID3D11DeviceContext *m_Context;
    };

    class IndexBufferDX11 : public IndexBuffer
    {
    public:
        IndexBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data);

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        ID3D11Buffer *GetHandle();

    private:
        ID3D11Buffer *m_Buffer;
        ID3D11DeviceContext *m_Context;
    };

    class UniformBufferDX11 : public UniformBuffer
    {
    public:
        UniformBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data);

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        ID3D11Buffer *GetHandle();

    private:
        ID3D11Buffer *m_Buffer;
        ID3D11DeviceContext *m_Context;
    };
}

#endif