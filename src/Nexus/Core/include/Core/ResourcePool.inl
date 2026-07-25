#pragma once

#include "Core/ResourcePool.hpp"

namespace Nexus
{
    template <typename Tag>
    HandleT<Tag>::HandleT(uint32_t index, uint32_t generation, void *parent)
        : m_Index(index), m_Generation(generation), m_Parent(parent)
    {
    }

    template <typename Tag> uint32_t HandleT<Tag>::GetIndex() const
    {
        return m_Index;
    }

    template <typename Tag> uint32_t HandleT<Tag>::GetGeneration() const
    {
        return m_Generation;
    }

    template <typename Tag> inline void *HandleT<Tag>::GetParent() const
    {
        return m_Parent;
    }

    template <typename T, typename Handle>
    SharedHandle<T, Handle>::SharedHandle(ResourcePool<T, Handle> *pool, Handle handle)
        : m_Pool(pool), m_Handle(handle), m_Control(std::shared_ptr<void>(nullptr, [pool, handle](void *) {
              if (pool)
              {
                  pool->Destroy(handle);
              }
          }))
    {
    }

    template <typename T, typename Handle> T *SharedHandle<T, Handle>::operator->()
    {
        return m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> const T *SharedHandle<T, Handle>::operator->() const
    {
        return m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> T &SharedHandle<T, Handle>::operator*()
    {
        return *m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> const T &SharedHandle<T, Handle>::operator*() const
    {
        return *m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> bool SharedHandle<T, Handle>::IsValid() const
    {
        return m_Pool && m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> Handle SharedHandle<T, Handle>::Raw() const
    {
        return m_Handle;
    }

    template <typename T, typename Handle> T *SharedHandle<T, Handle>::GetResource()
    {
        if (!m_Pool)
        {
            return nullptr;
        }

        return m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> const T *SharedHandle<T, Handle>::GetResource() const
    {
        if (!m_Pool)
        {
            return nullptr;
        }

        return m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> template <typename Derived> Derived *SharedHandle<T, Handle>::AsDerived()
    {
        T *ptr = m_Pool->Get(m_Handle);
        return dynamic_cast<Derived *>(ptr);
    }

    template <typename T, typename Handle>
    template <typename Derived>
    const Derived *SharedHandle<T, Handle>::AsDerived() const
    {
        T *ptr = m_Pool->Get(m_Handle);
        return dynamic_cast<Derived *>(ptr);
    }

    template <typename T, typename Handle>
    UniqueHandle<T, Handle>::UniqueHandle(ResourcePool<T, Handle> *pool, Handle handle) : m_Pool(pool), m_Handle(handle)
    {
    }

    template <typename T, typename Handle>
    UniqueHandle<T, Handle>::UniqueHandle(UniqueHandle &&other) noexcept
        : m_Pool(other.m_Pool), m_Handle(other.m_Handle)
    {
        other.m_Pool = nullptr;
    }

    template <typename T, typename Handle>
    UniqueHandle<T, Handle> &UniqueHandle<T, Handle>::operator=(UniqueHandle &&other) noexcept
    {
        if (this != &other)
        {
            Reset();
            m_Pool = other.m_Pool;
            m_Handle = other.m_Handle;
            other.m_Pool = nullptr;
        }
        return *this;
    }

    template <typename T, typename Handle> UniqueHandle<T, Handle>::~UniqueHandle()
    {
        Reset();
    }

    template <typename T, typename Handle> void UniqueHandle<T, Handle>::Reset()
    {
        if (m_Pool)
        {
            m_Pool->Destroy(m_Handle);
            m_Pool = nullptr;
        }
    }

    template <typename T, typename Handle> T *UniqueHandle<T, Handle>::operator->()
    {
        return m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> const T *UniqueHandle<T, Handle>::operator->() const
    {
        return m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> T &UniqueHandle<T, Handle>::operator*()
    {
        return *m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> const T &UniqueHandle<T, Handle>::operator*() const
    {
        return *m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> bool UniqueHandle<T, Handle>::IsValid() const
    {
        return m_Pool && m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> Handle UniqueHandle<T, Handle>::Raw() const
    {
        return m_Handle;
    }

    template <typename T, typename Handle> T *UniqueHandle<T, Handle>::GetResource()
    {
        if (!m_Pool)
        {
            return nullptr;
        }

        return m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> template <typename Derived> Derived *UniqueHandle<T, Handle>::AsDerived()
    {
        T *ptr = m_Pool->Get(m_Handle);
        return static_cast<Derived *>(ptr);
    }

    template <typename T, typename Handle>
    template <typename Derived>
    const Derived *UniqueHandle<T, Handle>::AsDerived() const
    {
        T *ptr = m_Pool->Get(m_Handle);
        return static_cast<Derived *>(ptr);
    }

    template <typename T, typename Handle> const T *UniqueHandle<T, Handle>::GetResource() const
    {
        if (!m_Pool)
        {
            return nullptr;
        }

        return m_Pool->Get(m_Handle);
    }

    template <typename T, typename Handle> Handle ResourcePool<T, Handle>::Create(std::unique_ptr<T> resource)
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

        Entry &entry = m_Entries[index];
        entry.resource = std::move(resource);
        entry.alive = true;

        return Handle(index, entry.generation, this);
    }

    template <typename T, typename Handle>
    template <typename... Args>
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

        Entry &e = m_Entries[index];
        e.resource = std::make_unique<T>(std::forward<Args>(args)...);
        e.alive = true;

        return Handle(index, e.generation, this);
    }

    template <typename T, typename Handle> void ResourcePool<T, Handle>::Destroy(Handle handle)
    {
        // we need to validate that this resource handle belongs to this pool
        if (handle.GetParent() != this)
        {
            return;
        }

        uint64_t index = handle.GetIndex();

        if (index >= m_Entries.size())
        {
            return;
        }

        Entry &entry = m_Entries[index];
        entry.resource = nullptr;

        entry.alive = false;
        entry.generation++;
        m_FreeList.push_back(index);
    }

    template <typename T, typename Handle>
    SharedHandle<T, Handle> ResourcePool<T, Handle>::CreateShared(std::unique_ptr<T> resource)
    {
        Handle h = Create(std::move(resource));
        return SharedHandle<T, Handle>(this, h);
    }

    template <typename T, typename Handle>
    template <typename... Args>
    SharedHandle<T, Handle> ResourcePool<T, Handle>::EmplaceShared(Args &&...args)
    {
        Handle h = Emplace(std::forward<Args>(args)...);
        return SharedHandle<T, Handle>(this, h);
    }

    template <typename T, typename Handle>
    UniqueHandle<T, Handle> ResourcePool<T, Handle>::CreateUnique(std::unique_ptr<T> resource)
    {
        Handle h = Create(std::move(resource));
        return UniqueHandle<T, Handle>(this, h);
    }

    template <typename T, typename Handle>
    template <typename... Args>
    UniqueHandle<T, Handle> ResourcePool<T, Handle>::EmplaceUnique(Args &&...args)
    {
        Handle h = Emplace(std::forward<Args>(args)...);
        return UniqueHandle<T, Handle>(this, h);
    }

    template <typename T, typename Handle> T *ResourcePool<T, Handle>::Get(Handle handle)
    {
        // we need to validate that this resource handle belongs to this pool
        if (handle.GetParent() == nullptr || handle.GetParent() != this)
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

        return e.resource.get();
    }
} // namespace Nexus