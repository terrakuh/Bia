#pragma once

#include <cstdarg>
#include <cstdint>

#include "biaMember.hpp"
#include "biaException.hpp"
#include "biaMemberCreator.hpp"


namespace bia
{
namespace force
{

inline void DisguisedCaller(void(*p_pFunction)(), framework::BiaMember * p_pDestination)
{
	p_pFunction();

	framework::MemberCreator(p_pDestination);
}

template<typename _RETURN>
inline void DisguisedCaller(_RETURN(*p_pFunction)(), framework::BiaMember * p_pDestination)
{
	framework::MemberCreator(p_pDestination, p_pFunction());
}

template<typename _RETURN, typename... _ARGS>
inline void DisguisedCaller(_RETURN(*)(_ARGS...), framework::BiaMember*)
{
	throw exception::ArgumentException("Arguments expected.");
}

template<typename _RETURN>
inline void DisguisedCallerCount(_RETURN(*p_pFunction)(), framework::BiaMember * p_pDestination, framework::BiaMember::parameter_count p_count, va_list p_args)
{
	if (p_count != 0)
		throw exception::ArgumentException("Argument count does not match.");

	framework::MemberCreator(p_pDestination, p_pFunction());
}

template<typename _RETURN, typename _0>
inline void DisguisedCallerCount(_RETURN(*p_pFunction)(_0), framework::BiaMember * p_pDestination, framework::BiaMember::parameter_count p_count, va_list p_args)
{
	if (p_count != 1)
		throw exception::ArgumentException("Argument count does not match.");

	framework::MemberCreator(p_pDestination, p_pFunction(std::forward<_0>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 0))->Cast<_0>())));
}

template<typename _RETURN, typename _0, typename _1>
inline void DisguisedCallerCount(_RETURN(*p_pFunction)(_0, _1), framework::BiaMember * p_pDestination, framework::BiaMember::parameter_count p_count, va_list p_args)
{
	if (p_count != 2)
		throw exception::ArgumentException("Argument count does not match.");

	framework::MemberCreator(p_pDestination, p_pFunction(std::forward<_0>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 0))->Cast<_0>()), std::forward<_1>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 1))->Cast<_1>())));
}

template<typename _RETURN, typename _0, typename _1, typename _2>
inline void DisguisedCallerCount(_RETURN(*p_pFunction)(_0, _1, _2), framework::BiaMember * p_pDestination, framework::BiaMember::parameter_count p_count, va_list p_args)
{
	if (p_count != 3)
		throw exception::ArgumentException("Argument count does not match.");

	framework::MemberCreator(p_pDestination, p_pFunction(std::forward<_0>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 0))->Cast<_0>()), std::forward<_1>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 1))->Cast<_1>()), std::forward<_2>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 2))->Cast<_2>())));
}


inline void DisguisedCallerCount(void(*p_pFunction)(), framework::BiaMember * p_pDestination, framework::BiaMember::parameter_count p_count, va_list p_args)
{
	if (p_count != 0)
		throw exception::ArgumentException("Argument count does not match.");

	p_pFunction();

	framework::MemberCreator(p_pDestination);
}

template<typename _0>
inline void DisguisedCallerCount(void(*p_pFunction)(_0), framework::BiaMember * p_pDestination, framework::BiaMember::parameter_count p_count, va_list p_args)
{
	if (p_count != 1)
		throw exception::ArgumentException("Argument count does not match.");

	p_pFunction(std::forward<_0>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 0))->Cast<_0>()));

	framework::MemberCreator(p_pDestination);
}

template<typename _0, typename _1>
inline void DisguisedCallerCount(void(*p_pFunction)(_0, _1), framework::BiaMember * p_pDestination, framework::BiaMember::parameter_count p_count, va_list p_args)
{
	if (p_count != 2)
		throw exception::ArgumentException("Argument count does not match.");

	p_pFunction(std::forward<_0>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 0))->Cast<_0>()), std::forward<_1>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 1))->Cast<_1>()));

	framework::MemberCreator(p_pDestination);
}

template<typename _0, typename _1, typename _2>
inline void DisguisedCallerCount(void(*p_pFunction)(_0, _1, _2), framework::BiaMember * p_pDestination, framework::BiaMember::parameter_count p_count, va_list p_args)
{
	if (p_count != 3)
		throw exception::ArgumentException("Argument count does not match.");

	p_pFunction(std::forward<_0>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 0))->Cast<_0>()), std::forward<_1>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 1))->Cast<_1>()), std::forward<_2>(*(*reinterpret_cast<framework::BiaMember**>(p_args + sizeof(framework::BiaMember*) * 2))->Cast<_2>()));

	framework::MemberCreator(p_pDestination);
}


}
}