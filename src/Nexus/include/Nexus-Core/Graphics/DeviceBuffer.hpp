#pragma once

#include "Nexus-Core/Vertex.hpp"
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

	enum class IndexBufferFormat
	{
		UInt16,
		UInt32
	};

	inline uint32_t GetIndexFormatSizeInBytes(IndexBufferFormat format)
	{
		switch (format)
		{
			case IndexBufferFormat::UInt16: return sizeof(uint16_t);
			case IndexBufferFormat::UInt32: return sizeof(uint32_t);
			default: throw std::runtime_error("Failed to find a valid index buffer format");
		}
	}

	struct DeviceBufferDescription
	{
		/// @brief The type of buffer to create
		DeviceBufferType Type = DeviceBufferType::Upload;

		/// @brief The full size of the buffer in bytes
		uint32_t SizeInBytes = 0;

		/// @brief The stride of each item in the buffer in bytes
		uint32_t StrideInBytes = 0;

		/// @brief Whether the CPU is able to directly read and write data to the buffer without first using an upload/readback buffer
		bool HostVisible = false;
	};

	class DeviceBuffer
	{
	  public:
		virtual ~DeviceBuffer()
		{
		}
		virtual void			  SetData(const void *data, uint32_t offset, uint32_t size) = 0;
		virtual std::vector<char> GetData(uint32_t offset, uint32_t size) const				= 0;

		virtual const DeviceBufferDescription &GetDescription() const = 0;

		uint32_t GetCount() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.SizeInBytes / description.StrideInBytes;
		}

		bool IsWriteable() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.Type == DeviceBufferType::Upload || description.HostVisible == true;
		}

		bool IsReadable() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.Type == DeviceBufferType::Upload || description.Type == DeviceBufferType::Readback || description.HostVisible == true;
		}
	};

	struct BufferCopyDescription
	{
		DeviceBuffer *Source	  = nullptr;
		DeviceBuffer *Destination = nullptr;
		uint64_t	  ReadOffset  = 0;
		uint64_t	  WriteOffset = 0;
		uint64_t	  Size		  = 0;
	};

	struct VertexBufferView
	{
		DeviceBuffer *BufferHandle = nullptr;
		uint64_t	  Offset	   = 0;
	};

	struct IndexBufferView
	{
		DeviceBuffer	 *BufferHandle = nullptr;
		uint64_t		  Offset	   = 0;
		IndexBufferFormat BufferFormat = IndexBufferFormat::UInt32;
	};

	struct UniformBufferView
	{
		DeviceBuffer *BufferHandle = nullptr;
		uint64_t	  Offset	   = 0;
		uint64_t	  Size		   = 0;
	};
}	 // namespace Nexus::Graphics