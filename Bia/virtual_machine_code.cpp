#include "virtual_machine_code.hpp"
#include "op_code.hpp"
#include "create_member.hpp"

#include <cstring>


namespace bia
{
namespace machine
{
namespace virtual_machine
{

virtual_machine_code::virtual_machine_code(memory::universal_allocation _code, virtual_machine_schein && _schein, bool _take_ownership) : _schein(std::move(_schein))
{
	// Copy buffer
	if (!_take_ownership) {
		auto _tmp = this->_schein.allocator()->allocate(_code.second);

		std::memcpy(_tmp.first, _code.first, _code.second);

		_code = _tmp;
	}

	this->_code = memory::cast_allocation<uint8_t>(_code);
}

virtual_machine_code::virtual_machine_code(virtual_machine_code && _move) : _schein(std::move(_move._schein))
{
	_code = std::move(_move._code);

	_move.clear();
}

virtual_machine_code::~virtual_machine_code()
{
	clear();
}

void virtual_machine_code::execute()
{
	auto & _globals = _schein.globals();
	auto & _stack = _schein.stack();
	const auto _end = _code.first + _code.second;
	const uint8_t * _cursor = _code.first;
	framework::member::test_result_t _test_register = 0;

	while (_cursor < _end) {
		auto _operation = read<op_code_t>(_cursor);

		/*********GENERATED BY 'generateVM.py'**********/
		switch (_operation) {
		/** P-Type */
		case (OC_RETURN):
		{
			goto gt_return;
			break;
		}
		case (OC_PUSH_TEST):
		{
			_stack.push(_test_register);
			break;
		}
		/** int-Type */
		case (OC_SETUP - IIOCO_INT32):
		{
			auto _int = read<int32_t>(_cursor);

			break;
		}
		case (OC_SETUP - IIOCO_INT8):
		{
			auto _int = read<int8_t>(_cursor);

			break;
		}
		case (OC_JUMP - IIOCO_INT32):
		{
			auto _int = read<int32_t>(_cursor);
			_cursor += _int;
			break;
		}
		case (OC_JUMP - IIOCO_INT8):
		{
			auto _int = read<int8_t>(_cursor);
			_cursor += _int;
			break;
		}
		case (OC_JUMP_TRUE - IIOCO_INT32):
		{
			auto _int = read<int32_t>(_cursor);
			_cursor += _test_register ? _int : 0;
			break;
		}
		case (OC_JUMP_TRUE - IIOCO_INT8):
		{
			auto _int = read<int8_t>(_cursor);
			_cursor += _test_register ? _int : 0;
			break;
		}
		case (OC_JUMP_FALSE - IIOCO_INT32):
		{
			auto _int = read<int32_t>(_cursor);
			_cursor += _test_register ? 0 : _int;
			break;
		}
		case (OC_JUMP_FALSE - IIOCO_INT8):
		{
			auto _int = read<int8_t>(_cursor);
			_cursor += _test_register ? 0 : _int;
			break;
		}
		/** I-Type */
		case (OC_PUSH_IMMEDIATE - IOCO_INT32):
		{
			auto _immediate = read<int32_t>(_cursor);
			_stack.push(_immediate);
			break;
		}
		case (OC_PUSH_IMMEDIATE - IOCO_INT8):
		{
			auto _immediate = read<int8_t>(_cursor);
			_stack.push(_immediate);
			break;
		}
		case (OC_PUSH_IMMEDIATE - IOCO_INT64):
		{
			auto _immediate = read<int64_t>(_cursor);
			_stack.push(_immediate);
			break;
		}
		case (OC_PUSH_IMMEDIATE - IOCO_FLOAT):
		{
			auto _immediate = read<double>(_cursor);
			_stack.push(_immediate);
			break;
		}
		/** M-Type */
		case (OC_TEST - MOCO_TINY_MEMBER):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			_test_register = _member->test();
			break;
		}
		case (OC_TEST - MOCO_MEMBER):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			_test_register = _member->test();
			break;
		}
		case (OC_PUSH - MOCO_TINY_MEMBER):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			_stack.push(_member);
			break;
		}
		case (OC_PUSH - MOCO_MEMBER):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			_stack.push(_member);
			break;
		}
		case (OC_UNDEFINE - MOCO_TINY_MEMBER):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			_member->undefine();
			break;
		}
		case (OC_UNDEFINE - MOCO_MEMBER):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			_member->undefine();
			break;
		}
		case (OC_EXECUTE_VOID - MOCO_TINY_MEMBER):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			_member->execute(nullptr);
			break;
		}
		case (OC_EXECUTE_VOID - MOCO_MEMBER):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			_member->execute(nullptr);
			break;
		}
		case (OC_EXECUTE_COUNT_VOID - MOCO_TINY_MEMBER):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			_member->execute_count(nullptr, nullptr, read<framework::member::parameter_count_t>(_cursor), &_stack);
			break;
		}
		case (OC_EXECUTE_COUNT_VOID - MOCO_MEMBER):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			_member->execute_count(nullptr, nullptr, read<framework::member::parameter_count_t>(_cursor), &_stack);
			break;
		}
		case (OC_EXECUTE_FORMAT_VOID - MOCO_TINY_MEMBER):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _parameter_count = read<framework::member::parameter_count_t>(_cursor);
			if (_cursor + _parameter_count > _end) {
				BIA_IMPLEMENTATION_ERROR;
			}
			_member->execute_format(nullptr, reinterpret_cast<const char*>(_cursor), _parameter_count, &_stack);
			_cursor += _parameter_count;
			break;
		}
		case (OC_EXECUTE_FORMAT_VOID - MOCO_MEMBER):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _parameter_count = read<framework::member::parameter_count_t>(_cursor);
			if (_cursor + _parameter_count > _end) {
				BIA_IMPLEMENTATION_ERROR;
			}
			_member->execute_format(nullptr, reinterpret_cast<const char*>(_cursor), _parameter_count, &_stack);
			_cursor += _parameter_count;
			break;
		}
		/** MM-Type */
		case (OC_EXECUTE - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->execute(_member1);
			break;
		}
		case (OC_EXECUTE - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->execute(_member1);
			break;
		}
		case (OC_EXECUTE - (MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->execute(_member1);
			break;
		}
		case (OC_EXECUTE - (MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->execute(_member1);
			break;
		}
		case (OC_EXECUTE_COUNT - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->execute_count(_member1, nullptr, read<framework::member::parameter_count_t>(_cursor), &_stack);
			break;
		}
		case (OC_EXECUTE_COUNT - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->execute_count(_member1, nullptr, read<framework::member::parameter_count_t>(_cursor), &_stack);
			break;
		}
		case (OC_EXECUTE_COUNT - (MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->execute_count(_member1, nullptr, read<framework::member::parameter_count_t>(_cursor), &_stack);
			break;
		}
		case (OC_EXECUTE_COUNT - (MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->execute_count(_member1, nullptr, read<framework::member::parameter_count_t>(_cursor), &_stack);
			break;
		}
		case (OC_EXECUTE_FORMAT - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _parameter_count = read<framework::member::parameter_count_t>(_cursor);
			if (_cursor + _parameter_count > _end) {
				BIA_IMPLEMENTATION_ERROR;
			}
			_member0->execute_format(_member1, reinterpret_cast<const char*>(_cursor), _parameter_count, &_stack);
			_cursor += _parameter_count;
			break;
		}
		case (OC_EXECUTE_FORMAT - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _parameter_count = read<framework::member::parameter_count_t>(_cursor);
			if (_cursor + _parameter_count > _end) {
				BIA_IMPLEMENTATION_ERROR;
			}
			_member0->execute_format(_member1, reinterpret_cast<const char*>(_cursor), _parameter_count, &_stack);
			_cursor += _parameter_count;
			break;
		}
		case (OC_EXECUTE_FORMAT - (MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _parameter_count = read<framework::member::parameter_count_t>(_cursor);
			if (_cursor + _parameter_count > _end) {
				BIA_IMPLEMENTATION_ERROR;
			}
			_member0->execute_format(_member1, reinterpret_cast<const char*>(_cursor), _parameter_count, &_stack);
			_cursor += _parameter_count;
			break;
		}
		case (OC_EXECUTE_FORMAT - (MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _parameter_count = read<framework::member::parameter_count_t>(_cursor);
			if (_cursor + _parameter_count > _end) {
				BIA_IMPLEMENTATION_ERROR;
			}
			_member0->execute_format(_member1, reinterpret_cast<const char*>(_cursor), _parameter_count, &_stack);
			_cursor += _parameter_count;
			break;
		}
		case (OC_CLONE - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->clone(_member1);
			break;
		}
		case (OC_CLONE - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->clone(_member1);
			break;
		}
		case (OC_CLONE - (MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->clone(_member1);
			break;
		}
		case (OC_CLONE - (MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->clone(_member1);
			break;
		}
		case (OC_REFER - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->refer(_member1);
			break;
		}
		case (OC_REFER - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->refer(_member1);
			break;
		}
		case (OC_REFER - (MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->refer(_member1);
			break;
		}
		case (OC_REFER - (MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->refer(_member1);
			break;
		}
		case (OC_COPY - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->copy(_member1);
			break;
		}
		case (OC_COPY - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->copy(_member1);
			break;
		}
		case (OC_COPY - (MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->copy(_member1);
			break;
		}
		case (OC_COPY - (MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_member0->copy(_member1);
			break;
		}
		case (OC_TEST_MEMBER - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_test_register = _member0->test_member(read<framework::operator_t>(_cursor), _member1);
			break;
		}
		case (OC_TEST_MEMBER - (MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_test_register = _member0->test_member(read<framework::operator_t>(_cursor), _member1);
			break;
		}
		case (OC_TEST_MEMBER - (MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			_test_register = _member0->test_member(read<framework::operator_t>(_cursor), _member1);
			break;
		}
		case (OC_TEST_MEMBER - (MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			_test_register = _member0->test_member(read<framework::operator_t>(_cursor), _member1);
			break;
		}
		/** MI-Type */
		case (OC_INSTANTIATE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			framework::create_member(_member, _immediate);
			break;
		}
		case (OC_INSTANTIATE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			framework::create_member(_member, _immediate);
			break;
		}
		case (OC_INSTANTIATE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			framework::create_member(_member, _immediate);
			break;
		}
		case (OC_INSTANTIATE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			framework::create_member(_member, _immediate);
			break;
		}
		case (OC_INSTANTIATE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			framework::create_member(_member, _immediate);
			break;
		}
		case (OC_INSTANTIATE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			framework::create_member(_member, _immediate);
			break;
		}
		case (OC_INSTANTIATE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			framework::create_member(_member, _immediate);
			break;
		}
		case (OC_INSTANTIATE - (MOCO_MEMBER * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			framework::create_member(_member, _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			_test_register = test(_member, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			_test_register = test(_member, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			_test_register = test(_member, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			_test_register = test(_member, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			_test_register = test(_member, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			_test_register = test(_member, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			_test_register = test(_member, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE - (MOCO_MEMBER * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			_test_register = test(_member, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_TEST_IMMEDIATE_REVERSE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_TEST_IMMEDIATE_REVERSE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_TEST_IMMEDIATE_REVERSE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_TEST_IMMEDIATE_REVERSE - (MOCO_TINY_MEMBER * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_TEST_IMMEDIATE_REVERSE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_TEST_IMMEDIATE_REVERSE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_TEST_IMMEDIATE_REVERSE - (MOCO_MEMBER * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_TEST_IMMEDIATE_REVERSE - (MOCO_MEMBER * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member2 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->operator_call(_member1, read<framework::operator_t>(_cursor), _member2);
			break;
		}
		case (OC_OPERATOR_CALL - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member2 = _globals[read<member_index_t>(_cursor)];
			_member0->operator_call(_member1, read<framework::operator_t>(_cursor), _member2);
			break;
		}
		case (OC_OPERATOR_CALL - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _member2 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->operator_call(_member1, read<framework::operator_t>(_cursor), _member2);
			break;
		}
		case (OC_OPERATOR_CALL - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _member2 = _globals[read<member_index_t>(_cursor)];
			_member0->operator_call(_member1, read<framework::operator_t>(_cursor), _member2);
			break;
		}
		case (OC_OPERATOR_CALL - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member2 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->operator_call(_member1, read<framework::operator_t>(_cursor), _member2);
			break;
		}
		case (OC_OPERATOR_CALL - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member2 = _globals[read<member_index_t>(_cursor)];
			_member0->operator_call(_member1, read<framework::operator_t>(_cursor), _member2);
			break;
		}
		case (OC_OPERATOR_CALL - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * MOCO_COUNT + MOCO_TINY_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _member2 = _globals[read<tiny_member_index_t>(_cursor)];
			_member0->operator_call(_member1, read<framework::operator_t>(_cursor), _member2);
			break;
		}
		case (OC_OPERATOR_CALL - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * MOCO_COUNT + MOCO_MEMBER)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _member2 = _globals[read<member_index_t>(_cursor)];
			_member0->operator_call(_member1, read<framework::operator_t>(_cursor), _member2);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			operator_call(_member0, _member1, read<framework::operator_t>(_cursor), _immediate);
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_TINY_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member0 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_TINY_MEMBER) * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<tiny_member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT32)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int32_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT8)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int8_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_INT64)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<int64_t>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		case (OC_OPERATOR_CALL_IMMEDIATE_REVERSE - ((MOCO_MEMBER * MOCO_COUNT + MOCO_MEMBER) * IOCO_COUNT + IOCO_FLOAT)):
		{
			auto _member0 = _globals[read<member_index_t>(_cursor)];
			auto _member1 = _globals[read<member_index_t>(_cursor)];
			auto _immediate = read<double>(_cursor);
			BIA_NOT_IMPLEMENTED;
			break;
		}
		default:
			BIA_IMPLEMENTATION_ERROR;
		}
		/*********GENERATED BY 'generateVM.py'**********/
	}

gt_return:;
}

void virtual_machine_code::clear()
{
	if (_code) {
		_schein.allocator()->deallocate(memory::cast_allocation<void>(_code));
		_code.clear();
	}
}

bool virtual_machine_code::is_executable() const noexcept
{
	return _code;
}

}
}
}