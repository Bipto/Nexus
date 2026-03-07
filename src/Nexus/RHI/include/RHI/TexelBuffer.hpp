#pragma once

#include "RHI/DeviceBuffer.hpp"
#include "RHI/RHI-Core.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::Graphics
{
	struct TexelBufferDescription
	{
		Ref<IDeviceBuffer> Buffer	   = nullptr;
		size_t			   Offset	   = 0;
		size_t			   SizeInBytes = 0;
		PixelFormat		   Format	   = {};
	};

	class NX_RHI_API ITexelBuffer
	{
	  public:
		virtual ~ITexelBuffer()										 = default;
		virtual const TexelBufferDescription &GetDescription() const = 0;
	};
}	 // namespace Nexus::Graphics