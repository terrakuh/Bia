#pragma once

#include "function_base_member.hpp"

#include <connector/connector.hpp>

namespace bia {
namespace member {
namespace function {

template<typename Return, typename... Args>
class static_function_member : public function_base_member
{
public:
	typedef Return (*function_type)(Args...);

	static_function_member(function_type function) : function(function)
	{}

	virtual flag_type flags() const override
	{
		return F_NONE;
	}
	virtual member* shallow_copy() const override
	{
		return deep_copy();
	}
	virtual member* deep_copy() const override
	{
		return gc::gc::active_gc()->construct<static_function_member>(*this);
	}
	virtual member* call(stack::stack* stack, std::int32_t param_count) override
	{
		return connector::static_connector(function, stack, param_count);
	}
	virtual bool_type test() const override
	{
		return static_cast<bool_type>(function != nullptr);
	}

protected:
	virtual void gc_mark_children(bool mark) const noexcept override
	{}
	virtual bool other_data(const std::type_info& type, void*& data) override
	{
		return false;
	}
	virtual bool other_data(const std::type_info& type, const void*& data) const override
	{
		return false;
	}

private:
	const function_type function;
};

} // namespace function
} // namespace member
} // namespace bia