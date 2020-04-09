#ifndef BIA_MEMBER_INTERFACE_MEMBER_HPP_
#define BIA_MEMBER_INTERFACE_MEMBER_HPP_

#include <cstdint>
#include <gc/object/base.hpp>

namespace bia {
namespace member {

/**
 * The base type for all member types. All members are gc objects and must provide the following functions.
 */
class member : public gc::object::base
{
public:
	/** the flag type */
	typedef int flag_type;
	/** the type for testing operations */
	typedef int test_type;

	/**
	 * Additional flags describing the member and how its going to behave in certain situations.
	 */
	enum flag : flag_type
	{
		flag_none = 0,
	};

	virtual ~member() = default;
	/**
	 * Returns the set flags of this member.
	 *
	 * @returns the flags
	 */
	virtual flag_type flags() const = 0;
	/**
	 *Tests this member.
	 *
	 * @returns `1` if the test succeeded, otherwise `0`
	 */
	virtual test_type test() const = 0;
};

} // namespace member
} // namespace bia

#endif