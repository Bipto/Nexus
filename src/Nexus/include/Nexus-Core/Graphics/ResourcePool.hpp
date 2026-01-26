#pragma once

#include "Nexus-Core/Graphics/ResourceHandle.hpp"

namespace Nexus::Graphics
{
	template<typename T, typename Handle>
	class ResourcePool
	{
	  public:
		struct Entry
		{
			T		 resource {};
			uint32_t generation = 0;
			bool	 alive		= false;
		};

		Handle Create(const T &resource)
		{
			uint64_t index;

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

			return Handle(index, entry.generation);
		}

		template<typename... Args>
		Handle Emplace(Args &&...args)
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

			return Handle(index, e.generation);
		}

		void Destroy(Handle handle)
		{
			uint64_t index = handle.GetIndex();
			Entry	&entry = m_Entries[index];

			entry.alive = false;
			entry.generation++;
			m_FreeList.push_back(index);
		}

		T *Get(Handle handle)
		{
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

	  private:
		std::vector<Entry>	  m_Entries	 = {};
		std::vector<uint64_t> m_FreeList = {};
	};
}	 // namespace Nexus::Graphics