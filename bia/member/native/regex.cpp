#include "regex.hpp"

#include "../cast/cast.hpp"

using namespace bia::member::native;

regex::regex(std::regex pattern) : _pattern{ std::move(pattern) }
{}

regex::flag_type regex::flags() const
{
	return flag_none;
}

regex::test_type regex::test(test_operator op, const member& right) const
{
	switch (op) {
	case test_operator::in: return std::regex_search(cast::cast<const char*>(right), _pattern);
	case test_operator::equal: return std::regex_match(cast::cast<const char*>(right), _pattern);
	case test_operator::not_equal: return !std::regex_match(cast::cast<const char*>(right), _pattern);
	default: return 0;
	}
}

bia::gc::gcable<bia::member::member> regex::copy() const
{
	return gc::gc::active_gc()->construct<regex>(*this).template to<member>();
}

bia::gc::gcable<bia::member::member> regex::invoke(parameters_type params)
{
	return {};
}

bia::gc::gcable<bia::member::member> regex::operation(const member& right, infix_operator op)
{
	return {};
}

bia::gc::gcable<bia::member::member> regex::self_operation(self_operator op)
{
	return {};
}

bia::member::member* regex::get(const native::string& name)
{
	return nullptr;
}

regex::float_type regex::as_float() const noexcept
{
	return {};
}

regex::int_type regex::as_int() const noexcept
{
	return {};
}

bool regex::as_data(const std::type_info& type, void* output)
{
	return false;
}

bool regex::as_data(const std::type_info& type, void* output) const
{
	return false;
}

void regex::gc_mark_children(bool mark) const noexcept
{}

void regex::register_gcables(gc::gc& gc) const noexcept
{}
