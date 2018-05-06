#pragma once

#include <cstring>
#include <cstdint>
#include <regex>
#include <type_traits>

#include "config.hpp"
#include "interpreter_rule.hpp"
#include "interpreter_id.hpp"
#include "input_stream.hpp"
#include "utf.hpp"


namespace bia
{
namespace grammar
{

enum STRING_CUSTOM_PARAMETER_FLAGS : uint64_t
{
	SCPF_RAW_STRING = 1ull << 63,
	SCPF_MASK = 0xffffffffull
};

template<size_t _Size>
inline bool fast_find(char _what, const char * _sorted_string) noexcept
{
	for (auto i = _sorted_string + _Size; _sorted_string < i; ++_sorted_string) {
		if (_what == *_sorted_string) {
			return true;
		}
	}

	return false;
}

inline int determine_base(const char *& _buffer, size_t & _length) noexcept
{
	if (*_buffer == '0') {
		if (_length > 1) {
			_buffer += 2;
			_length -= 2;

			// Binary
			if (_buffer[1] == 'b') {
				return 2;
			}
			// Hexadecimal
			else if (_buffer[1] == 'x') {
				return 16;
			}
			// Octal
			else if (_buffer[1] == 'o') {
				return 8;
			}
		}
	}

	return 10;
}



template<flag_type _Flags, int _For_clarity = 0>
inline ACTION StringValueToken(const char * p_pcBuffer, size_t p_iSize, token_param, token_output & p_output)
{
	constexpr auto SUCCESS = _Flags & FILLER_TOKEN ? (_Flags & LOOPING_TOKEN ? ACTION::DONT_REPORT_AND_LOOP : ACTION::DONT_REPORT) : (_Flags & LOOPING_TOKEN ? ACTION::REPORT_AND_LOOP : ACTION::REPORT);
	constexpr auto ERROR = _Flags & OPTIONAL_TOKEN ? ACTION::DONT_REPORT : (_Flags & LOOPING_TOKEN ? ACTION::DONT_REPORT : ACTION::ERROR);

	enum FLAGS : uint32_t
	{
		F_SINGLE_QUOTE = 0x1,	/**	If set ', otherwise ".	*/
		F_QUOTE_SET = 0x2,
		F_RAW_STRING = 0x4,
	};

	auto pcDelimitor = "";
	size_t iDelimitor = 0;
	uint32_t fFlags = 0;

	//Starting whitespaces
	if (!whitespace_deleter<_Flags, true>(p_pcBuffer, p_iSize, p_output))
		return ERROR;

	//Determine delimitor and quote
	while (p_iSize--)
	{
		++p_output.iBufferOffset;

		switch (*p_pcBuffer++)
		{
		case 'R':
			//Add 'R' to delimitor
			if (fFlags & F_QUOTE_SET)
				++iDelimitor;
			else if (!(fFlags & F_RAW_STRING))
				fFlags |= F_RAW_STRING;
			else
				return ERROR;

			break;
		case '"': //Double quote
			//No quote set yet
			if (!(fFlags & F_QUOTE_SET))
			{
				fFlags |= F_QUOTE_SET;

				//Terminate
				if (!(fFlags & F_RAW_STRING))
					goto gt_break;
				else
					pcDelimitor = p_pcBuffer;
			}
			//Add to delimitor
			else
				++iDelimitor;

			break;
		case '\'': //Single quote
			//No quote set yet
			if (!(fFlags & F_QUOTE_SET))
			{
				fFlags |= F_QUOTE_SET | F_SINGLE_QUOTE;

				//Terminate
				if (!(fFlags & F_RAW_STRING))
					goto gt_break;
				else
					pcDelimitor = p_pcBuffer;
			}
			//Add to delimitor
			else
				++iDelimitor;

			break;
		case '(':
			//Terminate delimitor
			if (fFlags & F_QUOTE_SET)
				goto gt_break;

			return ERROR;
		default:
			//Add delimitor
			if (fFlags & F_QUOTE_SET)
				++iDelimitor;
			else
				return ERROR;

			break;
		}
	}

gt_break:;

	//Raw string
	if (fFlags & F_RAW_STRING)
	{
		p_output.iCustom = SCPF_RAW_STRING;

		while (p_iSize--)
		{
			switch (*p_pcBuffer++)
			{
			case ')':
				//Check delimitor size + quote
				if (p_iSize < iDelimitor + 1)
					return ERROR;
				else if (!memcmp(pcDelimitor, p_pcBuffer, iDelimitor))
				{
					if (!(fFlags & F_SINGLE_QUOTE) && p_pcBuffer[iDelimitor] == '"' ||
						fFlags & F_SINGLE_QUOTE && p_pcBuffer[iDelimitor] == '\'')
					{
						p_output.iBufferPadding = iDelimitor + 2;

						return SUCCESS;
					}
				}
			default:
				++p_output.iTokenSize;

				break;
			}
		}
	}
	else
	{
		const auto cpcEnd = p_pcBuffer + p_iSize;
		const auto cpcBegin = p_pcBuffer;

		while (p_pcBuffer < cpcEnd)
		{
			try
			{
				switch (utf8::next(p_pcBuffer, cpcEnd))
				{
				case '\\':
					utf8::advance(p_pcBuffer, 1, cpcEnd);

					break;
				case '\'':
					if (fFlags & F_SINGLE_QUOTE)
					{
						p_output.iBufferPadding = 1;
						p_output.iTokenSize = p_pcBuffer - cpcBegin - 1;

						return SUCCESS;
					}
				case '"':
					if (!(fFlags & F_SINGLE_QUOTE))
					{
						p_output.iBufferPadding = 1;
						p_output.iTokenSize = p_pcBuffer - cpcBegin - 1;

						return SUCCESS;
					}
				default:
					break;
				}
			}
			catch (...)
			{
				return ERROR;
			}
		}
	}
	
	return ERROR;
}



class interpreter_token final
{
public:
	interpreter_token() = delete;
	
