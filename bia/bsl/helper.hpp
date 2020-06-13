#ifndef BIA_BSL_HELPER_HPP_
#define BIA_BSL_HELPER_HPP_

#include <bia/gc/gc.hpp>
#include <bia/member/function/static_.hpp>
#include <bia/member/native/dict.hpp>
#include <bia/member/native/string.hpp>
#include <bia/util/gsl.hpp>
#include <cstring>
#include <string>

namespace bia {
namespace bsl {

template<typename Return, typename... Args>
inline void put_function(gc::gc& gc, member::native::dict& dict, util::not_null<util::czstring> name,
                         Return (*function)(Args...))
{
	const auto length   = std::char_traits<char>::length(name.get());
	const auto name_mem = static_cast<char*>(gc.allocate(length + 1).release());

	std::memcpy(name_mem, name.get(), length);

	name_mem[length] = 0;

	dict.put(gc.construct<member::native::string>(name_mem).release(),
	         gc.construct<member::function::static_<Return, Args...>>(function).release());
}

} // namespace bsl
} // namespace bia

#endif