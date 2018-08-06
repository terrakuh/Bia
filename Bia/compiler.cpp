#include "compiler.hpp"
#include "link.hpp"
#include "compile_normal_operation.hpp"
#include "buffer_output_stream.hpp"
#include "string_stream.hpp"

#include <vector>


namespace bia
{
namespace compiler
{

using namespace bia::grammar;

compiler::compiler(stream::output_stream & _output, machine::machine_context & _context) : _toolset(_output, &_context), _context(_context)
{
}

void compiler::report(const grammar::report * _begin, const grammar::report * _end)
{
	puts("global scope");
	handle_root(_begin);
}

void compiler::finalize()
{
	_toolset.finalize(_counter.max());
}

void compiler::test_compiler_value()
{
	using VT = compiler_value::VALUE_TYPE;

	switch (_value.type()) {
	case VT::INT:
		_value.set_return(_value.value().rt_int != 0);

		break;
	case VT::DOUBLE:
		_value.set_return(_value.value().rt_double != 0.);

		break;
	case VT::STRING:
		_value.set_return(_value.value().rt_string.length != 0);

		break;
	case VT::MEMBER:
		_toolset.call(&framework::member::test, _value.value().rt_member);
		_value.set_return_test();
		_toolset.write_test();

		break;
	case VT::TEMPORARY_MEMBER:
		_toolset.call(&framework::member::test, machine::platform::toolset::to_temp_member(_value.value().rt_temp_member));
		_value.set_return_test();
		_toolset.write_test();

		break;
	case VT::RESULT_REGISTER:
		_toolset.call(&framework::member::test, machine::platform::toolset::result_value());
		_value.set_return_test();
		_toolset.write_test();

		break;
	case VT::TEST_VALUE_REGISTER:
	case VT::TEST_VALUE_CONSTANT:
		break;
	default:
		BIA_IMPLEMENTATION_ERROR;
	}
}

const grammar::report * compiler::handle_value_expression(const grammar::report * _report)
{
	using JUMP = machine::platform::toolset::JUMP;

	const auto _end = _report->content.end;
	auto _test = false;
	///TODO
	do {
		// Handle expression
		_report = handle_condition_expression(_report + 1) + 1;

		// Create test result
		if (_test || _report < _end) {
			test_compiler_value();
		}

		_test = true;
	} while (_report < _end);

	return _report->content.end;
}

const grammar::report * compiler::handle_root(const grammar::report * _report)
{
	switch (_report->rule_id) {
	case BGR_ROOT_HELPER_0:
	{
		puts("open scope");
		auto _end = _report->content.end;

		++_report;

		// Handle all reports
		while (_report < _end) {
			_report = handle_root(_report);
		}

		return _end;
	}
	case BGR_VARIABLE_DECLARATION:
		return handle_variable_declaration(_report);
	case BGR_IF:
		return handle_if(_report);
	case BGR_PRINT:
		return handle_print(_report);
	case BGR_TEST_LOOP:
		return handle_test_loop(_report);
	//case BGR_IMPORT:
		//return HandleImport(p_pReport->content.children);
	case BGR_VALUE:
		return handle_value<false>(_report, [] {});
	default:
		BIA_IMPLEMENTATION_ERROR;
	}
}

const grammar::report * compiler::handle_root_ignore(const grammar::report * _report)
{
	switch (_report->rule_id) {
	case BGR_ROOT_HELPER_0:
	{
		auto _end = _report->content.end;

		++_report;

		// Handle all reports
		while (_report < _end) {
			_report = handle_root_ignore(_report);
		}

		return _end;
	}
	case BGR_VARIABLE_DECLARATION:
	case BGR_IF:
	case BGR_PRINT:
	case BGR_TEST_LOOP:
	case BGR_IMPORT:
	case BGR_VALUE:
		break;
	default:
		BIA_IMPLEMENTATION_ERROR;
	}

	return _report->content.end;
}

const grammar::report * compiler::handle_math_expression_and_term_inner(const grammar::report * _report, handle_function _next)
{
	// Handle leftmost math term
	auto _old_counter = _counter.peek();
	auto i = (this->*_next)(_report + 1);
	auto _left = _value;

	// Pop if not used
	if (_value.type() != compiler_value::VALUE_TYPE::TEMPORARY_MEMBER) {
		_counter.pop(_old_counter);
	}

	do {
		// Operator
		auto _operator = i->content.operator_code;

		// Handle right math term
		i = (this->*_next)(i + 1);

		// Pop if not used and set destination
		auto _right = _value;

		// Get the lower one
		if (_left.type() == compiler_value::VALUE_TYPE::TEMPORARY_MEMBER && _right.type() == compiler_value::VALUE_TYPE::TEMPORARY_MEMBER) {
			_value.set_return_temp(_left.value().rt_temp_member < _right.value().rt_temp_member ? _left.value().rt_temp_member : _right.value().rt_temp_member);
		} else if (_left.type() == compiler_value::VALUE_TYPE::TEMPORARY_MEMBER) {
			_value.set_return_temp(_left.value().rt_temp_member);
		} else if (_right.type() == compiler_value::VALUE_TYPE::TEMPORARY_MEMBER) {
			_value.set_return_temp(_right.value().rt_temp_member);
		} else {
			//_counter.pop(_old_counter);
			_value.set_return_temp(_counter.next());
		}

		// Call operator
		compile_normal_operation(_toolset, _value).operate(_left, _operator, _right);

		// Update if used
		if (_value.type() == compiler_value::VALUE_TYPE::TEMPORARY_MEMBER) {
			_old_counter = _value.value().rt_temp_member;

			_counter.update(_old_counter);
		}

		_left = _value;

		// Pop back
		_counter.pop(_old_counter);
	} while (i < _report->content.end);

	return _report->content.end;
}

const grammar::report * compiler::handle_condition_expression(const grammar::report * _report)
{
	// Handle left value
	auto _operator = handle_math_expression_and_term(_report + 1);

	// Operator is present
	if (_operator < _report->content.end) {
		auto _left = _value;

		// Handle right value
		handle_math_expression_and_term(_operator + 1);

		// Call operator
		///TODO: destination?
		compile_compare_operation(_toolset, _value).operate(_left, _operator->content.operator_code, _value);
	}

	return _report->content.end;
}

const grammar::report *  compiler::handle_number(const grammar::report * _report)
{
	using TYPE = report::TYPE;

	switch (_report->type) {
	case TYPE::INT_VALUE:
		_value.set_return(_report->content.int_value);

		break;
	case TYPE::DOUBLE_VALUE:
		_value.set_return(_report->content.double_value);

		break;
	default:
		BIA_IMPLEMENTATION_ERROR;
	}

	return _report;
}

const grammar::report * compiler::handle_raw_value(const grammar::report * _report)
{
	switch (_report[1].token_id) {
	case BV_NUMBER:
		handle_number(_report + 1);

		break;
	case BV_TRUE:
		_value.set_return(true);

		break;
	case BV_FALSE:
		_value.set_return(false);

		break;
	case BV_MEMBER:
		handle_member(_report + 1);

		break;
	default:
		BIA_IMPLEMENTATION_ERROR;
	}

	return _report->content.end;
}

const grammar::report * compiler::handle_identifier(const grammar::report * _report)
{
	// Global member
	_value.set_return(_context.address_of_member(_report->content.member));

	return _report + 1;
}

const grammar::report * compiler::handle_math_factor(const grammar::report * _report)
{
	switch (_report[1].rule_id) {
	case BGR_VALUE_RAW:
		handle_raw_value(_report + 1);

		break;
	case BGR_VALUE:
		handle_value_insecure<false>(_report + 1);

		break;
	default:
		BIA_IMPLEMENTATION_ERROR;
	}

	return _report->content.end;
}

const grammar::report * compiler::handle_member(const grammar::report * _report)
{
	const auto _end = _report->content.end;

	++_report;

	// First member
	if (_report->type == report::TYPE::STRING) {
		_report = handle_string(_report);
	} else if (_report->type == report::TYPE::MEMBER) {
		void(framework::member::*_function)(framework::member*) = nullptr;

		// Ref of
		if (_report->custom_parameter == keyword_refof::string_id()) {
			_function = &framework::member::refer;
		} // Copy of
		else if (_report->custom_parameter) {
			_function = &framework::member::copy;
		}

		_report = handle_identifier(_report);

		// Get refof/copyof
		if (_function) {
			_toolset.call(_function, _value.value().rt_member, machine::platform::toolset::to_temp_member(_counter.next()));
			_value.set_return_temp(_counter.current());
		}
	} else {
		BIA_IMPLEMENTATION_ERROR;
	}

	// Handle rest
	while (_report < _end) {
		// Handle parameter
		if (_report->rule_id == BGR_PARAMETER || _report->rule_id == BGR_PARAMETER_ITEM_ACCESS) {
			_report = handle_parameter(_report);
		} // Get member
		else if (_report->type == report::TYPE::MEMBER) {
			switch (_value.type()) {
			case compiler_value::VALUE_TYPE::MEMBER:
				_toolset.call(&framework::member::object_member, _value.value().rt_member, _report->content.member);

				break;
			case compiler_value::VALUE_TYPE::TEMPORARY_MEMBER:
				_toolset.call(&framework::member::object_member, machine::platform::toolset::to_temp_member(_value.value().rt_temp_member), _report->content.member);

				break;
			case compiler_value::VALUE_TYPE::RESULT_REGISTER:
				_toolset.call(&framework::member::object_member, machine::platform::toolset::result_value(), _report->content.member);

				break;
			default:
				BIA_IMPLEMENTATION_ERROR;
			}

			_value.set_return_result();
			++_report;
		}
	}

	return _end;
}

const grammar::report * compiler::handle_parameter(const grammar::report * _report)
{
	using VT = compiler_value::VALUE_TYPE;

	auto _caller = _value;
	machine::platform::toolset::pass_count _passed = 0;
	uint32_t _count = 0;
	std::string _format;
	auto _mixed = false;

	if (_report->type != grammar::report::TYPE::EMPTY_CHILD) {
		// Save member before it gets lost
		if (_caller.type() == VT::RESULT_REGISTER) {
			_toolset.save_result_value();
		}

		// Save old counter
		auto _old = _counter.peek();

		for (auto i = _report + 1; i < _report->content.end; ++_count) {
			char _type;

			i = handle_value_insecure<false>(i);

			switch (_value.type()) {
			case VT::TEST_VALUE_CONSTANT:
				_value.set_return(static_cast<int64_t>(_value.value().rt_test_result));
			case VT::INT:
			{
				if (_value.is_int32()) {
					_passed += _toolset.pass_varg(static_cast<int32_t>(_value.value().rt_int));
					_type = 'i';
				} else {
					_passed += _toolset.pass_varg(_value.value().rt_int);
					_type = 'I';
				}

				break;
			}
			case VT::DOUBLE:
				_passed += _toolset.pass_varg(_value.value().rt_double);
				_type = 'd';

				break;
			case VT::STRING:
				_passed += _toolset.pass_varg(_value.value().rt_string.data);
				_type = 'a';

				break;
			case VT::STRING16:
				_passed += _toolset.pass_varg(_value.value().rt_string.data);
				_type = 'u';

				break;
			case VT::STRING32:
				_passed += _toolset.pass_varg(_value.value().rt_string.data);
				_type = 'U';

				break;
			case VT::WSTRING:
				_passed += _toolset.pass_varg(_value.value().rt_string.data);
				_type = 'w';

				break;
			case VT::MEMBER:
				_passed += _toolset.pass_varg(_value.value().rt_member);
				_type = 'M';

				break;
			case VT::TEMPORARY_MEMBER:
				_passed += _toolset.pass_varg(machine::platform::toolset::to_temp_member(_value.value().rt_temp_member));
				_type = 'M';

				break;
			case VT::TEST_VALUE_REGISTER:
				_passed += _toolset.pass_varg(machine::platform::toolset::test_result_value());
				_type = 'i';

				break;
			default:
				BIA_IMPLEMENTATION_ERROR;
			}

			// Add type to format
			_format += _type;

			if (_type != 'M') {
				_mixed = true;
			}
		}

		_counter.pop(_old);
	}

	// Call function
	switch (_caller.type()) {
	case VT::MEMBER:
		handle_parameter_execute(_caller.value().rt_member, _format, _mixed, _count, _passed);

		break;
	case VT::TEMPORARY_MEMBER:
		handle_parameter_execute(machine::platform::toolset::to_temp_member(_caller.value().rt_temp_member), _format, _mixed, _count, _passed);

		break;
	case VT::RESULT_REGISTER:
		handle_parameter_execute(machine::platform::toolset::result_value(), _format, _mixed, _count, _passed);

		break;
	default:
		BIA_IMPLEMENTATION_ERROR;
	}

	return _report->content.end;
}

const grammar::report * compiler::handle_instantiation(const grammar::report * _report)
{
	// Set identifier
	//_value.set_return(_report[1].content.member);

	// Handle parameters

	// Instantiate

	return _report->content.end;
}

const grammar::report * compiler::handle_string(const grammar::report * _report)
{
	using S = stream::string_stream;

	auto _buffer = _report->content.string;

	switch (_report->custom_parameter) {
	case report::ASCII:
	case report::UTF8:
		_value.set_return(S::string<char>(_buffer), S::size(_buffer), S::length(_buffer));

		break;
	case report::UTF16:
	case report::UTF32:
	default:
		BIA_IMPLEMENTATION_ERROR;
	}

	return _report + 1;
}

const grammar::report * compiler::handle_variable_declaration(const grammar::report * _report)
{
	using T = machine::platform::toolset;
	using VT = compiler_value::VALUE_TYPE;

	// Handle value and prepare the result for a function call
	handle_value<false>(_report + 3, [&] {
		auto _expression = _value;

		handle_identifier(_report + 2);

		auto _destination = _value.value().rt_member;

		// Make call
		switch (_expression.type()) {
		case VT::TEST_VALUE_CONSTANT:
			_expression.set_return(static_cast<int64_t>(_expression.value().rt_test_result));
		case VT::INT:
		{
			// Optimize common used constant values
			switch (_expression.value().rt_int) {
			case 0:
				_toolset.call(&machine::link::instantiate_int_0, _destination);

				break;
			case 1:
				_toolset.call(&machine::link::instantiate_int_1, _destination);

				break;
			case -1:
				_toolset.call(&machine::link::instantiate_int_n1, _destination);

				break;
			default:
			{
				// Can be int32
				if (_expression.is_int32()) {
					_toolset.call(&machine::link::instantiate_int32, static_cast<int32_t>(_expression.value().rt_int), _destination);
				} else {
					_toolset.call(&machine::link::instantiate_int64, _expression.value().rt_int, _destination);
				}

				break;
			}
			}

			break;
		}
		case VT::DOUBLE:
			_toolset.call(&machine::link::instantiate_double, _expression.value().rt_double, _destination);

			break;
		case VT::STRING:
			_toolset.call(&machine::link::instantiate_string, _expression.value().rt_string.data, _expression.value().rt_string.size, _expression.value().rt_string.length, _destination);

			break;
		case VT::MEMBER:
			_toolset.call(&framework::member::clone, _expression.value().rt_member, _destination);

			break;
		case VT::TEMPORARY_MEMBER:
			_toolset.call(&framework::member::clone, T::to_temp_member(_expression.value().rt_temp_member), _destination);

			break;
		case VT::RESULT_REGISTER:
			_toolset.call(&framework::member::clone, T::result_value(), _destination);

			break;
		case VT::TEST_VALUE_REGISTER:
			_toolset.call(&machine::link::instantiate_int32, T::test_result_value(), _destination);

			break;
		default:
			BIA_IMPLEMENTATION_ERROR;
		}
	});

	return _report->content.end;
}

const grammar::report * compiler::handle_if(const grammar::report * _report)
{
	std::vector<machine::platform::toolset::position> _end_jumps;

	for (auto i = _report + 1; i < _report->content.end;) {
		// Else statement
		if (i->rule_id == grammar::BGR_IF_HELPER_1) {
			// Set the results for compiling
			_value.set_return(true);

			// Move because the else statement is wrapped up
			++i;
		} // Handle condition
		else {
			i = handle_value<true>(i, []() {});
		}

		// Compile statement
		if (_value.type() != compiler_value::VALUE_TYPE::TEST_VALUE_CONSTANT || _value.value().rt_test_result) {
			// Write jump
			auto _constant = _value.type() == compiler_value::VALUE_TYPE::TEST_VALUE_CONSTANT;
			auto _jump = _constant ? 0 : _toolset.jump(machine::platform::toolset::JUMP::JUMP_IF_FALSE);

			// Compile statement
			i = handle_root(i);

			// Jump to end if it has not been reached
			if (i < _report->content.end && !_constant) {
				_end_jumps.push_back(_toolset.jump(machine::platform::toolset::JUMP::JUMP));

				// Update test jump to next statement
				_toolset.jump(machine::platform::toolset::JUMP::JUMP_IF_FALSE, _toolset.output_stream().position(), _jump);
			} // End of ifs or condition is at compile time true
			else {
				// Update test jump to ent
				if (!_constant) {
					_toolset.jump(machine::platform::toolset::JUMP::JUMP_IF_FALSE, _toolset.output_stream().position(), _jump);
				}

				break;
			}
		} // Skip statements. These are conditions that are at compile time false
		else {
			i = handle_root_ignore(i);
		}
	}

	// Update end jump locations
	auto _end = _toolset.output_stream().position();

	for (auto & _jump : _end_jumps) {
		_toolset.jump(machine::platform::toolset::JUMP::JUMP, _end, _jump);
	}

	return _report->content.end;
}

const grammar::report * compiler::handle_print(const grammar::report * _report)
{
	using VT = compiler_value::VALUE_TYPE;

	// Handle value to print
	handle_value<false>(_report + 1, [this] {
		// Call print function
		switch (_value.type()) {
		case VT::INT:
		{
			// Can be int32
			if (_value.is_int32()) {
				_toolset.call(&machine::link::print_int32, static_cast<int32_t>(_value.value().rt_int));
			} else {
				_toolset.call(&machine::link::print_int64, _value.value().rt_int);
			}

			break;
		}
		case VT::DOUBLE:
			_toolset.call(&machine::link::print_double, _value.value().rt_double);

			break;
		case VT::STRING:
			_toolset.call(&machine::link::print_string, _value.value().rt_string.data);

			break;
		case VT::MEMBER:
			_toolset.call(&framework::member::print, _value.value().rt_member);

			break;
		case VT::TEMPORARY_MEMBER:
			_toolset.call(&framework::member::print, machine::platform::toolset::to_temp_member(_value.value().rt_temp_member));

			break;
		case VT::RESULT_REGISTER:
			_toolset.call(&framework::member::print, machine::platform::toolset::result_value());

			break;
		case VT::TEST_VALUE_REGISTER:
			_toolset.call(&machine::link::print_bool, machine::platform::toolset::test_result_value());

			break;
		case VT::TEST_VALUE_CONSTANT:
		{
			if (_value.value().rt_test_result) {
				_toolset.call(&machine::link::print_true);
			} else {
				_toolset.call(&machine::link::print_false);
			}

			break;
		}
		default:
			BIA_IMPLEMENTATION_ERROR;
		}
	});

	return _report->content.end;
}

const grammar::report * compiler::handle_test_loop(const grammar::report * _report)
{
	using namespace machine::platform;

	const auto _end = _report->content.end;

	++_report;

	// Loop type
	toolset::position _pre_test_jump;

	// Post test
	if (_report->content.keyword == grammar::IS_DO) {
		_pre_test_jump = -1;
		++_report;
	}  // Pre test
	else {
		_pre_test_jump = _toolset.jump(toolset::JUMP::JUMP);
	}

	// Loop jump type
	auto _jump_type = _report++->content.keyword == grammar::IS_WHILE ? toolset::JUMP::JUMP_IF_TRUE : toolset::JUMP::JUMP_IF_FALSE;

	// Handle loop condition
	toolset::position _condition_jump = -1;
	stream::buffer_output_stream _condition_buffer;
	auto & _orig_buffer = _toolset.output_stream();
	auto _compile = true;

	// Redirect conditional code to a temporary buffer
	_toolset.set_output(_condition_buffer);

	// Loop condition
	_report = handle_value<true>(_report, [&] {
		// Constant condition
		if (_value.type() == compiler_value::VALUE_TYPE::TEST_VALUE_CONSTANT) {
			// Don't compile this loop
			if (!_value.value().rt_test_result) {
				_compile = false;

				return;
			}

			// Change to unconditional jump
			_jump_type = toolset::JUMP::JUMP;
		} // Not a test register
		else if (_value.type() != compiler_value::VALUE_TYPE::TEST_VALUE_REGISTER) {
			BIA_IMPLEMENTATION_ERROR;
		}

		_condition_jump = _toolset.jump(_jump_type);
	});

	// Reset output buffer
	_toolset.set_output(_orig_buffer);

	// Compile loop
	if (_compile) {
		auto _start_pos = _orig_buffer.position();

		handle_root(_report);

		// Update jump positions
		auto _end_pos = _orig_buffer.position();

		_condition_jump += _end_pos;

		// Write loop condition
		_orig_buffer.append_stream(_condition_buffer);

		// Upate jump offset for loop jump
		_toolset.jump(_jump_type, _start_pos, _condition_jump);

		// Upate jump offset for pre test
		if (_pre_test_jump != -1) {
			_toolset.jump(toolset::JUMP::JUMP, _end_pos, _pre_test_jump);
		}
	} // Discard pre test jump
	else if (_pre_test_jump != -1) {
		_toolset.output_stream().set_position(_pre_test_jump);
	}

	return _end;
}

}
}