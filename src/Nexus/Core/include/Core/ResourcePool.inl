#pragma once

#include "Core/ResourceHandle.hpp"
#include "Core/ResourcePool.hpp"

namespace Nexus
{
	template<typename T, typename Handle>
	SharedHandle<T, Handle>::SharedHandle(ResourcePool<T, Handle> *pool, Handle handle)
		: m_Pool(pool),
		  m_Handle(handle),
		  m_Control(std::shared_ptr<void>(nullptr, [pool, handle](void *) { pool->Destroy(handle); }))
	{
	}

	template<typename T, typename Handle>
	T *SharedHandle<T, Handle>::operator->()
	{
		return m_Pool->Get(m_Handle);
	}

	template<typename T, typename Handle>
	T &SharedHandle<T, Handle>::operator*()
	{
		return *m_Pool->Get(m_Handle);
	}

	template<typename T, typename Handle>
	bool SharedHandle<T, Handle>::Valid() const
	{
		return m_Pool && m_Pool->Get(m_Handle);
	}

	template<typename T, typename Handle>
	Handle SharedHandle<T, Handle>::Raw() const
	{
		return m_Handle;
	}

	template<typename T, typename Handle>
	UniqueHandle<T, Handle>::UniqueHandle(ResourcePool<T, Handle> *pool, Handle handle) : m_Pool(pool),
																						  m_Handle(handle)
	{
	}

	template<typename T, typename Handle>
	UniqueHandle<T, Handle>::UniqueHandle(UniqueHandle &&other) noexcept : m_Pool(other.m_Pool),
																		   m_Handle(other.m_Handle)
	{
		other.m_Pool = nullptr;
	}

	template<typename T, typename Handle>
	UniqueHandle<T, Handle> &UniqueHandle<T, Handle>::operator=(UniqueHandle &&other) noexcept
	{
		if (this != &other)
		{
			Reset();
			m_Pool		 = other.m_Pool;
			m_Handle	 = other.m_Handle;
			other.m_Pool = nullptr;
		}
		return *this;
	}

	template<typename T, typename Handle>
	UniqueHandle<T, Handle>::~UniqueHandle()
	{
		Reset();
	}

	template<typename T, typename Handle>
	void UniqueHandle<T, Handle>::Reset()
	{
		if (m_Pool)
		{
			m_Pool->Destroy(m_Handle);
			m_Pool = nullptr;
		}
	}

	template<typename T, typename Handle>
	T *UniqueHandle<T, Handle>::operator->()
	{
		return m_Pool->Get(m_Handle);
	}

	template<typename T, typename Handle>
	T &UniqueHandle<T, Handle>::operator*()
	{
		return *m_Pool->Get(m_Handle);
	}

	template<typename T, typename Handle>
	bool UniqueHandle<T, Handle>::Valid() const
	{
		return m_Pool && m_Pool->Get(m_Handle);
	}

	template<typename T, typename Handle>
	Handle UniqueHandle<T, Handle>::Raw() const
	{
		return m_Handle;
	}

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
	template<typename... Args>
	Shared ResourcePool<T, Handle>::CreateShared(const T &resource)
	{
		Handle h = Create(resource);
		return Shared(this, h);
	}

	template<typename T, typename Handle>
	template<typename... Args>
	Shared ResourcePool<T, Handle>::EmplaceShared(Args &&...args)
	{
		Handle h = Emplace(std::forward<Args>(args)...);
		return Shared(this, h);
	}

	template<typename T, typename Handle>
	Unique ResourcePool<T, Handle>::CreateUnique(const T &resource)
	{
		Handle h = Create(resource);
		return Unique(this, h);
	}

	template<typename T, typename Handle>
	template<typename... Args>
	Unique ResourcePool<T, Handle>::EmplaceUnique(Args &&...args)
	{
		Handle h = Emplace(std::forward<Args>(args)...);
		return Unique(this, h);
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