#ifndef CTM_OPTIONAL_CWPARSER
#define CTM_OPTIONAL_CWPARSER


#if __cplusplus >= 201703L

#include <optional>

#else

#	include <stdexcept>
#	include <utility>
namespace std
{
template <typename T>
class optional
{
public:
	optional()
		: has_value_(false)
		, value_(T{})
	{ }

	optional(const T& value)
		: has_value_(true)
		, value_(value)
	{ }

	optional(T&& value)
		: has_value_(true)
		, value_(std::move(value))
	{ }

	optional(const optional& other)
	{
		if(other.has_value_)
		{
			has_value_ = true;
			value_ = other.value_;
		}
		else
		{
			has_value_ = false;
		}
	}

	optional(optional&& other) noexcept
	{
		if(other.has_value_)
		{
			has_value_ = true;
			value_ = std::move(other.value_);
			other.has_value_ = false;
		}
		else
		{
			has_value_ = false;
		}
	}

	T& operator*()
	{
		return value_;
	}

	T* operator->()
	{
		return &value_;
	}

	optional& operator=(const optional& other)
	{
		if(this != &other)
		{
			if(other.has_value_)
			{
				has_value_ = true;
				value_ = other.value_;
			}
			else
			{
				has_value_ = false;
			}
		}
		return *this;
	}

	optional& operator=(optional&& other) noexcept
	{
		if(this != &other)
		{
			if(other.has_value_)
			{
				has_value_ = true;
				value_ = std::move(other.value_);
				other.has_value_ = false;
			}
			else
			{
				has_value_ = false;
			}
		}
		return *this;
	}

	bool operator==(T&& other)
	{
		if(has_value_)
			return value_ == other;
		else
			return false;
	}
	bool operator!=(T&& other)
	{
		if(has_value_)
			return value_ == other;
		else
			return false;
	}
	bool operator<(T&& other)
	{
		if(has_value_)
			return value_ < other;
		else
			return false;
	}
	bool operator<=(T&& other)
	{
		if(has_value_)
			return value_ <= other;
		else
			return false;
	}

	bool operator>(T&& other)
	{
		if(has_value_)
			return value_ > other;
		else
			return false;
	}
	bool operator>=(T&& other)
	{
		if(has_value_)
			return value_ >= other;
		else
			return false;
	}

	explicit operator bool() const
	{
		return has_value_;
	}

	bool has_value() const
	{
		return has_value_;
	}

	T& value()
	{
		if(!has_value_)
		{
			throw std::runtime_error("No value present");
		}
		return value_;
	}

	const T& value() const
	{
		if(!has_value_)
		{
			throw std::runtime_error("No value present");
		}
		return value_;
	}

	void reset()
	{
		has_value_ = false;
	}

private:
	bool has_value_;
	T value_;
};

} // namespace std
#endif

#endif