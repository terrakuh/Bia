#pragma once

#include <member/function/static_function_member.hpp>
#include <member/member.hpp>
#include <member/native/int_member.hpp>
#include <type_traits>

namespace bia {
namespace creator {

class creator
{
public:
	typedef member::member* return_type;

	creator(gc::gc* gc) noexcept : gc(gc)
	{}

	template<typename T>
	typename std::enable_if<std::is_signed<T>::value, return_type>::type create_member(T value)
	{
		return gc->construct<member::native::int_member>(value);
	}
	return_type create_member(bool value)
	{
		return create_member(static_cast<int>(value));
	}
	return_type create_member(const char16_t* string)
	{
		BIA_IMPLEMENTATION_ERROR(u"not implemented");
	}
	template<typename Return, typename... Args>
	return_type create_member(Return (*function)(Args...))
	{
		return gc->construct<member::function::static_function_member<Return, Args...>>(function);
	}

private:
	gc::gc* gc;
};

} // namespace creator
} // namespace bia