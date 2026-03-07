#pragma once

#include <cstdint>

namespace Nexus::Graphics
{
	template<typename Tag>
	struct HandleT
	{
	  public:
		HandleT(uint64_t index, uint32_t generation) : m_Index(index), m_Generation(generation)
		{
		}

		uint64_t GetIndex() const
		{
			return m_Index;
		}

		uint32_t GetGeneration() const
		{
			return m_Generation;
		}

	  private:
		uint64_t m_Index	  = 0;
		uint32_t m_Generation = 0;
	};

	struct TextureTag
	{
	};

	struct BufferTag
	{
	};

	struct SamplerTag
	{
	};

	struct ResourceSetTag
	{
	};

	using TextureHandle		= HandleT<TextureTag>;
	using BufferHandle		= HandleT<BufferTag>;
	using SamplerHandle		= HandleT<SamplerTag>;
	using ResourceSetHandle = HandleT<ResourceSetTag>;

}	 // namespace Nexus::Graphics