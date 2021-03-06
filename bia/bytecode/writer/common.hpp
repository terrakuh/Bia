#ifndef BIA_BYTECODE_WRITER_COMMON_HPP_
#define BIA_BYTECODE_WRITER_COMMON_HPP_

#include "../op_code.hpp"

#include <bia/util/limit_checker.hpp>
#include <bia/util/portable/stream.hpp>
#include <bia/util/type_traits/equals_any.hpp>
#include <bia/util/type_traits/type_index.hpp>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <type_traits>

namespace bia {
namespace bytecode {
namespace writer {

template<typename T>
using is_offset =
    util::type_traits::equals_any_type<typename std::decay<T>::type, std::int8_t, std::int16_t, std::int32_t>;
template<typename T>
using is_constant =
    util::type_traits::equals_any_type<typename std::decay<T>::type, std::int8_t, std::int32_t, std::int64_t,
                                       double, test_register, std::nullptr_t>;
template<typename T>
using is_empty_constant =
    util::type_traits::equals_any_type<typename std::decay<T>::type, test_register, std::nullptr_t>;

template<bool Optimize, typename T>
inline typename std::enable_if<is_empty_constant<T>::value>::type optimized_write(std::ostream& output,
                                                                                  T value)
{}

template<bool Optimize, typename T>
inline typename std::enable_if<
    util::type_traits::equals_any_type<typename std::decay<T>::type, op_code, std::int8_t, std::int16_t,
                                       std::int32_t, std::int64_t, std::uint8_t, std::uint16_t,
                                       std::uint32_t>::value != 0>::type
    optimized_write(std::ostream& output, T value)
{
	typedef typename std::decay<T>::type type;

	if (Optimize && !std::is_same<type, op_code>::value) {
		if (std::is_unsigned<type>::value) {
			if (util::limit_checker<std::uint8_t>::in_bounds(value)) {
				util::portable::write(output, static_cast<std::uint8_t>(value));
			} else if (util::limit_checker<std::uint16_t>::in_bounds(value)) {
				util::portable::write(output, static_cast<std::uint16_t>(value));
			} else {
				util::portable::write(output, value);
			}
		} else {
			if (util::limit_checker<std::int8_t>::in_bounds(value)) {
				util::portable::write(output, static_cast<std::int8_t>(value));
			} else if (util::limit_checker<std::int16_t>::in_bounds(value)) {
				util::portable::write(output, static_cast<std::int16_t>(value));
			} else if (util::limit_checker<std::int32_t>::in_bounds(value)) {
				util::portable::write(output, static_cast<std::int32_t>(value));
			} else {
				util::portable::write(output, value);
			}
		}
	} // don't optimize
	else {
		util::portable::write(output, value);
	}
}

template<bool Optimize>
inline void optimized_write(std::ostream& output, double value)
{
	static_assert(sizeof(value) == sizeof(std::int64_t), "invalid double write");

	optimized_write<false>(output, *reinterpret_cast<std::int64_t*>(&value));
}

template<bool Optimize, typename T>
inline typename std::enable_if<is_offset<T>::value, offset_option>::type offset_index(T value) noexcept
{
	typedef typename std::decay<T>::type type;

	if (Optimize) {
		if (util::limit_checker<std::int8_t>::in_bounds(value)) {
			return oo_8;
		} else if (util::limit_checker<std::int16_t>::in_bounds(value)) {
			return oo_16;
		}

		return oo_32;
	}

	switch (sizeof(type)) {
	case 1: return oo_8;
	case 2: return oo_16;
	case 4: return oo_32;
	}
}

template<bool Optimize, typename T>
inline typename std::enable_if<is_empty_constant<T>::value, constant_option>::type
    constant_index(T value) noexcept
{
	return static_cast<constant_option>(
	    constant_option::co_test_register +
	    util::type_traits::type_index<typename std::decay<T>::type, test_register, std::nullptr_t>::value);
}

template<bool Optimize, typename T>
inline typename std::enable_if<is_constant<T>::value && !is_empty_constant<T>::value, constant_option>::type
    constant_index(T value) noexcept
{
	typedef typename std::decay<T>::type type;

	if (Optimize && std::is_integral<type>::value) {
		if (util::limit_checker<std::int8_t>::in_bounds(value)) {
			return co_int_8;
		} else if (util::limit_checker<std::int16_t>::in_bounds(value)) {
			return co_int_16;
		} else if (util::limit_checker<std::int32_t>::in_bounds(value)) {
			return co_int_32;
		}

		return co_int_64;
	}

	return static_cast<constant_option>(
	    util::type_traits::type_index<type, std::int8_t, std::int16_t, std::int32_t, std::int64_t,
	                                  double>::value);
}

} // namespace writer
} // namespace bytecode
} // namespace bia

#endif
