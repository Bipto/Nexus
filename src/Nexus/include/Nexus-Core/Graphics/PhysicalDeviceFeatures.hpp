#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	struct DeviceLimits
	{
		bool			  Texture1D							  = false;
		bool			  Texture2D							  = false;
		bool			  Texture3D							  = false;
		uint32_t		  MaxUniformBufferRange			 = 0;
		uint32_t		  MaxStorageBufferRange			 = 0;
		uint32_t		  MaxVertexInputAttributes		 = 0;
		uint32_t		  MaxVertexInputBindings		 = 0;
		uint32_t		  MaxVertexInputOffset			 = 0;
		uint32_t		  MaxVertexInputStride			 = 0;
		uint32_t		  MaxFramebufferColourAttachmentCount = 0;
		uint32_t		  MaxViewports						  = 0;
	};

	struct DeviceFeatures
	{
		bool SupportsGeometryShaders			  = false;
		bool SupportsTesselationShaders			  = false;
		bool SupportsComputeShaders				  = false;
		bool SupportsStorageBuffers				  = false;
		bool SupportsMultiviewport				  = false;
		bool SupportsSamplerAnisotropy			  = false;
		bool SupportsETC2Compression			  = false;
		bool SupportsASTC_LDRCompression		  = false;
		bool SupportsBCCompression				  = false;
		bool SupportShaderStorageImageMultisample = false;
		bool SupportsCubemapArray				  = false;
		bool SupportsIndependentBlend			  = false;
		bool SupportsMeshTaskShaders			  = false;
		bool SupportsDepthBoundsTesting			  = false;
	};
}	 // namespace Nexus::Graphics