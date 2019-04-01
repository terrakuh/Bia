#pragma once

#include "cstring_member_def.hpp"
#include "share.hpp"
#include "print.hpp"


namespace bia
{
namespace framework
{
namespace native
{

template<typename Char_type>
inline cstring_member<Char_type>::cstring_member(const Char_type * _string)
{
	_data->template create<machine::cstring_resource>(_string);
}

template<typename Char_type>
inline cstring_member<Char_type>::cstring_member(machine::string_manager::string_wrapper<Char_type> _string) noexcept : _data(*_string.string)
{
}

template<typename Char_type>
inline cstring_member<Char_type>::cstring_member(const data_type & _data) noexcept : _data(_data)
{
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::print() const
{
	print(_data->get()->template string<char>());
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::copy(member * _destination)
{
	_destination->template replace_this<cstring_member<Char_type>>(_data);
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::refer(member * _destination)
{
	_destination->template replace_this<cstring_member<Char_type>>(_data);
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::clone(member * _destination)
{
	copy(_destination);
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::operator_call(member * _destination, operator_t _operator, const member * _right)
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_OPERATION);
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::operator_call_int32(member * _destination, operator_t _operator, int32_t _right)
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_OPERATION);
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::operator_call_int64(member * _destination, operator_t _operator, int64_t _right)
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_OPERATION);
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::operator_call_big_int(member * _destination, operator_t _operator, const detail::big_int * _right)
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_OPERATION);
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::operator_call_double(member * _destination, operator_t _operator, double _right)
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_OPERATION);
}

template<typename Char_type>
inline void BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::object_member(member * _destination, machine::name_manager::name_t _name)
{
	BIA_NOT_IMPLEMENTED;
}

template<typename Char_type>
inline int cstring_member<Char_type>::flags() const
{
	return F_CSTRING;
}

template<typename Char_type>
inline int32_t BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::test() const
{
	return static_cast<int32_t>(_data->get()->length() != 0);
}

template<typename Char_type>
inline int32_t BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::test_member(operator_t _operator, member * _right) const
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_TEST);
}

template<typename Char_type>
inline int32_t BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::test_int32(operator_t _operator, int32_t _right) const
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_TEST);
}

template<typename Char_type>
inline int32_t BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::test_int64(operator_t _operator, int64_t _right) const
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_TEST);
}

template<typename Char_type>
inline int32_t BIA_MEMBER_CALLING_CONVENTION cstring_member<Char_type>::test_double(operator_t _operator, double _right) const
{
	throw exception::execution_error(BIA_EM_UNSUPPORTED_TEST);
}

template<typename Char_type>
inline int64_t cstring_member<Char_type>::to_int() const
{
	throw exception::type_error(BIA_EM_UNSUPPORTED_TYPE);
}

template<typename Char_type>
inline double cstring_member<Char_type>::to_double() const
{
	throw exception::type_error(BIA_EM_UNSUPPORTED_TYPE);
}

template<typename Char_type>
inline const char * cstring_member<Char_type>::to_cstring(utility::buffer_builder * _builder) const
{
	///TODO
//	static_assert(std::is_same<Char_type, char>::value, "Unsupported char type");
	BIA_NOT_IMPLEMENTED;
	//return _data->get()->string<char>();
}

template<typename Char_type>
inline void * cstring_member<Char_type>::data(const std::type_info & _type, bool & _success)
{
	return nullptr;
}

template<typename Char_type>
inline const void * cstring_member<Char_type>::const_data(const std::type_info & _type, bool & _success) const
{
	if (_type == typeid(Char_type)) {
		_success = true;

		return _data->get()->template string<Char_type>();
	}

	return nullptr;
}

template<typename Char_type>
inline void cstring_member<Char_type>::print(const char * _string)
{
	machine::link::print_string(_string);
}

template<typename Char_type>
template<typename Type>
inline void cstring_member<Char_type>::print(const Type * _string) noexcept
{
}

}
}
}
