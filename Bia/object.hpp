#pragma once

#include <type_traits>
#include <cstdarg>
#include <utility>

#include "member.hpp"
#include "allocator.hpp"
#include "undefined_member.hpp"
#include "share.hpp"
#include "type_traits.hpp"
#include "constructor_chain.hpp"
#include "machine_context.hpp"
#include "member_map.hpp"
#include "instance_holder.hpp"


namespace bia
{
namespace framework
{
namespace object
{

template<typename _Ty>
class object : public member
{
public:
	typedef utility::share<std::pair<instance_holder<_Ty>, member_map>> data_type;

	/**
	 * Constructor.
	 *
	 * @remarks A new share will be created for the object.
	 *
	 * @since 3.67.135.753
	 * @date 7-Aug-18
	 *
	 * @param _instance The instance.
	 * @param _members The known members.
	*/
	object(const instance_holder<_Ty> & _instance, const member_map & _members) noexcept : _data(_instance, _members)
	{
	}
	/**
	 * Refer-Constructor.
	 *
	 * @since 3.67.135.753
	 * @date 7-Aug-18
	 *
	 * @param _data The data.
	*/
	object(const data_type & _data) noexcept : _data(_data)
	{
	}
	virtual void undefine() noexcept override
	{
		replace_this<undefined_member>();
	}
	virtual void print() const override
	{
		printf("<%s at %p>\n", typeid(_Ty).name(), this);
	}
	virtual void copy(member * _destination) override
	{
		instance_holder<_Ty> _instance(constructor_chain_wrapper<_Ty>(machine::machine_context::active_allocator(), *_data.get().first.get()), true);

		_destination->replace_this<object<_Ty>>(_instance, _data.get().second);
	}
	virtual void refer(member * _destination) override
	{
		_destination->replace_this<object<_Ty>>(_data);
	}
	virtual void clone(member * _destination) override
	{
		refer(_destination);
	}
	virtual void execute(member * _destination) override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual void execute_count(member * _destination, parameter_count _count...) override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual void execute_format(member * _destination, const char * _format, parameter_count _count...) override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual void operator_call(member * _destination, operator_type _operator, const member * _right) override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual void operator_call_int32(member * _destination, operator_type _operator, int32_t _right) override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual void operator_call_int64(member * _destination, operator_type _operator, int64_t _right) override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual void operator_call_double(member * _destination, operator_type _operator, double _right) override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual int flags() const override
	{
		return F_NONE;
	}
	virtual int32_t test() const override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual int32_t test_member(operator_type _operator, member * _right) const override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual int32_t test_int32(operator_type _operator, int32_t _right) const override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual int32_t test_int64(operator_type _operator, int64_t _right) const override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual int32_t test_double(operator_type _operator, double _right) const override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual int64_t to_int() const override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual double to_double() const override
	{
		BIA_NOT_IMPLEMENTED;
	}
	virtual member * object_member(machine::string_manager::name_type _name) override
	{
		auto _result = _data.get().second.find(_name);

		if (_result == _data.get().second.end()) {
			throw exception::symbol_error(BIA_EM_UNDEFINED_MEMBER);
		}

		return _result->second;
	}

protected:
	virtual void * native_data(native::NATIVE_TYPE _type) override
	{
		throw exception::type_error(BIA_EM_UNSUPPORTED_TYPE);
	}
	virtual const void * const_native_data(native::NATIVE_TYPE _type) const override
	{
		throw exception::type_error(BIA_EM_UNSUPPORTED_TYPE);
	}
	virtual void * data(const std::type_info & _type) override
	{
		if (!std::is_const<_Ty>::value) {
			if (typeid(_Ty) == _type) {
				return const_cast<typename std::remove_cv<_Ty>::type*>(static_cast<_Ty*>(_data.get().first.get()));
			} else if (typeid(_Ty*) == _type) {
				return &_data.get().first.get();
			}
		}

		throw exception::type_error(BIA_EM_UNSUPPORTED_TYPE);
	}
	virtual const void * const_data(const std::type_info & _type) const override
	{
		if (typeid(_Ty) == _type) {
			return _data.get().first.get();
		} else if (typeid(_Ty*) == _type) {
			return &_data.get().first.get();
		}

		throw exception::type_error(BIA_EM_UNSUPPORTED_TYPE);
	}

private:
	data_type _data;
};

}
}
}