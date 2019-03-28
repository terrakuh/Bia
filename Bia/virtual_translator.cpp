#include "virtual_translator.hpp"
#include "exception.hpp"

#include <limits>


namespace bia
{
namespace machine
{
namespace virtual_machine
{

virtual_translator::virtual_translator(stream::output_stream & _output)
{
	this->_output = &_output;

	_temp_member_pos = _output.position();

	op_code::write_int_type<member_index_t, false>(_output, OC_SETUP, 0);

	_setup_end_pos = _output.position();
}

void virtual_translator::open_scope()
{
}

void virtual_translator::close_scope(member_index_t _variable_count)
{
}

void virtual_translator::finalize(member_index_t _temp_count, member_index_t _local_count)
{
	// Update first instruction
	if (_temp_count + _local_count) {
		// Update
		auto _current_pos = _output->position();

		_output->set_position(_temp_member_pos);
		op_code::write_int_type<member_index_t, false>(*_output, OC_SETUP, _temp_count + _local_count);
		_output->set_position(_current_pos);
	} // Skip temp member creation
	else {
		_output->set_beginning(_setup_end_pos);
	}

	op_code::write_p_type(*_output, OC_RETURN);
}

void virtual_translator::instantiate_int(const index & _member, int64_t _value)
{
	op_code::write_mi_type(*_output, OC_INSTANTIATE, _member, _value);
}

void virtual_translator::instantiate_double(const index & _member, double _value)
{
	op_code::write_mi_type(*_output, OC_INSTANTIATE, _member, _value);
}

void virtual_translator::instantiate_string(const index & _member, string_manager::utf8_index_t _value)
{
	op_code::write_mi_type(*_output, OC_INSTANTIATE, _member, _value);
}

void virtual_translator::instantiate_regex(const index & _member, schein::regex_index_t _regex)
{
	op_code::write_mint_type(*_output, OC_INSTANTIATE_REGEX, _member, _regex);
}

void virtual_translator::test(const index & _member)
{
	op_code::write_m_type(*_output, OC_TEST, _member);
}

void virtual_translator::execute(const index & _member, const index * _destination)
{
	if (_destination) {
		op_code::write_mm_type(*_output, OC_EXECUTE, _member, *_destination);
	} else {
		op_code::write_m_type(*_output, OC_EXECUTE_VOID, _member);
	}
}

void virtual_translator::execute_count(const index & _member, const index * _destination, framework::member::parameter_count_t _count)
{
	if (_destination) {
		op_code::write_mm_type(*_output, OC_EXECUTE_COUNT, _member, *_destination);
	} else {
		op_code::write_m_type(*_output, OC_EXECUTE_COUNT_VOID, _member);
	}

	_output->write_all(_count);
}

void virtual_translator::execute_format(const index & _member, const index * _destination, const char * _format, framework::member::parameter_count_t _count)
{
	if (_destination) {
		op_code::write_mm_type(*_output, OC_EXECUTE_FORMAT, _member, *_destination);
	} else {
		op_code::write_m_type(*_output, OC_EXECUTE_FORMAT_VOID, _member);
	}

	_output->write_all(_count);
	_output->write(_format, _count);
}

void virtual_translator::pass_parameter(const index & _member)
{
	op_code::write_m_type(*_output, OC_PUSH, _member);
}

void virtual_translator::pass_test()
{
	op_code::write_p_type(*_output, OC_PUSH_TEST);
}

void virtual_translator::clone(const index & _member, const index & _destination)
{
	op_code::write_mm_type(*_output, OC_CLONE, _member, _destination);
}

void virtual_translator::refer(const index & _member, const index & _destination)
{
	op_code::write_mm_type(*_output, OC_REFER, _member, _destination);
}

void virtual_translator::copyof(const index & _member, const index & _destination)
{
	op_code::write_mm_type(*_output, OC_COPY, _member, _destination);
}

void virtual_translator::operator_call(const index & _member, const index * _destination, framework::operator_t _operator, const index & _right)
{
	// With destination
	if (_destination) {
		op_code::write_mmm_type(*_output, OC_OPERATOR_CALL, _member, *_destination, _right);
	} else {
		op_code::write_mm_type(*_output, OC_OPERATOR_CALL_VOID, _member, _right);
	}

	_output->write_all(_operator);
}

void virtual_translator::operator_call_immediate(const index & _member, const index * _destination, framework::operator_t _operator, int64_t _value)
{
	// With destination
	if (_destination) {
		op_code::write_mmi_type(*_output, OC_OPERATOR_CALL_IMMEDIATE, _member, *_destination, _value);
	} else {
		op_code::write_mi_type(*_output, OC_OPERATOR_CALL_IMMEDIATE_VOID, _member, _value);
	}

	_output->write_all(_operator);
}

void virtual_translator::operator_call_immediate(const index & _member, const index * _destination, framework::operator_t _operator, double _value)
{
	// With destination
	if (_destination) {
		op_code::write_mmi_type(*_output, OC_OPERATOR_CALL_IMMEDIATE, _member, *_destination, _value);
	} else {
		op_code::write_mi_type(*_output, OC_OPERATOR_CALL_IMMEDIATE_VOID, _member, _value);
	}

	_output->write_all(_operator);
}

void virtual_translator::operator_call_immediate_reverse(const index & _member, const index * _destination, framework::operator_t _operator, int64_t _value)
{
	// With destination
	if (_destination) {
		op_code::write_mmi_type(*_output, OC_OPERATOR_CALL_IMMEDIATE_REVERSE, _member, *_destination, _value);
	} else {
		op_code::write_mi_type(*_output, OC_OPERATOR_CALL_IMMEDIATE_REVERSE_VOID, _member, _value);
	}

	_output->write_all(_operator);
}

void virtual_translator::operator_call_immediate_reverse(const index & _member, const index * _destination, framework::operator_t _operator, double _value)
{
	// With destination
	if (_destination) {
		op_code::write_mmi_type(*_output, OC_OPERATOR_CALL_IMMEDIATE_REVERSE, _member, *_destination, _value);
	} else {
		op_code::write_mi_type(*_output, OC_OPERATOR_CALL_IMMEDIATE_REVERSE_VOID, _member, _value);
	}

	_output->write_all(_operator);
}

void virtual_translator::test_call(const index & _member)
{
	op_code::write_m_type(*_output, OC_TEST, _member);
}

void virtual_translator::test_call(const index & _member, framework::operator_t _operator, const index & _right)
{
	op_code::write_mm_type(*_output, OC_TEST_MEMBER, _member, _right);

	_output->write_all(_operator);
}

void virtual_translator::test_call_immediate(const index & _member, framework::operator_t _operator, int64_t _value)
{
	op_code::write_mi_type(*_output, OC_TEST_IMMEDIATE, _member, _value);

	_output->write_all(_operator);
}

void virtual_translator::test_call_immediate(const index & _member, framework::operator_t _operator, double _value)
{
	op_code::write_mi_type(*_output, OC_TEST_IMMEDIATE, _member, _value);

	_output->write_all(_operator);
}

void virtual_translator::test_call_immediate_reverse(const index & _member, framework::operator_t _operator, int64_t _value)
{
	op_code::write_mi_type(*_output, OC_TEST_IMMEDIATE_REVERSE, _member, _value);

	_output->write_all(_operator);
}

void virtual_translator::test_call_immediate_reverse(const index & _member, framework::operator_t _operator, double _value)
{
	op_code::write_mi_type(*_output, OC_TEST_IMMEDIATE_REVERSE, _member, _value);

	_output->write_all(_operator);
}

void virtual_translator::object_member(const index & _member, const index & _destination, name_manager::name_t _name)
{
	op_code::write_mmint_type(*_output, OC_OBJECT_MEMBER, _member, _destination, _name_map.get_or_insert(_name));
}

virtual_translator::position_t virtual_translator::jump(JUMP _type, position_t _destination, position_t _overwrite_pos)
{
	int32_t _pos = _destination - op_code::jump_instruction_length<int32_t>();
	OP_CODE _operation;
	auto _current_pos = _output->position();

	// Position provided
	if (_overwrite_pos >= 0) {
		_output->set_position(_overwrite_pos);
		_pos -= _overwrite_pos;
	} else {
		_pos -= _current_pos;
	}

	switch (_type) {
	case JUMP::JUMP:
		_operation = OC_JUMP;

		break;
	case JUMP::JUMP_IF_TRUE:
		_operation = OC_JUMP_TRUE;

		break;
	case JUMP::JUMP_IF_FALSE:
		_operation = OC_JUMP_FALSE;

		break;
	default:
		BIA_IMPLEMENTATION_ERROR;
	}

	op_code::write_int_type<int32_t, false>(*_output, _operation, _pos);

	// Jump back
	if (_overwrite_pos >= 0) {
		_output->set_position(_current_pos);
	}

	return _current_pos;
}

stream::output_stream & virtual_translator::output_stream() noexcept
{
	return *_output;
}

const utility::index_map & virtual_translator::member_map() noexcept
{
	return _member_map;
}

const utility::index_map & virtual_translator::name_map() noexcept
{
	return _name_map;
}

member_index virtual_translator::to_member(grammar::report::member_t _name)
{
	return _member_map.get_or_insert(_name);
}

temp_index virtual_translator::to_temp(member_index_t _index) noexcept
{
	return _index - 1;
}

local_index virtual_translator::to_local(member_index_t _index) noexcept
{
	return _index - 1;
}

}
}
}