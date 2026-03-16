#include "ResourceHandle.hpp"
#pragma once

#include <cstdint>

#include "Core/ResourceHandle.hpp"

namespace Nexus
{
	template<typename Tag>
	HandleT<Tag>::HandleT(uint32_t index, uint32_t generation, void *parent) : m_Index(index),
																			   m_Generation(generation),
																			   m_Parent(parent)
	{
	}

	template<typename Tag>
	uint32_t HandleT<Tag>::GetIndex() const
	{
		return m_Index;
	}

	template<typename Tag>
	uint32_t HandleT<Tag>::GetGeneration() const
	{
		return m_Generation;
	}

	template<typename Tag>
	inline void *HandleT<Tag>::GetParent() const
	{
		return m_Parent;
	}

}	 // namespace Nexus