#pragma once

#include "Nexus-Core/nxpch.hpp"

#include <vulkan/vulkan.h>

namespace Nexus::Vk
{
	class PNextBuilder
	{
	  public:
		PNextBuilder()			= default;
		virtual ~PNextBuilder() = default;

		template<typename T>
		T *Add(T &element)
		{
			element.pNext = nullptr;
			if (!m_Head)
			{
				m_Head = &element;
			}
			else
			{
				auto *last	= static_cast<VkBaseOutStructure *>(m_ChainElements.back());
				last->pNext = (VkBaseOutStructure *)&element;
			}

			m_ChainElements.push_back(&element);
			return &element;
		}

		void *GetHead() const
		{
			return m_Head;
		}

	  private:
		std::vector<void *> m_ChainElements = {};
		void			   *m_Head			= nullptr;
	};
}	 // namespace Nexus::Vk