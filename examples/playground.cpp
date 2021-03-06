#include <atomic>
#include <bia/bia.hpp>
#include <bia/exception/syntax_error.hpp>
#include <bia/member/function/generator.hpp>
#include <iostream>
#include <sstream>
#include <thread>
#include <typeinfo>

int main(int argc, char** argv)
{
	bia::engine engine;

	engine.use_bsl({ argv, argv + argc });

	std::atomic_bool b;
	auto t = std::thread{ [&] {
		while (!b) {
			std::this_thread::sleep_for(std::chrono::seconds{ 1 });
			engine.gc().run_once();
		}

		BIA_LOG(INFO, "exiting gc thread");
	} };
	auto f = bia::util::make_finally([&] {
		BIA_LOG(INFO, "waiting for gc to finish");

		b = true;

		t.join();
	});

	std::stringstream code;

	code << u8R"(

import io
import sys
import os

let x = "heyho"

fun foo {
	io.print("inside function")

	return x + " na wie gehts?"
}

io.print("return:", foo())
os.system("sleep 1")
io.print(sys.version)
return 33

)";

	try {
		const auto value = bia::member::cast::cast<int>(*engine.execute(code).peek());

		std::cout << "result: " << value << std::endl;
	} catch (const bia::exception::bia_error& e) {
		std::cout << "exception (" << e.name() << "; " << e.filename() << ":" << e.line() << "): " << e.what()
		          << "\n";

		if (dynamic_cast<const bia::exception::syntax_error*>(&e)) {
			const auto details = static_cast<const bia::exception::syntax_error&>(e).details();

			std::cout << details.message << ":" << details.position.operator std::streamoff() << "\n\n"
			          << code.str() << "\n";

			for (auto i = details.position.operator std::streamoff(); i--;) {
				std::cout.put(' ');
			}

			std::cout << "\e[0;32m^\e[0;30m\n";
		}

		return 1;
	}
}
