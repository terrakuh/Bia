#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <map>
#include <string>

#include "config.hpp"
#include "machine_context.hpp"


namespace bia
{
namespace machine
{

class disassembler
{
public:
	BIA_EXPORT disassembler(const void * _context_address) noexcept;
	BIA_EXPORT void disassemble(const void * _code, size_t _size) const;
	/**
	 * Returns the name for the given address.
	 *
	 * @since 3.64.127.718
	 * @date 16-May-18
	 *
	 * @param _address The address.
	 *
	 * @return The name if available, otherwise null.
	*/
	BIA_EXPORT const char * name(const void * _address) const noexcept;

private:
	struct instruction
	{
		typedef std::function<void(const disassembler*, const int8_t*)> callback_function;

		uint64_t op_code;
		uint64_t mask;
		callback_function callback;
		uint8_t size;
	};

	typedef std::vector<instruction> instruction_list;
	typedef std::map<const void*, std::string> function_map;

	//const BiaMachineContext * m_pMachineContextAddress;	/**	Defines the address of the machine context.	*/
	//variable_index & m_index;	/**	Defines the index holding all global variables.	*/

	/** The address of the corresponding context. */
	const void * _context_address;

	/** Holds all used machine instructions. */
	static instruction_list _instructions;
	/** Holds all addresses of all Bia functions. */
	static function_map _function_map;

	/**
	 * Creates a list with all instructions for the active platform.
	 *
	 * @since 3.64.128.720
	 * @date 16-May-18
	 *
	 * @throws See std::vector::emplace_back().
	 *
	 * @return The instruction list.
	*/
	BIA_EXPORT static instruction_list init_instructions();
	/**
	 * Creates a map with all addresses of the Bia functions.
	 *
	 * @since 3.64.128.720
	 * @date 16-May-18
	 *
	 * @throws See std::map::emplace().
	 *
	 * @return The function map.
	*/
	BIA_EXPORT static function_map init_function_map();
};

}
}