#pragma once

#include "report_bundle.hpp"


namespace bia
{
namespace grammar
{

enum BIA_GRAMMAR_RULE : report::rule_t
{
	BGR_ROOT,
	BGR_ROOT_HELPER_0,
	BGR_ROOT_HELPER_1,
	BGR_ROOT_HELPER_2,

	BGR_NORMAL_STATEMENT,

	BGR_VARIABLE_DECLARATION,
	BGR_VARIABLE_DECLARATION_INLINE,
	BGR_VARIABLE_DECLARATION_HELPER_0,

	BGR_IF,
	BGR_IF_HELPER_0,
	BGR_IF_HELPER_1,

	// Loops
	BGR_TEST_LOOP,
	BGR_TEST_LOOP_HELPER_0,

	// Import
	BGR_IMPORT,

	BGR_VALUE_RAW,
	BGR_PARAMETER,
	BGR_PARAMETER_ITEM_ACCESS,
	BGR_PARAMETER_HELPER_0,
	BGR_PARAMETER_HELPER_1,

	BGR_MATH_FACTOR,
	BGR_MATH_FACTOR_HELPER_0,
	BGR_MATH_TERM,
	BGR_MATH_TERM_HELPER_0,

	BGR_MATH_EXPRESSION,
	BGR_MATH_EXPRESSION_HELPER_0,

	BGR_CONDITION_EXPRESSION,
	BGR_CONDITION_EXPRESSION_HELPER_0,

	BGR_VALUE_EXPRESSION,
	BGR_VALUE_EXPRESSION_HELPER_0,
	BGR_VALUE_EXPRESSION_HELPER_1,

	BGR_VALUE,
	BGR_VALUE_HELPER_0,

	BGR_MEMBER,
	BGR_MEMBER_HELPER_0,
	BGR_MEMBER_HELPER_1,

	BGR_RULE_COUNT,

	// Rules not in the rule set
	BGR_CONTROL_STATEMENT
};

enum BIA_SCOPE
{
	BS_NO_SCOPE = 0,
	BS_SCOPE
};

enum BIA_VALUE
{
	BV_NUMBER = 1,
	BV_TRUE,
	BV_FALSE,
	BV_MEMBER
};

enum BIA_VALUE_OPERATOR
{
	BVO_LOGICAL_AND = 1,
	BVO_LOGICAL_OR
};

enum NUMBER_ID
{
	NI_BASE_MASK = 0xf,

	NI_INTEGER = 0x10,
	NI_DOUBLE,
	NI_FLOAT,

	NI_NUMBER_ID_MASK = 0xf0,

	NI_NEGATIVE_NUMBER = 0x100
};

}
}