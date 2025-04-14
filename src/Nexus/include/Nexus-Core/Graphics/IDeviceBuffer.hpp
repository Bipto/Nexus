#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	enum BufferType
	{
		Vertex,
		Index,
		Uniform,
		Upload,
		Readback,
		Indirect,
		Storage
	};

	struct BufferDescription
	{
		size_t	   SizeInBytes = 0;
		BufferType Type		   = BufferType::Vertex;
	};

	class IDeviceBuffer
	{
		virtual const BufferDescription &GetDescription() const														 = 0;
		virtual size_t					 GetSize() const															 = 0;
		virtual BufferType				 GetType() const															 = 0;
		virtual bool					 SetData()																	 = 0;
		virtual bool					 GetData(std::vector<unsigned char> &pixels, uint32_t offset, uint32_t size) = 0;
	};
}	 // namespace Nexus::Graphics