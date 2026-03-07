#pragma once

#include "D3D12Include.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::D3D12
{
	// this seems like the easiest way to force the correct 8 byte alignment for the D3D12 structures
	template<typename T>
	struct alignas(8) AlignedSubObject
	{
		D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type;
		// typename std::aligned_storage<sizeof(T), 8>::type Data;
		T Data;

		AlignedSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type, const T &value) : Type(type)
		{
			// std::memcpy(&Data, &value, sizeof(T));
			Type = type;
			Data = value;
		}
	};

	class StreamStateBuilder
	{
	  public:
		StreamStateBuilder()  = default;
		~StreamStateBuilder() = default;

		template<typename T>
		void AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE subobjectType, const T &subobject)
		{
			static_assert(std::is_trivially_copyable<T>::value, "Object must be trivially copyable");
			AlignedSubObject<T> alignedSubObject {subobjectType, subobject};
			AppendBytes(&alignedSubObject, sizeof(alignedSubObject));
		}

		void Clear()
		{
			m_Stream.clear();
		}

		void *GetStream()
		{
			return m_Stream.data();
		}

		size_t GetSizeInBytes() const
		{
			return m_Stream.size();
		}

	  private:
		void AppendBytes(const void *data, size_t size)
		{
			const uint8_t *bytePointer = (const uint8_t *)data;
			m_Stream.insert(m_Stream.end(), bytePointer, bytePointer + size);
		}

	  private:
		std::vector<uint8_t> m_Stream = {};
	};
}	 // namespace Nexus::D3D12