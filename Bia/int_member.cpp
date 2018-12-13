#include "int_member.hpp"
#include "share.hpp"
#include "print.hpp"
#include "native_operator.hpp"
#include "native_test_operator.hpp"
#include "double_member.hpp"

#include <cstdio>


namespace bia
{
namespace framework
{
namespace native
{

thread_local int_member::tmp_value int_member::_tmp_value;

int_member::int_member(int32_t _value) : _data(_value)
{
}

int_member::int_member(int64_t _value)
{
	_data.get() = _value;
}

int_member::int_member(const data_type & _data) noexcept : _data(_data)
{
}

void BIA_MEMBER_CALLING_CONVENTION int_member::print() const
{
	machine::link::print(_data.get());
}

void BIA_MEMBER_CALLING_CONVENTION int_member::copy(member * _destination)
{
	_destination->replace_this<int_member>(_data.get());
}

void BIA_MEMBER_CALLING_CONVENTION int_member::refer(member * _destination)
{
	_destination->replace_this<int_member>(_data);
}

void BIA_MEMBER_CALLING_CONVENTION int_member::clone(member * _destination)
{
	copy(_destination);
}

void BIA_MEMBER_CALLING_CONVENTION int_member::operator_call(member * _destination, operator_type _operator, const member * _right)
{
	auto _flags = _right->flags();

	if (_destination) {
		BIA_NOT_IMPLEMENTED;
	} else {
		if (_flags & F_INT) {
			native_operation<true>::operate_integral(_data.get(), _operator, static_cast<const int_member*>(_right)->_data.get());
		} else if (_flags & F_BIG_INT) {
			BIA_NOT_IMPLEMENTED;
		} else if (_flags & F_TO_INT) {
			native_operation<true>::operate_integral(_data.get(), _operator, _right->to_int());
		} else {
			BIA_NOT_IMPLEMENTED;
		}
	}
}

void BIA_MEMBER_CALLING_CONVENTION int_member::operator_call_int32(member * _destination, operator_type _operator, int32_t _right)
{
	if (_destination) {
		native_operation<true>::operate_integral(_destination, _data.get(), _operator, _right);
	} else {
		native_operation<true>::operate_integral(_data.get(), _operator, _right);
	}
}

void BIA_MEMBER_CALLING_CONVENTION int_member::operator_call_int64(member * _destination, operator_type _operator, int64_t _right)
{
	if (_destination) {
		native_operation<true>::operate_integral(_destination, _data.get(), _operator, _right);
	} else {
		native_operation<true>::operate_integral(_data.get(), _operator, _right);
	}
}

void BIA_MEMBER_CALLING_CONVENTION int_member::operator_call_double(member * _destination, operator_type _operator, double _right)
{
	if (_destination) {
		native_operation<true>::operate_arithmetic(_destination, _data.get(), _operator, _right);
	} else {
		native_operation<true>::operate_arithmetic(_data.get(), _operator, _right);
	}
}

void BIA_MEMBER_CALLING_CONVENTION int_member::object_member(member * _destination, machine::string_manager::name_type _name)
{
	BIA_NOT_IMPLEMENTED;
}

int int_member::flags() const
{
	return F_INT | F_TO_DOUBLE | F_TO_INT;
}

int32_t BIA_MEMBER_CALLING_CONVENTION int_member::test() const
{
	return test_operation(_data.get());
}

int32_t BIA_MEMBER_CALLING_CONVENTION int_member::test_member(operator_type _operator, member * _right) const
{
	auto _flags = _right->flags();

	if (_flags & F_INT) {
		return test_operation(_data.get(), _operator, static_cast<const int_member*>(_right)->_data.get());
	} else if (_flags & F_BIG_INT) {
	} else if (_flags & F_TO_INT) {
		return test_operation(_data.get(), _operator, _right->to_int());
	}
	
	BIA_NOT_IMPLEMENTED;
}

int32_t BIA_MEMBER_CALLING_CONVENTION int_member::test_int32(operator_type _operator, int32_t _right) const
{
	return test_operation(_data.get(), _operator, _right);
}

int32_t BIA_MEMBER_CALLING_CONVENTION int_member::test_int64(operator_type _operator, int64_t _right) const
{
	return test_operation(_data.get(), _operator, _right);
}

int32_t BIA_MEMBER_CALLING_CONVENTION int_member::test_double(operator_type _operator, double _right) const
{
	return test_operation(_data.get(), _operator, _right);
}

int64_t int_member::to_int() const
{
	return _data.get();
}

double int_member::to_double() const
{
	return static_cast<double>(_data.get());
}

const char * int_member::to_cstring(utility::buffer_builder * _builder) const
{
	std::snprintf(_builder->buffer<char>(), _builder->capacity(), "%lli", _data.get());

	return _builder->buffer<char>();
}

void * int_member::native_data(native::NATIVE_TYPE _type)
{
	return const_cast<void*>(const_native_data(_type));
}

const void * int_member::const_native_data(native::NATIVE_TYPE _type) const
{
	switch (_type) {
	case NATIVE_TYPE::BOOL:
	case NATIVE_TYPE::INT_8:
	case NATIVE_TYPE::INT_16:
	case NATIVE_TYPE::INT_32:
	case NATIVE_TYPE::INT_64:
		return &_data.get();
	case NATIVE_TYPE::FLOAT:
		_tmp_value.float_value = static_cast<float>(_data.get());

		return &_tmp_value.float_value;
	case NATIVE_TYPE::DOUBLE:
		_tmp_value.double_value = static_cast<double>(_data.get());

		return &_tmp_value.double_value;
	default:
		break;
	}

	throw exception::type_error(BIA_EM_UNSUPPORTED_TYPE);
}

}
}
}