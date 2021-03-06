#include "../jump_manager.hpp"
#include "expression.hpp"
#include "helpers.hpp"

bia::compiler::elve::tokens_type bia::compiler::elve::while_(present present, tokens_type tokens)
{
	using namespace tokenizer::token;

	jump_manager manager{ &present.writer.output() };
	bytecode::member::local condition{ present.variables.add_tmp() };

	manager.mark(jump_manager::destination::start);

	tokens = expression(present, tokens.subspan(1), condition);

	auto count = tokens.data()->value.get<token::batch>().count;

	present.writer.write<true, bytecode::oc_test>(
	    static_cast<typename std::underlying_type<member::test_operator>::type>(member::test_operator::self),
	    condition, condition);
	manager.jump(jump_manager::type::if_false, jump_manager::destination::end);

	tokens = tokens.subspan(1);

	while (count--) {
		// flow control
		if (static_cast<token::type>(tokens.data()->value.index()) == token::type::keyword) {
			switch (tokens.data()->value.get<token::keyword>()) {
			case token::keyword::break_:
				manager.jump(jump_manager::type::unconditional, jump_manager::destination::end);
				break;
			case token::keyword::continue_:
				manager.jump(jump_manager::type::unconditional, jump_manager::destination::start);
				break;
			default: goto gt_continue;
			}

			BIA_EXPECTS(tokens.size() >= 2 &&
			            static_cast<token::type>(tokens.data()[1].value.index()) == token::type::cmd_end);

			tokens = tokens.subspan(2);
		} else {
		gt_continue:;
			tokens = statement(present, tokens);
		}
	}

	manager.jump(jump_manager::type::unconditional, jump_manager::destination::start);
	manager.mark(jump_manager::destination::end);
	present.variables.remove_tmp(condition.index);

	return tokens;
}
