#include "TexelBufferD3D12.hpp"

namespace Nexus::Graphics
{
	TexelBufferD3D12::TexelBufferD3D12(const TexelBufferDescription &desc) : m_Description(desc)
	{
	}

	TexelBufferD3D12::~TexelBufferD3D12()
	{
	}

	const TexelBufferDescription &TexelBufferD3D12::GetDescription() const
	{
		return m_Description;
	}
}	 // namespace Nexus::Graphics