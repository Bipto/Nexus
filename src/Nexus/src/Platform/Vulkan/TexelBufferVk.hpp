#pragma once

#include "GraphicsDeviceVk.hpp"
#include "Nexus-Core/Graphics/TexelBuffer.hpp"

namespace Nexus::Graphics
{
	class TexelBufferVk : public ITexelBuffer
	{
	  public:
		TexelBufferVk(const TexelBufferDescription &desc, GraphicsDeviceVk *device);
		virtual ~TexelBufferVk();
		const TexelBufferDescription &GetDescription() const final;
		VkBufferView				  GetVkBufferView() const;

	  private:
		GraphicsDeviceVk	  *m_Device		 = nullptr;
		TexelBufferDescription m_Description = {};
		VkBufferView		   m_BufferView	 = VK_NULL_HANDLE;
	};
}	 // namespace Nexus::Graphics