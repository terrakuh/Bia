#include "machine_context.hpp"
#include "simple_allocator.hpp"
#include "simple_executable_allocator.hpp"
#include "compiler.hpp"
#include "buffer_output_stream.hpp"
#include "buffer_input_stream.hpp"
#include "syntax.hpp"
#include "disassembler.hpp"
#include "static_function.hpp"

#include <chrono>
#include <iostream>
#include <regex>


template<typename _Lambda>
inline void test_and_time(int _count, _Lambda && _lambda)
{
	auto _time_taken = 0ll;

	for (auto k = 0; k < _count; ++k)
	{
		auto _start = std::chrono::high_resolution_clock::now();

		_lambda();

		_time_taken += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _start).count();
	}

	printf("Time taken: %f ms\n", _time_taken / static_cast<double>(_count));
}

using namespace bia;

void test()
{
	puts("hello world");
}

int main()
{
	{
		// Create context which handles almost everything
		auto _allocator = std::make_shared<machine::memory::simple_allocator>();
		auto _exec_allocator = std::make_shared<machine::memory::simple_executable_allocator>();
		bia::machine::machine_context _context(_allocator, _exec_allocator);
		/*_context.get_address_or_create("foo")->replace_this<framework::executable::static_function<void>>(&test);
		_context.get_address_or_create(_context.get_name_address("a"))->replace_this<framework::native::int_member>(1);
		_context.get_address_or_create(_context.get_name_address("a"))->refer(_context.get_address_or_create("b"));
		*_context.get_address_or_create("b")->cast<int64_t>() = 69;*/
		// Script
		char _script[] = R""(
var i = 34
global j = 34

print i == j

)"";

		// Compile
		bia::stream::buffer_input_stream _input(std::shared_ptr<const void>(_script, [](const void*) {}), sizeof(_script) - 1);
		bia::stream::buffer_output_stream _output;
		bia::compiler::compiler _compiler(_output, _context);

		test_and_time(1, [&]() {
			bia::grammar::syntax::get_interpreter().interpret(_input, _compiler, _context);
		});

		_compiler.finalize();

		// Disassemble
		bia::machine::disassembler _disassembler(&_context);

		_disassembler.disassemble(_output._buffer.get(), _output._size);

		system("pause");

		// Run
		//bia::machine::machine_code _machine_code = _compiler.get_code();
		bia::machine::machine_code _machine_code({ reinterpret_cast<const uint8_t*>(_output._buffer.get()), _output._size }, bia::machine::machine_schein(_context.get_allocator(), _context.get_executable_allocator()));

		if (_machine_code.is_executable()) {
			try {
				_machine_code.execute();
			} catch (const std::exception & e) {
				printf("%s: %s\n", typeid(e).name(), e.what());
			}
		}
	}

	system("pause");
}