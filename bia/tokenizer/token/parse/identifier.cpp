#include "tokens.hpp"

#include <bia/string/encoding/unicode.hpp>
#include <bia/util/finally.hpp>

using namespace bia::tokenizer::token;

struct comparator
{
	const char** first;
	const char** last;

	comparator(const char** first, const char** last) noexcept
	{
		this->first = first;
		this->last  = last;
	}
	void next(bia::string::encoding::code_point_type cp) noexcept
	{
		if (first == last || cp < 'a' || cp > 'z') {
			return;
		}

		// find beginning
		const auto c = static_cast<char>(cp);
		for (; first != last; ++first) {
			if (**first == c) {
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

error_info parse::identifier(parameter& param)
{
	using namespace string::encoding;
	const char* values[]{ "as",   "break",  "continue", "defer", "drop",   "else", "error", "false",
		                    "for",  "from",   "fun",      "if",    "import", "in",   "let",   "mut",
		                    "null", "return", "scoped",   "true",  "type",   "use",  "yield" };
	comparator keywords{ values, values + sizeof(values) / sizeof(const char*) };
	auto first        = true;
	auto streambuf    = param.manager.start_memory(true);
	const auto outenc = get_encoder(standard_encoding::utf_8);
	const auto free   = util::make_finally([outenc] { free_encoder(outenc); });
	std::ostream output{ &streambuf };

	while (true) {
		const auto pos = param.input.tellg();
		const auto cp  = param.encoder.read(param.input);
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
				keywords.next(cp);
				break;
			}
			BIA_ATTR_FALLTHROUGH;
		}
		default: {
			// valid identifier
			if (!first && !keywords.result()) {
				param.input.seekg(pos);
				// zero terminate
				outenc->put(output, 0);
				param.bundle.emplace_back(token::identifier{ streambuf.finish(resource::type::string) });
				return {};
			}
			// not an identifier
			return param.make_error(error::code::bad_identifier, -1);
		}
		}
	}
}
