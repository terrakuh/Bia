#pragma once

#include <cstdint>
#include <vector>

#include "config.hpp"
#include "allocator.hpp"
#include "member.hpp"
#include "share.hpp"
#include "machine_context.hpp"
#include "virtual_machine_schein.hpp"
#include "code.hpp"
#include "operation.hpp"


namespace bia
{
namespace machine
{
namespace virtual_machine
{

class virtual_machine_code : public code
{
public:
	BIA_EXPORT virtual_machine_code(memory::universal_allocation _code, virtual_machine_schein && _schein, bool _take_ownership = false);
	virtual_machine_code(const virtual_machine_code & _copy) = delete;
	BIA_EXPORT virtual_machine_code(virtual_machine_code && _move);
	/**
	 * Destructor.
	 *
	 * @since 3.72.149.810
	 * @date 18-Jan-19
	 *
	 * @throws See clear().
	*/
	BIA_EXPORT ~virtual_machine_code();

	BIA_EXPORT virtual void execute() override;
	BIA_EXPORT virtual void clear() override;
	BIA_EXPORT virtual bool is_executable() const noexcept override;

private:
	/** Contains the instructions for the virtual machine code. */
	memory::allocation<uint8_t> _code;
	/** The machine schein.  */
	virtual_machine_schein _schein;

	static void operator_call(framework::member * _member, framework::member * _destination, framework::operator_t _operator, int32_t _immediate)
	{
		_member->operator_call_int32(_destination, _operator, _immediate);
	}
	static void operator_call(framework::member * _member, framework::member * _destination, framework::operator_t _operator, int64_t _immediate)
	{
		_member->operator_call_int64(_destination, _operator, _immediate);
	}
	static void operator_call(framework::member * _member, framework::member * _destination, framework::operator_t _operator, double _immediate)
	{
		_member->operator_call_double(_destination, _operator, _immediate);
	}
	static void operator_call_reverse(framework::member * _member, framework::member * _destination, framework::operator_t _operator, int32_t _immediate)
	{
		link::operation_int32(_destination, _operator, _member, _immediate);
	}
	static void operator_call_reverse(framework::member * _member, framework::member * _destination, framework::operator_t _operator, int64_t _immediate)
	{
		link::operation_int64(_destination, _operator, _member, _immediate);
	}
	static void operator_call_reverse(framework::member * _member, framework::member * _destination, framework::operator_t _operator, double _immediate)
	{
		link::operation_double(_destination, _operator, _member, _immediate);
	}
	template<typename Type>
	static Type read(const uint8_t *& _cursor)
	{
		auto _tmp = *reinterpret_cast<const Type*>(_cursor);

		_cursor += sizeof(Type);

		return _tmp;
	}
	static framework::member::test_result_t test(framework::member * _member, framework::operator_t _operator, int32_t _immediate)
	{
		return _member->test_int32(_operator, _immediate);
	}
	static framework::member::test_result_t test(framework::member * _member, framework::operator_t _operator, int64_t _immediate)
	{
		return _member->test_int64(_operator, _immediate);
	}
	static framework::member::test_result_t test(framework::member * _member, framework::operator_t _operator, double _immediate)
	{
		return _member->test_double(_operator, _immediate);
	}
	static framework::member::test_result_t test_reverse(framework::member * _member, framework::operator_t _operator, int32_t _immediate)
	{
		return link::compare_operation_int32(_operator, _member, _immediate);
	}
	static framework::member::test_result_t test_reverse(framework::member * _member, framework::operator_t _operator, int64_t _immediate)
	{
		return link::compare_operation_int64(_operator, _member, _immediate);
	}
	static framework::member::test_result_t test_reverse(framework::member * _member, framework::operator_t _operator, double _immediate)
	{
		return link::compare_operation_double(_operator, _member, _immediate);
	}
};

}
}
}