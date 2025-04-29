#pragma once

#include <inttypes.h>

namespace Nexus::Graphics
{
	struct IndirectDrawArguments
	{
		uint32_t VertexCount   = 0;
		uint32_t InstanceCount = 0;
		uint32_t FirstVertex   = 0;
		uint32_t FirstInstance = 0;
	};

	struct IndirectIndexedDrawArguments
	{
		uint32_t IndexCount	   = 0;
		uint32_t InstanceCount = 0;
		uint32_t FirstIndex	   = 0;
		uint32_t VertexOffset  = 0;
		uint32_t FirstInstance = 0;
	};

	struct IndirectDispatchArguments
	{
		uint32_t GroupCountX = 0;
		uint32_t GroupCountY = 0;
		uint32_t GroupCountZ = 0;
	};
}	 // namespace Nexus::Graphics