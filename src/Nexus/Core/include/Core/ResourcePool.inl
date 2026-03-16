#pragma once

#include "Core/ResourceHandle.hpp"
#include "Core/ResourcePool.hpp"

namespace Nexus
{
	template<typename T, typename Handle>
	Handle ResourcePool<T, Handle>::Create(const T &resource)
	{
		uint32_t index;

		if (!m_FreeList.empty())
		{
			index = m_FreeList.back();
			m_FreeList.pop_back();
		}
		else
		{
			index = m_Entries.size();
			m_Entries.emplace_back();
		}

		Entry &entry   = m_Entries[index];
		entry.resource = resource;
		entry.alive	   = true;

		return Handle(index, entry.generation, this);
	}

	template<typename T, typename Handle>
	template<typename... Args>
	Handle ResourcePool<T, Handle>::Emplace(Args &&...args)
	{
		uint64_t index = 0;

		if (!m_FreeList.empty())
		{
			index = m_FreeList.back();
			m_FreeList.pop_back();
		}
		else
		{
			index = m_Entries.size();
			m_Entries.emplace_back();
		}

		Entry &e   = m_Entries[index];
		e.resource = T(std::forward<Args>(args)...);
		e.alive	   = true;

		return Handle(index, e.generation, this);
	}

	template<typename T, typename Handle>
	void ResourcePool<T, Handle>::Destroy(Handle handle)
	{
		// we need to validate that this resource handle belongs to this pool
		if (handle.GetParent() != this)
		{
			return;
		}

		uint64_t index = handle.GetIndex();
		Entry	&entry = m_Entries[index];

		entry.alive = false;
		entry.generation++;
		m_FreeList.push_back(index);
	}

	template<typename T, typename Handle>
	T *ResourcePool<T, Handle>::Get(Handle handle)
	{
		// we need to validate that this resource handle belongs to this pool
		if (handle.GetParent() != this)
		{
			return nullptr;
		}

		uint64_t index = handle.GetIndex();
		if (index >= m_Entries.size())
		{
			return nullptr;
		}

		Entry &e = m_Entries[index];

		if (!e.alive || e.generation != handle.GetGeneration())
		{
			return nullptr;
		}

		return &e.resource;
	}
}	 // namespace Nexus