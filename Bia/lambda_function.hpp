#pragma once

#include <cstdarg>

#include "function.hpp"
#include "exception.hpp"
#include "disguised_caller.hpp"
#include "share.hpp"


namespace bia
{
namespace framework
{
namespace executable
{

/**
 * @brief A lambda function member.
 *
 * @tparam _Lambda The lambda function.
*/
template<typename Lambda>
class lambda_function final : public function
{
public:
	typedef utility::share<Lambda> data_type;

	/**
	 * Constructor.
	 *
	 * @since 3.64.127.716
	 * @date 21-Apr-18
	 *
	 * @param _lambda The lambda function.
	 *
	 * @throws See utility::share::share().
	*/
	explicit lambda_function(const Lambda & _lambda) : _data(_lambda)
	{
	}
	/**
	 * Constructor.
	 *
	 * @since 3.64.127.716
	 * @date 21-Apr-18
	 *
	 * @param _lambda The lambda function.
	 *
	 * @throws See utility::share::share().
	*/
	explicit lambda_function(Lambda && _lambda) : _data(std::move(_lambda))
	{
	}
	/**
	 * Refer-Constructor.
	 *
	 * @since 3.65.134.742
	 * @date 1-Aug-18
	 *
	 * @param _lambda The lambda function.
	*/
	explicit lambda_function(const data_type & _data) noexcept : _data(_data)
	{
	}
	virtual void BIA_MEMBER_CALLING_CONVENTION print() const override
	{
		puts(typeid(&Lambda::operator()).name());
	}
	virtual void BIA_MEMBER_CALLING_CONVENTION copy(member * _destination) override
	{
		_destination->replace_this<lambda_function<Lambda>>(_data.get());
	}
	virtual void BIA_MEMBER_CALLING_CONVENTION refer(member * _destination) override
	{
		_destination->replace_this<lambda_function<Lambda>>(_data);
	}
	virtual void BIA_MEMBER_CALLING_CONVENTION clone(member * _destination) override
	{
		refer(_destination);
	}
	virtual void BIA_MEMBER_CALLING_CONVENTION execute(member * _destination) override
	{
		force::disguised_caller(&Lambda::operator(), &_data.get(), _destination);
	}
	virtual void BIA_VARG_MEMBER_CALLING_CONVENTION execute_count(member * _destination, void * _reserved, parameter_count _count...) override
	{
		force::va_list_wrapper _args;
		va_start(_args.args, _count);

		force::disguised_caller_count(&Lambda::operator(), &_data.get(), _destination, _count, _args);

		va_end(_args.args);
	}
	virtual void BIA_VARG_MEMBER_CALLING_CONVENTION execute_format(member * _destination, const char * _format, parameter_count _count...) override
	{
		force::va_list_wrapper _args;
		va_start(_args.args, _count);

		force::disguised_caller_format(&Lambda::operator(), &_data.get(), _destination, _format, _count, _args);

		va_end(_args.args);
	}
	virtual void set_instance(const void * _instance, const std::type_info & _type) override
	{
	}

private:
	/** The lambda function. */
	data_type _data;
};

}
}
}