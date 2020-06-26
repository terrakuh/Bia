#include "identifier.hpp"

#include <bia/string/encoding/unicode.hpp>
#include <bia/util/finally.hpp>

struct comparator
{
	const char** first;
	const char** last;
	std::size_t index = 0;

	comparator(const char** first, const char** last) noexcept
	{
		this->first = first;
		this->last  = last;
	}
	void next(bia::string::encoding::code_point_type cp) noexcept
	{
		if (first == last || cp < 'a' || cp > 'z') {
			first = last = nullptr;

			return;
		}

		char c = static_cast<char>(cp);

		++index;

		for (auto i = first; i != last; ++i) {
			if (**i && **i == c) {
				first = i;

				break;
			}
		}

		for (auto i = first; i != last; ++i) {
			if (**i) {
				if (**i != c) {
					last = i;

					break;
				}

				++*i;
			}
		}
	}
	const char** result() const noexcept
	{
		if (first != last && !**first) {
			return first;
		}

		return nullptr;
	}
};

bia::exception::syntax_details bia::tokenizer::token::parse::identifier(parameter& tp)
{
	using namespace string::encoding;

	const char* builtins[] = { "list" };
	auto first             = true;
	auto streambuf         = tp.manager.start_memory(true);
	const auto outenc      = get_encoder(standard_encoding::utf_8);
	const auto free        = util::make_finally([outenc] { free_encoder(outenc); });
	comparator builtin_comparator{ +builtins, builtins + sizeof(builtins) / sizeof(const char*) };
	std::ostream output{ &streambuf };

	while (true) {
		const auto pos = tp.input.tellg();
		const auto cp  = tp.encoder.read(tp.input);

		switch (category_of(cp)) {
		case category::Ll:
		case category::Lu:
		case category::Lt:
		case category::Lo:
		case category::Pc:
		case category::Nl: first = false;
		case category::Nd:
		case category::No: {
			// add to output
			if (!first) {
				outenc->put(output, cp);
				builtin_comparator.next(cp);

				break;
			}
		}
		default: {
			// invalid char -> reset
			tp.input.seekg(pos);

			// valid identifier
			if (!first) {
				if (const auto builtin = builtin_comparator.result()) {
					tp.bundle.add(token{ static_cast<bytecode::member::builtin>(builtins - builtin) });
				} else {
					// zero terminate
					outenc->put(output, 0);
					tp.bundle.add(token{ token::identifier{ streambuf.finish(resource::type::string) } });
				}

				return {};
			}

			// not an identifier
			return { pos, "invalid identifier" };
		}
		}
	}
}
