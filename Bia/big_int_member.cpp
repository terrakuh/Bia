#include "big_int_member.hpp"
#include "share.hpp"


namespace bia
{
namespace framework
{
namespace native
{

template<typename Right>
inline void big_int_member_operation(dependency::big_int & _left, dependency::big_int & _destination, operator_type _operator, Right && _right)
{
	switch (_operator) {
	case O_ASSIGN:
		_left.set(_right);

		return;
	case O_ASSIGN_PLUS:
	case O_PLUS:
	{
		_left.add(_right, _destination);

		return;
	}
	case O_ASSIGN_MINUS:
	case O_MINUS:
	{
		_left.subtract(_right, _destination);

		return;
	}
	/*case O_ASSIGN_MULTIPLY:
	case O_MULTIPLY:
	{
		_left.multiply(_right, _destination);

		return;
	}
	case O_ASSIGN_DIVIDE:
	case O_DIVIDE:
	{
		_left.divide(_right, _destination);

		return;
	}
	case O_ASSIGN_MODULUS:
	case O_MODULUS:
	{
		_left.modulo(_right, _destination);

		return;
	}
	case O_ASSIGN_DOUBLE_MULTIPLY:
	case O_DOUBLE_MULTIPLY:
	{
		_left.power(_right, _destination);

		return;
	}*/
	default:
		break;
	}

	throw exception::operator_error(BIA_EM_UNSUPPORTED_OPERATOR);
}

template<typename Right>
inline void big_int_member_operation(dependency::big_int & _left, operator_type _operator, Right && _right)
{
	switch (_operator) {
	case O_ASSIGN:
		_left.set(_right);

		return;
	case O_ASSIGN_PLUS:
	case O_PLUS:
	{
		_left.add(_right);

		return;
	}
	case O_ASSIGN_MINUS:
	case O_MINUS:
	{
		_left.subtract(_right);

		return;
	}
	case O_ASSIGN_MULTIPLY:
	case O_MULTIPLY:
	{
		_left.multiply(_right);

		return;
	}
	case O_ASSIGN_DIVIDE:
	case O_DIVIDE:
	{
		_left.divide(_right);

		return;
	}
	case O_ASSIGN_MODULUS:
	case O_MODULUS:
	{
		_left.modulo(_right);

		return;
	}
	case O_ASSIGN_DOUBLE_MULTIPLY:
	case O_DOUBLE_MULTIPLY:
	{
		_left.power(_right);

		return;
	}
	default:
		break;
	}

	throw exception::operator_error(BIA_EM_UNSUPPORTED_OPERATOR);
}


thread_local big_int_member::tmp_value big_int_member::_tmp_value;

big_int_member::big_int_member()
{
}

big_int_member::big_int_member(int32_t _value) : _data(_value)
{
}

big_int_member::big_int_member(int64_t _value) : _data(_value)
{
}

big_int_member::big_int_member(const dependency::big_int & _value) : _data(_value)
{
}

big_int_member::big_int_member(const data_type & _data) noexcept : _data(_data)
{
}

void big_int_member::print() const
{
	_data->print(stdout);
	puts("");
}

void big_int_member::copy(member * _destination)
{
	_destination->template replace_this<big_int_member>(_data.get());
}

void big_int_member::refer(member * _destination)
{
	_destination->template replace_this<big_int_member>(_data);
}

void big_int_member::clone(member * _destination)
{
	copy(_destination);
}

void big_int_member::operator_call(member * _destination, operator_type _operator, const member * _right)
{
	auto _flags = _right->flags();

	if (_flags & F_BIG_INT) {
		if (_destination) {
			big_int_member_operation(_data.get(), _destination->template replace_this<big_int_member>()->_data.get(), _operator, static_cast<const big_int_member*>(_right)->_data.get());
		} else {
			big_int_member_operation(_data.get(), _operator, static_cast<const big_int_member*>(_right)->_data.get());
		}

		return;
	}

	BIA_NOT_IMPLEMENTED;
}

void big_int_member::operator_call_int32(member * _destination, operator_type _operator, int32_t _right)
{
	if (_destination) {
		big_int_member_operation(_data.get(), _destination->template replace_this<big_int_member>()->_data.get(), _operator, _right);
	} else {
		big_int_member_operation(_data.get(), _operator, _right);
	}
}

void big_int_member::operator_call_int64(member * _destination, operator_type _operator, int64_t _right)
{
	if (_destination) {
		big_int_member_operation(_data.get(), _destination->template replace_this<big_int_member>()->_data.get(), _operator, _right);
	} else {
		big_int_member_operation(_data.get(), _operator, _right);
	}
}

void BIA_MEMBER_CALLING_CONVENTION big_int_member::operator_call_big_int(member * _destination, operator_type _operator, const dependency::big_int * _right)
{
	if (_destination) {
		big_int_member_operation(_data.get(), _destination->template replace_this<big_int_member>()->_data.get(), _operator, *_right);
	} else {
		big_int_member_operation(_data.get(), _operator, *_right);
	}
}

void big_int_member::operator_call_double(member * _destination, operator_type _operator, double _right)
{
	if (_destination) {
		big_int_member_operation(_data.get(), _destination->template replace_this<big_int_member>()->_data.get(), _operator, _right);
	} else {
		big_int_member_operation(_data.get(), _operator, _right);
	}
}

void big_int_member::object_member(member * _destination, machine::string_manager::name_type _name)
{
	BIA_NOT_IMPLEMENTED;
}

int big_int_member::flags() const
{
	return F_BIG_INT | F_TO_INT | F_TO_DOUBLE;
}

int32_t big_int_member::test() const
{
	return static_cast<int32_t>(!_data->is_zero());
}

int32_t big_int_member::test_member(operator_type _operator, member * _right) const
{
	BIA_NOT_IMPLEMENTED;
}

int32_t big_int_member::test_int32(operator_type _operator, int32_t _right) const
{
	BIA_NOT_IMPLEMENTED;
}

int32_t big_int_member::test_int64(operator_type _operator, int64_t _right) const
{
	BIA_NOT_IMPLEMENTED;
}

int32_t big_int_member::test_double(operator_type _operator, double _right) const
{
	BIA_NOT_IMPLEMENTED;
}

int64_t big_int_member::to_int() const
{
	return _data->to_int();
}

double big_int_member::to_double() const
{
	return _data->to_double();
}

const char * big_int_member::to_cstring(utility::buffer_builder * _builder) const
{
	_data->to_string(*_builder);

	return _builder->buffer<char>();
}

void * big_int_member::native_data(native::NATIVE_TYPE _type)
{
	return const_cast<void*>(const_native_data(_type));
}

const void * big_int_member::const_native_data(native::NATIVE_TYPE _type) const
{
	switch (_type) {
	case NATIVE_TYPE::BOOL:
		_tmp_value.bool_value = !_data->is_zero();

		return &_tmp_value.bool_value;
	case NATIVE_TYPE::INT_8:
		_tmp_value.int8_value = static_cast<int8_t>(_data->cast_int());

		return &_tmp_value.int8_value;
	case NATIVE_TYPE::INT_16:
		_tmp_value.int16_value = static_cast<int16_t>(_data->cast_int());

		return &_tmp_value.int16_value;
	case NATIVE_TYPE::INT_32:
		_tmp_value.int32_value = static_cast<int32_t>(_data->cast_int());

		return &_tmp_value.int32_value;
	case NATIVE_TYPE::INT_64:
		_tmp_value.int64_value = _data->cast_int();

		return &_tmp_value.int64_value;
	case NATIVE_TYPE::FLOAT:
		_tmp_value.float_value = static_cast<float>(_data->cast_double());

		return &_tmp_value.float_value;
	case NATIVE_TYPE::DOUBLE:
		_tmp_value.double_value = _data->cast_double();

		return &_tmp_value.double_value;
	default:
		break;
	}

	throw exception::type_error(BIA_EM_UNSUPPORTED_TYPE);
}

}
}
}