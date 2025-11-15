#pragma once

#include "GraphicsDeviceD3D12.hpp"

#include "Nexus-Core/Graphics/TextureView.hpp"

namespace Nexus::Graphics
{
	class TextureViewD3D12 : public ITextureView
	{
	  public:
		TextureViewD3D12(const TextureViewDescription &desc);
		virtual ~TextureViewD3D12();
		const TextureViewDescription &GetDescription() const final;

	  private:
		TextureViewDescription m_Description = {};
	};
}	 // namespace Nexus::Graphics