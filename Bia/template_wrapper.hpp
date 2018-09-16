#pragma once

#include <utility>
#include <type_traits>

#include "class_template.hpp"
#include "static_function.hpp"
#include "lambda_function.hpp"
#include "member_function.hpp"
#include "string_manager.hpp"


namespace bia
{
namespace framework
{
namespace object
{

template<typename _Ty, bool _Enable_set_constructor>
class template_wrapper
{
public:
	template_wrapper(machine::machine_context & _context, _Ty * _template_member) : _context(_context)
	{
		this->_template_member = _template_member;
	}
	template<typename... _Args, bool _T = _Enable_set_constructor>
	typename std::enable_if<_T, template_wrapper&>::type set_constructor()
	{
		// Set the active allocator
		_context.activate_context();

		_template_member->template set_constructor<_Args...>();

		return *this;
	}
	template<typename _Return, typename... _Args>
	template_wrapper & set_function(member_map::name_type _name, _Return(*_function)(_Args...))
	{
		// Set the active allocator
		_context.activate_context();

		_template_member->members().template emplace<framework::executable::static_function<_Return, _Args...>>(_context.name_address(_name), _function);

		return *this;
	}
	template<typename _Class, typename _Return, typename... _Args>
	template_wrapper & set_function(member_map::name_type _name, _Return(_Class::*_function)(_Args...))
	{
		// Set the active allocator
		_context.activate_context();

		_template_member->members().template emplace<framework::executable::member_function<_Return(_Class::*)(_Args...)>>(_context.name_address(_name), _function);

		return *this;
	}
	template<typename _Lambda>
	template_wrapper & set_lambda(member_map::name_type _name, _Lambda && _lambda)
	{
		// Set the active allocator
		_context.activate_context();

		_template_member->members().template emplace<framework::executable::lambda_function<typename std::remove_cv<typename std::remove_reference<_Lambda>::type>::type>>(_context.name_address(_name), std::forward<_Lambda>(_lambda));

		return *this;
	}

protected:
	machine::machine_context & _context;
	_Ty * _template_member;
};

}
}
}