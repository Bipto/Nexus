#pragma once

#if defined(WIN32)

#include "Core/Graphics/Buffer.hpp"
#include "DX11.hpp"

namespace Nexus::Graphics
{
    /// @brief A class representing a vertex buffer to be used with the DirectX11 backend
    class VertexBufferDX11 : public VertexBuffer
    {
    public:
        /// @brief A constructor creating a new vertex buffer
        /// @param device A pointer to an ID3D11Device* to use to create the underlying buffer
        /// @param context A pointer to an ID3D11DeviceContext to use to update the buffer
        /// @param description A const reference to a BufferDescription to describe how the buffer should be represented in memory
        /// @param data A const pointer to a set of data to use as the initial data of the buffer
        /// @param layout A const reference to a VertexBufferLayout describing the layout of the vertex buffer
        VertexBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data, const VertexBufferLayout &layout);

        /// @brief A destructor to clean up resources
        ~VertexBufferDX11();

        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;

        /// @brief A method that returns a pointer to the ID3D11Buffer handle
        /// @return A pointer to the ID3D11Buffers
        ID3D11Buffer *GetHandle();

    private:
        /// @brief A pointer to an underlying ID3D11Buffer representing the vertex buffer
        ID3D11Buffer *m_Buffer;

        /// @brief A pointer to an ID3D11DeviceContext to use to create and update the buffer
        ID3D11DeviceContext *m_Context;
    };

    /// @brief A class representing an index buffer to be used with the DirectX11 backend
    class IndexBufferDX11 : public IndexBuffer
    {
    public:
        /// @brief A constructor creating a new index buffer
        /// @param device A pointer to an ID3D11Device* to use to create the underlying buffer
        /// @param context A pointer to an ID3D11DeviceContext to use to update the buffer
        /// @param description A const reference to a BufferDescription to describe how the buffer should be represented in memory
        /// @param data A const pointer to a set of data to use as the initial data of the buffer
        IndexBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data);

        /// @brief A destructor to clean up resources
        ~IndexBufferDX11();

        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;

        /// @brief A method that returns a pointer to the ID3D11Buffer handle
        /// @return A pointer to the ID3D11Buffers
        ID3D11Buffer *GetHandle();

    private:
        /// @brief A pointer to an underlying ID3D11Buffer representing the index buffer
        ID3D11Buffer *m_Buffer;

        /// @brief A pointer to an ID3D11DeviceContext to use to create and update the buffer
        ID3D11DeviceContext *m_Context;
    };

    /// @brief A class representing a uniform buffer to be used with the DirectX11 backend
    class UniformBufferDX11 : public UniformBuffer
    {
    public:
        /// @brief A constructor creating a new uniform buffer
        /// @param device A pointer to an ID3D11Device* to use to create the underlying buffer
        /// @param context A pointer to an ID3D11DeviceContext to use to update the buffer
        /// @param description A const reference to a BufferDescription to describe how the buffer should be represented in memory
        /// @param data A const pointer to a set of data to use as the initial data of the buffer
        UniformBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data);

        /// @brief A destructor to clean up resources
        ~UniformBufferDX11();

        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;

        /// @brief A method that returns a pointer to the ID3D11Buffer handle
        /// @return A pointer to the ID3D11Buffers
        ID3D11Buffer *GetHandle();

    private:
        /// @brief A pointer to an underlying ID3D11Buffer representing the uniform buffer
        ID3D11Buffer *m_Buffer;

        /// @brief A pointer to an ID3D11DeviceContext to use to create and update the buffer
        ID3D11DeviceContext *m_Context;
    };
}

#endif