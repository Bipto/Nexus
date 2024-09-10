#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/GPUBuffer.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceD3D12;

	class VertexBufferD3D12 : public VertexBuffer
	{
	  public:
		VertexBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data);
		virtual ~VertexBufferD3D12();

		virtual void	 SetData(const void *data, uint32_t size, uint32_t offset = 0) override;
		ID3D12Resource2 *GetHandle();

	  private:
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_VertexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer = nullptr;
		// D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		D3D12_RANGE			 m_UploadRange;
		GraphicsDeviceD3D12 *m_Device = nullptr;
	};

	class IndexBufferD3D12 : public IndexBuffer
	{
	  public:
		IndexBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data, IndexBufferFormat format);
		virtual ~IndexBufferD3D12();
		void						  CreateIndexBufferFormat();
		const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

		virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

	  private:
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_IndexBuffer  = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer = nullptr;
		D3D12_INDEX_BUFFER_VIEW					m_IndexBufferView;
		D3D12_RANGE								m_UploadRange;
		DXGI_FORMAT								m_IndexBufferFormat;
		GraphicsDeviceD3D12					   *m_Device = nullptr;
	};

	class UniformBufferD3D12 : public UniformBuffer
	{
	  public:
		UniformBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data);
		virtual ~UniformBufferD3D12();
		ID3D12Resource2 *GetHandle();

		virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

	  private:
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_ConstantBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer	 = nullptr;
		D3D12_RANGE								m_UploadRange;
		GraphicsDeviceD3D12					   *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics
#endif