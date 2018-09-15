#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>

#include "config.hpp"
#include "machine_schein.hpp"


namespace bia
{
namespace machine
{

/**
 * @brief Holds executable machine code.
 *
 * For directly executable machine code.
 *
 * @see @ref machine::platform::architecture
*/
class machine_code
{
public:
	/**
	 * Constructor.
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @param _machine_code Defines the machine code generated by a suitable compiler.
	 * @param _machine_schein Defines some details needed for safe execution of the machine code.
	 *
	 * @throws See memory::allocator::allocate() and memory::executable_allocator::protect().
	 */
	BIA_EXPORT machine_code(std::pair<const uint8_t *, size_t> _machine_code, machine_schein _machine_schein);
	BIA_EXPORT machine_code(const machine_code & _copy) = delete;
	/**
	 * Move-Constructor.
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @param [in,out] _rvalue Defines the object that should be moved.
	*/
	BIA_EXPORT machine_code(machine_code && _rvalue) noexcept;
	/**
	 * Destructor.
	 *
	 * @since 3.64.127.716
	 * @date 6-May-18
	 *
	 * @throws See clear().
	*/
	BIA_EXPORT ~machine_code();
	/**
	 * Executes the machine code.
	 * 
	 * @since 3.42.93.562
	 * @date 14-Dec-2017
	 *
	 * @throws Any exception from the Bia script.
	 */
	BIA_EXPORT void execute() const;
	/**
	 * Clears the machine code. Cannot be executed afterwards.
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @throws See memory::allocator::deallocate().
	*/
	BIA_EXPORT void clear();
	/**
	 * Checks wheter the code is executable.
	 * 
	 * @remarks This function does not check if the machine code itself is valid.
	 * 
	 * @since 3.42.93.562
	 * @date 14-Dec-2017
	 * 
	 * @return true if the code can be executed, otherwise false.
	 */
	BIA_EXPORT bool is_executable() const noexcept;
	/**
	 * Move operator.
	 *
	 * @since 3.64.127.716
	 * @date 7-Apr-18
	 *
	 * @param [in,out] _rvalue Defines the value that should be moved.
	 *
	 * @throws See machine_code::clear().
	 *
	 * @return This.
	*/
	BIA_EXPORT machine_code & operator=(machine_code && _rvalue);

private:
	typedef void(*entry_point)();

	/** If non-null, defines the entry point of the machine code. */
	memory::universal_allocation _entry_point;
	/** Defines the machine code details. */
	machine_schein _machine_schein;
};

}
}