#pragma once

#include <stdexcept>
#include <vector>

#include "RHI/DeviceAddress.hpp"
#include "RHI/RHI-Core.hpp"
#include "RHI/Types.hpp"

#include "Core/Utils.hpp"

namespace Nexus::Graphics
{
	enum class IndexFormat
	{
		UInt8,
		UInt16,
		UInt32
	};

	inline uint32_t GetIndexFormatSizeInBytes(IndexFormat format)
	{
		switch (format)
		{
			case IndexFormat::UInt8: return sizeof(uint8_t);
			case IndexFormat::UInt16: return sizeof(uint16_t);
			case IndexFormat::UInt32: return sizeof(uint32_t);
			default: throw std::runtime_error("Failed to find a valid index buffer format");
		}
	}

	enum class BufferMemoryAccess
	{
		/// @brief The buffer memory is accessible to the CPU and GPU and is optimized for transferring from CPU to GPU. Can also be used to read back
		/// data, although this may not be as optimized as with Readback.
		Upload,

		/// @brief The buffer memory is resident on the GPU and is inaccessible to the CPU. Recommended for most resources.
		Default,

		/// @brief The buffer memory is accessible to the CPU and GPU, however it is an error to attempt to write to this memory with the CPU.
		Readback
	};

	enum BufferCreateFlags : uint8_t
	{
		BufferCreateFlags_None			= 0,
		BufferCreateFlags_SparseBinding = BIT(0)
	};

	enum BufferUsage : uint16_t
	{
		BufferUsage_None									= 0,
		BufferUsage_Vertex									= BIT(0),
		BufferUsage_Index									= BIT(1),
		BufferUsage_Uniform									= BIT(2),
		BufferUsage_Storage									= BIT(3),
		BufferUsage_Indirect								= BIT(4),
		BufferUsage_AccelerationStructureStorage			= BIT(5),
		BufferUsage_AccelerationStructureBuildInputReadOnly = BIT(6),
		BufferUsage_ShaderBindingTable						= BIT(7),
		BufferUsage_TransformFeedback						= BIT(8),
		BufferUsage_TexelUniform							= BIT(9),
		BufferUsage_TexelStorage							= BIT(10)
	};

	struct DeviceBufferDescription
	{
		/// @brief Flags containing how the buffer should be created
		BufferCreateFlags Flags = BufferCreateFlags_None;

		/// @brief The accessibility of the buffer
		BufferMemoryAccess Access = BufferMemoryAccess::Default;

		/// @brief How the buffer is able to be used
		uint8_t Usage = 0;

		/// @brief The full size of the buffer in bytes
		size_t SizeInBytes = 0;

		/// @brief The stride of each item in the buffer in bytes
		size_t StrideInBytes = 0;

		/// @brief A debug name for the buffer, shows up in debugging tools
		std::string DebugName = "DeviceBuffer";
	};

	struct BufferRange
	{
		size_t Offset = 0;
		size_t Size	  = 0;
	};

	class IDeviceBuffer
	{
	  public:
		virtual ~IDeviceBuffer()
		{
		}
		virtual void			  SetData(const void *data, uint32_t offset, uint32_t size) = 0;
		virtual std::vector<char> GetData(uint32_t offset, uint32_t size)					= 0;
		virtual DeviceAddress	  GetDeviceAddress(size_t offset) const						= 0;

		[[nodiscard]] virtual const DeviceBufferDescription &GetDescription() const = 0;

		[[nodiscard]] virtual uint8_t *Map()						 = 0;
		virtual void				   Unmap()						 = 0;
		virtual void				   FlushRange(BufferRange range) = 0;

		uint32_t GetCount() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.SizeInBytes / description.StrideInBytes;
		}

		bool IsWriteable() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.Access == BufferMemoryAccess::Upload;
		}

		bool IsReadable() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.Access == BufferMemoryAccess::Upload || description.Access == BufferMemoryAccess::Readback;
		}

		bool CheckUsage(BufferUsage usage) const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.Usage & usage;
		}

		size_t GetSizeInBytes() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.SizeInBytes;
		}

		size_t GetStrideInBytes() const
		{
			const DeviceBufferDescription &description = GetDescription();
			return description.StrideInBytes;
		}
	};

	struct VertexBufferView
	{
		Ref<IDeviceBuffer> BufferHandle = {};
		size_t			   Offset		= 0;
		size_t			   Size			= 0;
	};

	struct IndexBufferView
	{
		Ref<IDeviceBuffer> BufferHandle = {};
		size_t			   Offset		= 0;
		size_t			   Size			= 0;
		IndexFormat		   BufferFormat = IndexFormat::UInt32;
	};

	struct UniformBufferView
	{
		Ref<IDeviceBuffer> BufferHandle = {};
		size_t			   Offset		= 0;
		size_t			   Size			= 0;
	};
}	 // namespace Nexus::Graphics