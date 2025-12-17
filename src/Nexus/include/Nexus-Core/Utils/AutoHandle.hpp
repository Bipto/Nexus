#include <functional>
#include <utility>

// Generic RAII wrapper for integer/opaque handles
template<typename HandleType>
class AutoHandle
{
  public:
	using Deleter = std::function<void(HandleType)>;

	AutoHandle(HandleType handle = HandleType {}, Deleter deleter = nullptr) : handle_(handle), deleter_(deleter)
	{
	}

	~AutoHandle()
	{
		release();
	}

	// Non-copyable
	AutoHandle(const AutoHandle &)			  = delete;
	AutoHandle &operator=(const AutoHandle &) = delete;

	// Movable
	AutoHandle(AutoHandle &&other) noexcept : handle_(other.handle_), deleter_(std::move(other.deleter_))
	{
		other.handle_  = HandleType {};
		other.deleter_ = nullptr;
	}

	AutoHandle &operator=(AutoHandle &&other) noexcept
	{
		if (this != &other)
		{
			release();
			handle_		   = other.handle_;
			deleter_	   = std::move(other.deleter_);
			other.handle_  = HandleType {};
			other.deleter_ = nullptr;
		}
		return *this;
	}

	HandleType get() const
	{
		return handle_;
	}
	explicit operator bool() const
	{
		return handle_ != HandleType {};
	}

  private:
	void release()
	{
		if (deleter_ && handle_ != HandleType {})
		{
			deleter_(handle_);
			handle_ = HandleType {};
		}
	}

	HandleType handle_;
	Deleter	   deleter_;
};
