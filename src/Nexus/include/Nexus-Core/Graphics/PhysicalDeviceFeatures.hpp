#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	struct PhysicalDeviceLimits
	{
		uint32_t		  MaxDimensionTexture1D			 = 0;
		uint32_t		  MaxDimensionTexture2D			 = 0;
		uint32_t		  MaxDimensionTexture3D			 = 0;
		uint32_t		  MaxDimensionCubemap			 = 0;
		uint32_t		  MaxImageArrayLayers			 = 0;
		uint32_t		  MaxUniformBufferRange			 = 0;
		uint32_t		  MaxStorageBufferRange			 = 0;
		uint32_t		  MaxVertexInputAttributes		 = 0;
		uint32_t		  MaxVertexInputBindings		 = 0;
		uint32_t		  MaxVertexInputOffset			 = 0;
		uint32_t		  MaxVertexInputStride			 = 0;
		uint32_t		  MaxFramebufferWidth			 = 0;
		uint32_t		  MaxFramebufferHeight			 = 0;
		uint32_t		  MaxFramebufferLayers			 = 0;
		uint32_t		  MaxFramebufferColourAttachment = 0;
		uint32_t		  MaxComputeWorkGroupCount[3]	 = {0, 0, 0};
		uint32_t		  MaxComputeWorkGroupInvocations = 0;
		uint32_t		  MaxComputeSize[3]				 = {0, 0, 0};
		uint32_t		  MaxViewports					 = 0;
		Point2D<uint32_t> MaxViewportDimensions			 = {0, 0};
		uint32_t		  MaxSamplerCount				 = 0;
		uint32_t		  MaxUniformBufferBindingsPerSet = 0;
		uint32_t		  MaxStorageImages				 = 0;
		uint32_t		  MaxSampledImages				 = 0;
	};

	struct PhysicalDeviceFeatures
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
	};
}	 // namespace Nexus::Graphics