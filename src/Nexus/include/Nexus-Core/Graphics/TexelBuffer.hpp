#pragma once

#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
	struct TexelBufferDescription
	{
		Ref<IDeviceBuffer> Buffer	   = nullptr;
		size_t			   Offset	   = 0;
		size_t			   SizeInBytes = 0;
		PixelFormat		   Format	   = {};
	};

	class ITexelBuffer
	{
	  public:
		virtual ~ITexelBuffer()										 = default;
		virtual const TexelBufferDescription &GetDescription() const = 0;
	};
}	 // namespace Nexus::Graphics