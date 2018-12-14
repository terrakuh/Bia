#pragma once

#include <vector>

#include "config.hpp"
#include "big_int_allocator.hpp"
#include "executable_allocator.hpp"


namespace bia
{
namespace machine
{

/**
 * @brief Contains information about machine code.
 *
 * Keeps track of all allocations of one machine code instance.
 *
 * @see @ref machine_code
*/
class machine_schein
{
public:
	typedef void(*deleter_function_signature)(memory::universal_allocation);

	/**
	 * Constructor.
	 *
	 * @remarks During the lifetime of this object the allocators have to stay vaild. They will not be released at any time.
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @param [in] _allocator The allocator for normal memory.
	 * @param [in] _executable_allocator The allocator for executable memory.
	*/
	BIA_EXPORT machine_schein(memory::allocator * _allocator, memory::executable_allocator * _executable_allocator) noexcept;
	machine_schein(const machine_schein & _copy) = delete;
	machine_schein(machine_schein && _move) noexcept = default;
	/**
	 * Destructor.
	 *
	 * @since 3.64.127.716
	 * @date 6-May-18
	 *
	 * @throws See delete_all_allocations().
	*/
	BIA_EXPORT ~machine_schein();
	/**
	 * Registers an allocation. Must be created by get_allocator().
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @param _allocation Defines the allocation.
	 * @param _deleter (Optional) The deleter function.
	 *
	 * @throws See std::vector::push_back().
	*/
	BIA_EXPORT void register_allocation(memory::universal_allocation _allocation, deleter_function_signature _deleter = nullptr);
	void register_big_int(memory::big_int_allocation _allocation)
	{
		///TODO
	}
	/**
	 * Deletes all registered allocations.
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @throws See memory::allocator::deallocate().
	*/
	BIA_EXPORT void delete_all_allocations();
	/**
	 * Returns the normal memory allocator.
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @return The pointer to the allocator.
	*/
	BIA_EXPORT memory::allocator * allocator() noexcept;
	/**
	 * Returns the executable memory allocator.
	 *
	 * @since 3.64.127.716
	 * @date 6-May-18
	 *
	 * @return The pointer to the allocator.
	*/
	BIA_EXPORT memory::executable_allocator * executable_allocator() noexcept;
	/**
	 * Move operator.
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @param [in,out] _move Defines the value that should be moved.
	 *
	 * @throws See delete_all_allocations().
	 *
	 * @return This.
	*/
	BIA_EXPORT machine_schein & operator=(machine_schein && _move);

private:
	/** The memory allocator for normal memory. */
	memory::allocator * _allocator;
	/** The memory allocator for executable memory. */
	memory::executable_allocator * _executable_allocator;
	/** Stores all registered allocations with deleter. */
	std::vector<std::pair<memory::universal_allocation, deleter_function_signature>> _allocated;
};

}
}