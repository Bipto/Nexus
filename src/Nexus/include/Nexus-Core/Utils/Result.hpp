#pragma once
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

template<typename T, typename E>
class Result
{
	std::variant<T, E> data;
	bool			   is_ok;

  public:
	// Constructors
	Result(const T &value) : data(value), is_ok(true)
	{
	}
	Result(T &&value) : data(std::move(value)), is_ok(true)
	{
	}
	Result(const E &error) : data(error), is_ok(false)
	{
	}
	Result(E &&error) : data(std::move(error)), is_ok(false)
	{
	}

	// Observers
	bool has_value() const noexcept
	{
		return is_ok;
	}
	explicit operator bool() const noexcept
	{
		return is_ok;
	}

	const T &value() const
	{
		if (!is_ok)
			throw std::logic_error("Accessing value from error Result");
		return std::get<T>(data);
	}

	T &value()
	{
		if (!is_ok)
			throw std::logic_error("Accessing value from error Result");
		return std::get<T>(data);
	}

	const E &error() const
	{
		if (is_ok)
			throw std::logic_error("Accessing error from value Result");
		return std::get<E>(data);
	}

	E &error()
	{
		if (is_ok)
			throw std::logic_error("Accessing error from value Result");
		return std::get<E>(data);
	}

	// Ergonomic operators
	const T &operator*() const
	{
		return value();
	}
	T *operator->()
	{
		return &value();
	}
	const T *operator->() const
	{
		return &value();
	}

	// Utility: map success
	template<typename F>
	auto map(F &&f) const -> Result<decltype(f(std::declval<T>())), E>
	{
		using U = decltype(f(std::declval<T>()));
		if (is_ok)
		{
			return Result<U, E>(f(std::get<T>(data)));
		}
		else
		{
			return Result<U, E>(std::get<E>(data));
		}
	}

	// Utility: map error
	template<typename F>
	auto map_error(F &&f) const -> Result<T, decltype(f(std::declval<E>()))>
	{
		using E2 = decltype(f(std::declval<E>()));
		if (is_ok)
		{
			return Result<T, E2>(std::get<T>(data));
		}
		else
		{
			return Result<T, E2>(f(std::get<E>(data)));
		}
	}

	// and_then: chain on success
	template<typename F>
	auto and_then(F &&f) const -> decltype(f(std::declval<T>()))
	{
		using NextResult = decltype(f(std::declval<T>()));
		if (is_ok)
		{
			return f(std::get<T>(data));
		}
		else
		{
			return NextResult(std::get<E>(data));
		}
	}

	// or_else: chain on error
	template<typename F>
	auto or_else(F &&f) const -> decltype(f(std::declval<E>()))
	{
		using NextResult = decltype(f(std::declval<E>()));
		if (is_ok)
		{
			return NextResult(std::get<T>(data));
		}
		else
		{
			return f(std::get<E>(data));
		}
	}

	// value_or: provide default
	T value_or(const T &default_value) const
	{
		return is_ok ? std::get<T>(data) : default_value;
	}

	// unwrap_or_else: compute fallback
	template<typename F>
	T unwrap_or_else(F &&f) const
	{
		return is_ok ? std::get<T>(data) : f(std::get<E>(data));
	}

	// match: pattern matching style
	template<typename FSuccess, typename FError>
	auto match(FSuccess &&fs, FError &&fe) const
	{
		if (is_ok)
			return fs(std::get<T>(data));
		else
			return fe(std::get<E>(data));
	}

	// Swap
	void swap(Result &other) noexcept
	{
		std::swap(data, other.data);
		std::swap(is_ok, other.is_ok);
	}

	// Comparison
	bool operator==(const Result &other) const
	{
		return data == other.data && is_ok == other.is_ok;
	}

	bool operator!=(const Result &other) const
	{
		return !(*this == other);
	}
};

// Stream output helper
template<typename T, typename E>
std::ostream &operator<<(std::ostream &os, const Result<T, E> &r)
{
	if (r.has_value())
		os << "Ok(" << r.value() << ")";
	else
		os << "Err(" << r.error() << ")";
	return os;
}
