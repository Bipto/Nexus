#pragma once

namespace Nexus::ECS
{
	template<typename T>
	ComponentArray<T>::~ComponentArray()
	{
	}

	template<typename T>
	size_t ComponentArray<T>::GetComponentCount()
	{
		return m_Components.size();
	}

	template<typename T>
	void *ComponentArray<T>::GetRawComponent(size_t index)
	{
		return &m_Components[index];
	}

	template<typename T>
	void ComponentArray<T>::RemoveComponent(size_t index)
	{
		m_Components.erase(m_Components.begin() + index);
	}

	template<typename T>
	const char *ComponentArray<T>::GetTypeName()
	{
		const std::type_info &info = typeid(T);
		return info.name();
	}

	template<typename T>
	void ComponentArray<T>::AddComponent(const T &component)
	{
		m_Components.push_back(component);
	}

	template<typename T>
	T *ComponentArray<T>::GetComponent(size_t index)
	{
		return &m_Components[index];
	}

	template<typename T>
	bool ComponentArray<T>::IsValidComponent(size_t index)
	{
		if (index > m_Components.size() || index == 0)
		{
			return false;
		}

		return true;
	}
}	 // namespace Nexus::ECS