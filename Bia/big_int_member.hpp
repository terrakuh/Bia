#pragma once

#include <cstdint>
#include <cstdlib>

#include "config.hpp"
#include "native_variable.hpp"
#include "share_def.hpp"
#include "big_int.hpp"


namespace bia
{
namespace framework
{
namespace native
{

class big_int_member : public native_variable
{
public:
	typedef utility::share<dependency::big_int> data_type;

	/**
	 * Constructor. The default value is 0.
	 *
	 * @since 3.69.144.799
	 * @date 5-Dec-18
	 *
	 * @throws See utility::share::share().
	*/
	BIA_EXPORT big_int_member();
	/**
	 * Constructor.
	 *
	 * @since 3.68.140.790
	 * @date 20-Oct-18
	 *
	 * @param _value The initial value.
	 *
	 * @throws See utility::share::share().
	*/
	BIA_EXPORT big_int_member(int32_t _value);
	/**
	 * Constructor.
	 *
	 * @since 3.68.140.790
	 * @date 20-Oct-18
	 *
	 * @param _value The initial value.
	 *
	 * @throws See utility::share::share().
	*/
	BIA_EXPORT big_int_member(int64_t _value);
	/**
	 * Constructor.
	 *
	 * @since 3.68.140.790
	 * @date 20-Oct-18
	 *
	 * @param _value The initial value.
	 *
	 * @throws See utility::share::share().
	*/
	BIA_EXPORT big_int_member(const dependency::big_int & _value);
	/**
	 * Refer-Constructor.
	 *
	 * @since 3.68.140.790
	 * @date 20-Oct-18
	 *
	 * @param _data The data.
	*/
	BIA_EXPORT big_int_member(const data_type & _data) noexcept;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION print() const override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION copy(member * _destination) override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION refer(member * _destination) override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION clone(member * _destination) override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION operator_call(member * _destination, operator_t _operator, const member * _right) override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION operator_call_int32(member * _destination, operator_t _operator, int32_t _right) override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION operator_call_int64(member * _destination, operator_t _operator, int64_t _right) override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION operator_call_big_int(member * _destination, operator_t _operator, const dependency::big_int * _right) override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION operator_call_double(member * _destination, operator_t _operator, double _right) override;
	BIA_EXPORT virtual void BIA_MEMBER_CALLING_CONVENTION object_member(member * _destination, machine::name_manager::name_t _name) override;
	BIA_EXPORT virtual int flags() const override;
	BIA_EXPORT virtual int32_t BIA_MEMBER_CALLING_CONVENTION test() const override;
	BIA_EXPORT virtual int32_t BIA_MEMBER_CALLING_CONVENTION test_member(operator_t _operator, member * _right) const override;
	BIA_EXPORT virtual int32_t BIA_MEMBER_CALLING_CONVENTION test_int32(operator_t _operator, int32_t _right) const override;
	BIA_EXPORT virtual int32_t BIA_MEMBER_CALLING_CONVENTION test_int64(operator_t _operator, int64_t _right) const override;
	BIA_EXPORT virtual int32_t BIA_MEMBER_CALLING_CONVENTION test_double(operator_t _operator, double _right) const override;
	BIA_EXPORT virtual int64_t to_int() const override;
	BIA_EXPORT virtual double to_double() const override;
	BIA_EXPORT virtual const char * to_cstring(utility::buffer_builder * _builder) const override;

protected:
	BIA_EXPORT virtual int32_t int32_data(bool & _success) const override;
	BIA_EXPORT virtual int64_t int64_data(bool & _success) const override;
	BIA_EXPORT virtual double double_data(bool & _success) const override;

private:
	/** Holds the big integer. */
	data_type _data;
};

}
}
}