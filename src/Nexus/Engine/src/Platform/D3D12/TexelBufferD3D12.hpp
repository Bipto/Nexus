#pragma once

#include "GraphicsDeviceD3D12.hpp"
#include "RHI/TexelBuffer.hpp"

namespace Nexus::Graphics
{
	class TexelBufferD3D12 : public ITexelBuffer
	{
	  public:
		TexelBufferD3D12(const TexelBufferDescription &desc);
		virtual ~TexelBufferD3D12();
		const TexelBufferDescription &GetDescription() const final;

	  private:
		TexelBufferDescription m_Description = {};
	};
}	 // namespace Nexus::Graphics