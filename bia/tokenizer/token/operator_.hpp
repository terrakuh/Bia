#ifndef BIA_TOKENIZER_TOKEN_OPERATOR_HPP_
#define BIA_TOKENIZER_TOKEN_OPERATOR_HPP_

#include <bia/exception/implementation_error.hpp>
#include <bia/member/operator_.hpp>
#include <cstdint>

namespace bia {
namespace tokenizer {
namespace token {

typedef std::int8_t precedence_type;

enum class operator_type
{
	infix,
	prefix,
	postfix
};

template<precedence_type Precedence, std::uint8_t Variant, operator_type Type>
constexpr inline std::uint16_t make()
{
	static_assert(Precedence >= 0 && Precedence <= 0xe, "invalid precedence");
	static_assert(Variant <= 0xf, "too high variant");

	return (Variant << 6) | (Precedence << 2) | static_cast<int>(Type);
}

enum class operator_ : std::uint16_t
{
	logical_or           = make<0, 0, operator_type::infix>(),
	logical_and          = make<1, 0, operator_type::infix>(),
	logical_not          = make<2, 0, operator_type::prefix>(),
	equal                = make<3, 0, operator_type::infix>(),
	not_equal            = make<3, 1, operator_type::infix>(),
	less                 = make<3, 2, operator_type::infix>(),
	less_equal           = make<3, 3, operator_type::infix>(),
	greater              = make<3, 4, operator_type::infix>(),
	greater_equal        = make<3, 5, operator_type::infix>(),
	in                   = make<3, 6, operator_type::infix>(),
	three_way_comparison = make<4, 0, operator_type::infix>(),
	bitwise_or           = make<5, 0, operator_type::infix>(),
	bitwise_xor          = make<6, 0, operator_type::infix>(),
	bitwise_and          = make<7, 0, operator_type::infix>(),
	bitwise_left_shift   = make<8, 0, operator_type::infix>(),
	bitwise_right_shift  = make<8, 1, operator_type::infix>(),
	addition             = make<9, 0, operator_type::infix>(),
	subtraction          = make<9, 1, operator_type::infix>(),
	multiplication       = make<10, 0, operator_type::infix>(),
	division             = make<10, 1, operator_type::infix>(),
	remainder            = make<10, 2, operator_type::infix>(),
	unary_minus          = make<11, 0, operator_type::prefix>(),
	bitwise_not          = make<11, 1, operator_type::prefix>(),
	exponentation        = make<12, 0, operator_type::infix>(),
	member_access        = make<13, 0, operator_type::infix>(),
	function_call_open   = make<13, 1, operator_type::prefix>(),
	function_call_close  = make<13, 2, operator_type::postfix>(),
};

inline precedence_type precedence_of(operator_ op) noexcept
{
	return static_cast<precedence_type>((static_cast<std::uint16_t>(op) >> 2) & 0xf);
}

inline operator_type type_of(operator_ op) noexcept
{
	return static_cast<operator_type>(static_cast<std::uint16_t>(op) & 0x3);
}

inline member::infix_operator to_infix_operator(operator_ op)
{
	switch (op) {
	case operator_::bitwise_or: return member::infix_operator::bitwise_or;
	case operator_::bitwise_xor: return member::infix_operator::bitwise_xor;
	case operator_::bitwise_and: return member::infix_operator::bitwise_and;
	case operator_::bitwise_left_shift: return member::infix_operator::bitwise_left_shift;
	case operator_::bitwise_right_shift: return member::infix_operator::bitwise_right_shift;
	case operator_::addition: return member::infix_operator::addition;
	case operator_::subtraction: return member::infix_operator::subtraction;
	case operator_::multiplication: return member::infix_operator::multiplication;
	case operator_::division: return member::infix_operator::division;
	case operator_::remainder: return member::infix_operator::remainder;
	case operator_::exponentation: return member::infix_operator::exponentation;
	default: BIA_IMPLEMENTATION_ERROR("invalid operator conversion");
	}
}

inline member::test_operator to_test_operator(operator_ op)
{
	switch (op) {
	case operator_::equal: return member::test_operator::equal;
	case operator_::not_equal: return member::test_operator::not_equal;
	case operator_::less: return member::test_operator::less;
	case operator_::less_equal: return member::test_operator::less_equal;
	case operator_::greater: return member::test_operator::greater;
	case operator_::greater_equal: return member::test_operator::greater_equal;
	case operator_::in: return member::test_operator::in;
	case operator_::three_way_comparison: return member::test_operator::three_way_comparison;
	default: BIA_IMPLEMENTATION_ERROR("invalid operator conversion");
	}
}

inline member::self_operator to_self_operator(operator_ op)
{
	switch (op) {
	case operator_::unary_minus: return member::self_operator::unary_minus;
	case operator_::bitwise_not: return member::self_operator::bitwise_not;
	default: BIA_IMPLEMENTATION_ERROR("invalid operator conversion");
	}
}

} // namespace token
} // namespace tokenizer
} // namespace bia

#endif
