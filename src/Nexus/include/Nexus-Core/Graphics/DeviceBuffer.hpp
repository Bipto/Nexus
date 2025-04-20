#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	enum class DeviceBufferType
	{
		Vertex,
		Index,
		Uniform,
		Structured,
		Upload,
		Readback,
		Indirect
	};

	struct DeviceBufferDescription
	{
		/// @brief The type of buffer to create
		DeviceBufferType Type = DeviceBufferType::Upload;

		/// @brief The full size of the buffer in bytes
		uint32_t SizeInBytes = 0;

		/// @brief The stride of each item in the buffer in bytes
		uint32_t StrideInBytes = 0;
	};

	class DeviceBuffer
	{
	  public:
		virtual ~DeviceBuffer()
		{
		}
		virtual void						   SetData(const void *data, uint32_t offset, uint32_t size) = 0;
		virtual std::vector<char>			   GetData(uint32_t offset, uint32_t size) const			 = 0;

		virtual const DeviceBufferDescription &GetDescription() const = 0;

		uint32_t GetCount() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.SizeInBytes / description.StrideInBytes;
		}

		bool IsHostAccessible() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.Type == DeviceBufferType::Upload || description.Type == DeviceBufferType::Readback;
		}
	};

	struct BufferCopyDescription
	{
		DeviceBuffer *Source	  = nullptr;
		DeviceBuffer *Target	  = nullptr;
		uint32_t	  ReadOffset  = 0;
		uint32_t	  WriteOffset = 0;
		uint32_t	  Size		  = 0;
	};
}	 // namespace Nexus::Graphics