#pragma once

#include "yinterpreter.h"


namespace bia
{
namespace api
{
namespace grammar
{

enum BIA_GRAMMAR_IDENTIFIER
{
	BGI_R_ROOT,
	BGI_R_VALUE,
	BGI_R_MATH_EXPRESSION,

	BGI_OPERATOR,
	BGI_DOT_OPERATOR,
	BGI_ASSIGN_OPERATOR,
	BGI_CONDITIONAL_AND,
	BGI_CONDITIONAL_OR,
	BGI_QUOTE,
	BGI_EXCLAMATION_MARK,

	BGI_KEY_ELSE,
	BGI_KEY_TRUE,
	BGI_KEY_FALSE,
	BGI_KEY_NULL,
	BGI_COPYOF,
	BGI_IDENTIFIER,
	BGI_NUMBER,
	BGI_STRING,
	BGI_VALUE,
	BGI_PARAMETER_LIST,
	BGI_MEMBER_CALL,
	BGI_NUMBER_LITERAL,
	BGI_CONDITION,
	BGI_VARIABLE_DECLARATION,
	BGI_VARIABLE_ASSIGNMENT,
	BGI_FOR_LOOP_RIGHT,
	BGI_LOOP_REGISTERS,
	BGI_INSTANTIATION,
	BGI_ITEM_ACCESS,
	BGI_IMPORT_MODULE,

	BGI_MATH_TERM,
	BGI_MATH_EXPRESSION,
	BGI_MATH_PLUS,
	BGI_MATH_MINUS,

	BGI_TERM_VARIABLE_DECLARATION,
	BGI_TERM_VARIABLE_ASSIGNEMENT,
	BGI_TERM_CALL,
	BGI_TERM_WHILE,
	BGI_TERM_IF,
	BGI_TERM_EMPTY,
	BGI_TERM_FOR,
	BGI_TERM_RANGE_LOOP,
	BGI_TERM_PRINT,
	BGI_TERM_RETURN,
	BGI_TERM_DELETE,
	BGI_TERM_IMPORT,
	BGI_TERM_FUNCTION_DEFINITION
};

class BiaGrammar
{
public:
	static void InitializeGrammar();

	static yinterpreter sBiaGrammar;
};

}
}
}