#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	enum class DeviceBufferType
	{
		Vertex,
		Vertex,
		Index,
		Uniform,
		Structured,
		Upload,
		Readback
	};

	struct DeviceBufferDescription
	{
		/// @brief The type of buffer to create
		DeviceBufferType Type = DeviceBufferType::Upload;

		/// @brief Whether the CPU can directly access the buffer, it is an error to use GetData() or SetData() is this flag is not set
		bool HostAccessible = false;

		/// @brief The full size of the buffer in bytes
		uint32_t SizeInBytes = 0;

		/// @brief The stride of each item in the buffer in bytes, it is an error for this value to be set to anything other than 0 if the
		/// DeviceBufferType is anything other than DeviceBufferType::Structured
		uint32_t StrideInBytes = 0;
	};

	class DeviceBuffer
	{
	  public:
		virtual void						   SetData(const void *data, uint32_t offset, uint32_t size) = 0;
		virtual std::vector<char>			   GetData(uint32_t offset, uint32_t size)					 = 0;
		virtual const DeviceBufferDescription &GetDescription()											 = 0;
	};
}	 // namespace Nexus::Graphics