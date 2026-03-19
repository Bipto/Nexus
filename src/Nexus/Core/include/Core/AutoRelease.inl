#pragma once

#include "Core/AutoRelease.hpp"

namespace Nexus
{
	template<typename Handle, Handle InvalidValue, typename Deleter>
	template<typename D>
		requires std::is_default_constructible_v<D>
	AutoRelease<Handle, InvalidValue, Deleter>::AutoRelease(Handle handle) noexcept : m_Handle(handle),
																					  m_Deleter()
	{
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	AutoRelease<Handle, InvalidValue, Deleter>::AutoRelease(Handle handle, Deleter deleter) noexcept : m_Handle(handle),
																									   m_Deleter(std::move(deleter))
	{
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	AutoRelease<Handle, InvalidValue, Deleter>::AutoRelease(AutoRelease &&other) noexcept
		: m_Handle(std::exchange(other.m_Handle, InvalidValue)),
		  m_Deleter(std::exchange(other.m_Deleter, {}))
	{
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	AutoRelease<Handle, InvalidValue, Deleter> &AutoRelease<Handle, InvalidValue, Deleter>::operator=(AutoRelease &&other) noexcept
	{
		if (this != &other)
		{
			Reset();
			m_Handle  = std::exchange(other.m_Handle, InvalidValue);
			m_Deleter = std::exchange(other.m_Deleter, {});
		}
		return *this;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	AutoRelease<Handle, InvalidValue, Deleter>::~AutoRelease() noexcept(DeleterNoexcept)
	{
		Reset();
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	[[nodiscard]] Handle AutoRelease<Handle, InvalidValue, Deleter>::Release() noexcept
	{
		return std::exchange(m_Handle, InvalidValue);
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	void AutoRelease<Handle, InvalidValue, Deleter>::Reset(Handle newHandle) noexcept(DeleterNoexcept)
	{
		if (m_Handle != InvalidValue)
			m_Deleter(m_Handle);

		m_Handle = newHandle;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	Handle &AutoRelease<Handle, InvalidValue, Deleter>::Get() noexcept
	{
		return m_Handle;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	const Handle &AutoRelease<Handle, InvalidValue, Deleter>::Get() const noexcept
	{
		return m_Handle;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	Deleter &AutoRelease<Handle, InvalidValue, Deleter>::GetDeleter() noexcept
	{
		return m_Deleter;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	const Deleter &AutoRelease<Handle, InvalidValue, Deleter>::GetDeleter() const noexcept
	{
		return m_Deleter;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	AutoRelease<Handle, InvalidValue, Deleter>::operator bool() const noexcept
	{
		return m_Handle != InvalidValue;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	bool AutoRelease<Handle, InvalidValue, Deleter>::IsValid() const noexcept
	{
		return m_Handle != InvalidValue;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	void AutoRelease<Handle, InvalidValue, Deleter>::Swap(AutoRelease &other) noexcept
	{
		std::swap(m_Handle, other.m_Handle);
		std::swap(m_Deleter, other.m_Deleter);
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	template<typename H>
		requires std::is_pointer_v<H>
	auto AutoRelease<Handle, InvalidValue, Deleter>::operator->() const noexcept
	{
		return m_Handle;
	}

	template<typename Handle, Handle InvalidValue, typename Deleter>
	template<typename H>
		requires std::is_pointer_v<H>
	auto &AutoRelease<Handle, InvalidValue, Deleter>::operator*() const noexcept
	{
		return *m_Handle;
	}
}	 // namespace Nexus