	/**
	 * Matches all whitespace or padding characters it can.
	 *
	 * @since 3.64.127.716
	 * @date 6-May-18
	 *
	 * @tparam _Flags The flags for the behavior.
	 * @tparam _Begin If this is called before or after the token.
	 *
	 * @param [in] _input The input buffer.
	 * @param [in] _encoder The encoder.
	 *
	 * @throws See padding_skipper() and whitespace_skipper().
	 *
	 * @return true if succeded according to the @a _Flags, otherwise false.
	*/
	template<flags::flag_type _Flags, bool _Begin>
	static bool whitespace_deleter(stream::input_stream & _input, encoding::utf * _encoder) noexcept
	{
		constexpr auto match_begin = _Begin && (_Flags & (flags::starting_ws_token | flags::starting_ws_opt_token | flags::starting_padding_token | flags::starting_padding_opt_token));
		constexpr auto match_end = !_Begin && (_Flags & (flags::ending_ws_token | flags::ending_ws_opt_token));

		if (match_begin || match_end) {
			constexpr auto skipper = _Flags & (flags::starting_padding_token | flags::starting_padding_opt_token) ? padding_skipper : whitespace_skipper;

			// No characters were skipped
			if (!skipper(_input, _encoder)) {
				// But they are required
				if (_Begin && (_Flags & (flags::starting_ws_token | flags::starting_padding_token)) || !_Begin && (_Flags & flags::ending_ws_token)) {
					return false;
				}
			}
		}

		return true;
	}
	/**
	 * Matches all whitespace characters it can.
	 *
	 * @since 3.64.127.716
	 * @date 6-May-18
	 *
	 * @param [in] _input The input buffer.
	 * @param [in] _encoder The encoder.
	 *
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return true if any whitespace was matched, otherwise false.
	*/
	static bool whitespace_skipper(stream::input_stream & _input, encoding::utf * _encoder);
	/**
	 * Matches all padding characters it can.
	 *
	 * @since 3.64.127.716
	 * @date 6-May-18
	 *
	 * @param [in] _input The input buffer.
	 * @param [in] _encoder The encoder.
	 *
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return true if any padding character was matched, otherwise false.
	*/
	static bool padding_skipper(stream::input_stream & _input, encoding::utf * _encoder);
	/**
	 * Matches a number token.
	 *
	 * @since	3.64.127.716
	 * @date	9-Apr-18
	 *
	 * @param	_buffer	Defines the buffer that should be matched.
	 * @param	_length	Defines the length of the buffer.
	 * @param	_params	(Not used)	Defines additional interpreter information.
	 * @param	[out]	_output	Defines the token result.
	 *
	 * @return	Defines the success code. See @ref ACTION.
	*/
	static ACTION number(stream::input_stream & _input, token_param _params, token_output & _output) noexcept;
	static ACTION string(stream::input_stream & _input, token_param _params, token_output & _output);
	/**
	 * Matches an identifier. An identifier conists of alphanumeric characters and an underscore without a leading number.
	 *
	 * @since 3.64.127.716
	 * @date 24-Apr-18
	 *
	 * @param [in] _input The input buffer.
	 * @param _params Additional interpreter information.
	 * @param [out] _output The token result.
	 *
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return Defines the success code. See @ref ACTION.
	*/
	static ACTION identifier(stream::input_stream & _input, token_param _params, token_output & _output);
	/**
	 * Matches an assign operator.
	 *
	 * @remarks	A leading whitespace is optional and will be consumed, if present.
	 *
	 * @since 3.64.127.716
	 * @date 24-Apr-18
	 *
	 * @param [in] _input The input buffer.
	 * @param _params Additional interpreter information.
	 * @param [out] _output The token result.
	 *
	 * @throws See whitespace_deleter().
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return Defines the success code. See @ref ACTION.
	*/
	static ACTION assign_operator(stream::input_stream & _input, token_param _params, token_output & _output);
	/**
	 * Matches a compare operator.
	 *
	 * @remarks	A leading whitespace is optional and will be consumed, if present.
	 *
	 * @since 3.64.127.716
	 * @date 6-May-18
	 *
	 * @param [in] _input The input buffer.
	 * @param _params Additional interpreter information.
	 * @param [out] _output The token result.
	 *
	 * @throws See whitespace_deleter().
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return Defines the success code. See @ref ACTION.
	*/
	static ACTION compare_operator(stream::input_stream & _input, token_param _params, token_output & _output);
	/**
	 * Matches a comment which starts with '#' and ends with a line feed.
	 *
	 * @since 3.64.127.716
	 * @date 24-Apr-18
	 *
	 * @param [in] _input The input buffer.
	 * @param _params Additional interpreter information.
	 * @param [out] _output The token result.
	 *
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return Defines the success code. See @ref ACTION.
	*/
	static ACTION comment(stream::input_stream & _input, token_param _params, token_output & _output) noexcept;
	/**
	 * Matches a commend terminator.
	 *
	 * @since 3.64.127.716
	 * @date 24-Apr-18
	 *
	 * @param [in] _input The input buffer.
	 * @param _params Additional interpreter information.
	 * @param [out] _output The token result.
	 *
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return Defines the success code. See @ref ACTION.
	*/
	static ACTION command_end(stream::input_stream & _input, token_param _params, token_output & _output) noexcept;
	/**
	 * Matches a custom operator.
	 *
	 * @since 3.64.127.716
	 * @date 6-May-18
	 *
	 * @tparam _Flags Manipulating the behavior of this function. See @ref grammar::flags.
	 *
	 * @param [in] _input The input buffer.
	 * @param _params Additional interpreter information.
	 * @param [out] _output The token result.
	 *
	 * @throws See whitespace_deleter().
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return Defines the success code. See @ref ACTION.
	*/
	template<flags::flag_type _Flags>
	static ACTION custom_operator(stream::input_stream & _input, token_param _params, token_output & _output)
	{
		constexpr auto success = _Flags & flags::filler_token ? (_Flags & flags::looping_token ? ACTION::DONT_REPORT_AND_LOOP : ACTION::DONT_REPORT) : (_Flags & flags::looping_token ? ACTION::REPORT_AND_LOOP : ACTION::REPORT);
		constexpr auto error = _Flags & flags::opt_token ? ACTION::DONT_REPORT : (_Flags & flags::looping_token ? ACTION::DONT_REPORT : ACTION::ERROR);

		// Starting whitespaces
		if (!whitespace_deleter<_Flags, true>(_input, _params.encoder)) {
			return error;
		}

		if (_input.available() > 0) {
			auto _max = BIA_MAX_OPERATOR_LENGTH;
			auto _buffer = _input.get_buffer();
			auto _prev = _buffer.first;

			while (_max-- && _params.encoder->has_next(_buffer.first, _buffer.second)) {
				_prev = _buffer.first;
				auto _code_point = _params.encoder->next(_buffer.first, _buffer.second);

				switch (_code_point) {
				case '*':
				case '/':
				case '%':
				case '+':
				case '-':
				case '!':
				case '~':
				case '^':
				case '&':
				case '|':
				case '$':
					_output.content.content.operatorCode = _output.content.content.operatorCode << 8 | _code_point;

					break;
				default:
					break;
				}
			}

			if (_output.content.content.operatorCode) {
				_output.content.type = report::TYPE::OPERATOR_CODE;

				// Move cursor
				_input.skip(_prev);

				// Ending whitespaces
				if (!whitespace_deleter<_Flags, false>(_input, _params.encoder)) {
					return error;
				}

				return success;
			}
		}

		return error;
	}
	/**
	 * Matches a keyword token.
	 *
	 * @since 3.64.127.716
	 * @date 24-Apr-18
	 *
	 * @tparam _Ty The keyword. See @file interpreter_id.hpp for all keywords and operators.
	 * @tparam _Flags Manipulating the behavior of this function. See @ref grammar::flags.
	 *
	 * @param [in] _input The input buffer.
	 * @param _params Additional interpreter information.
	 * @param [out] _output The token result.
	 *
	 * @throws See whitespace_deleter().
	 * @throws See stream::input_stream::available(), stream::input_stream::get_buffer() and stream::input_stream::skip().
	 * @throws See encoding::utf::next().
	 *
	 * @return Defines the success code. See @ref ACTION.
	*/
	template<typename _Ty, flags::flag_type _Flags = flags::none>
	static ACTION keyword(stream::input_stream & _input, token_param _params, token_output & _output)
	{
		constexpr auto success = _Flags & flags::filler_token ? (_Flags & flags::looping_token ? ACTION::DONT_REPORT_AND_LOOP : ACTION::DONT_REPORT) : (_Flags & flags::looping_token ? ACTION::REPORT_AND_LOOP : ACTION::REPORT);
		constexpr auto error = _Flags & flags::opt_token ? ACTION::DONT_REPORT : (_Flags & flags::looping_token ? ACTION::DONT_REPORT : ACTION::ERROR);

		static_assert(_Ty::length() <= BIA_MAX_KEYWORD_LENGTH, "Keyword length exceeded.");

		// Starting whitespaces
		if (!whitespace_deleter<_Flags, true>(_input, _params.encoder)) {
			return error;
		}

		if (_input.available() >= _Ty::length()) {
			auto _buffer = _input.get_buffer();
			auto _keyword = _Ty::token();
			auto _length = _Ty::length();

			while (_params.encoder->has_next(_buffer.first, _buffer.second)) {
				if (_params.encoder->next(_buffer.first, _buffer.second) != *_keyword++) {
					break;
				}

				if (!--_length) {
					// Set output
					_output.content.type = report::TYPE::KEYWORD;
					_output.content.content.keyword = _Ty::string_id();

					// Move cursor
					_input.skip(_buffer.first);

					// Ending whitespaces
					if (!whitespace_deleter<_Flags, false>(_input, _params.encoder)) {
						return error;
					}

					return success;
				}
			}
		}

		return error;
	}
	/**
	 * Matches a rule pointer token.
	 *
	 * @since 3.64.127.716
	 * @date 9-Apr-18
	 *
	 * @tparam _Rule The rule id. See @ref BIA_GRAMMAR_RULE.
	 * @tparam _Flags Manipulating the behavior of this function. See @ref grammar::flags.
	 *
	 * @param [in] _input The input buffer.
	 * @param _params Additional interpreter information.
	 * @param [out] _output The token result.
	 *
	 * @throws See whitespace_deleter().
	 * @throws See interpreter_rule::run_rule().
	 *
	 * @return Defines the success code. See @ref ACTION.
	*/
	template<report::rule_type _Rule, flags::flag_type _Flags = flags::filler_token>
	static ACTION rule_pointer(stream::input_stream & _input, token_param _params, token_output & _output)
	{
		constexpr auto success = _Flags & flags::filler_token ? (_Flags & flags::looping_token ? ACTION::DONT_REPORT_AND_LOOP : ACTION::DONT_REPORT) : (_Flags & flags::looping_token ? ACTION::REPORT_AND_LOOP : ACTION::REPORT);
		constexpr auto error = _Flags & (flags::opt_token | flags::looping_token) ? ACTION::DONT_REPORT : ACTION::ERROR;
		
		// Starting whitespaces
		if (!whitespace_deleter<_Flags, true>(_input, _params.encoder)) {
			return error;
		}

		auto _result = _params.rules[_Rule].run_rule(_input, _params) ? success : error;

		// Ending whitespaces
		if (!whitespace_deleter<_Flags, false>(_input, _params.encoder)) {
			return error;
		}

		return _result;
	}

private:
	 /*
	1: sign information
	2: binary value
	3: hex value
	4: octal value
	5: decimal/floating point value
	6: is floating point
	7: is floating point
	*/
	const static std::regex _number_pattern;

	static int get_value(char _digit) noexcept;
	/**
	 * Parses the integer that was matched by interpreter_token::number().
	 *
	 * @remarks Do not use this function to parse other integers!
	 *
	 * @since 3.64.127.716
	 * @date 9-Apr-18
	 *
	 * @param _buffer The data.
	 *
	 * @return The parsed value.
	*/
	static int64_t parse_integer(stream::input_stream::buffer_type _buffer, encoding::utf * _encoder, int _base) noexcept;
	/**
	 * Parses the double that was matched by interpreter_token::number().
	 *
	 * @remarks Do not use this function to parse other doubles!
	 *
	 * @since 3.64.127.716
	 * @date 9-Apr-18
	 *
	 * @param _buffer The data.
	 *
	 * @return The parsed value.
	*/
	static double parse_double(stream::input_stream::buffer_type _buffer, encoding::utf * _encoder) noexcept;
};

}
}