#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>


namespace bia
{
namespace machine
{

class BiaMachineCode
{
public:
	/**
	 * Constructor.
	 * 
	 * @param	p_machineCode	Defines the machine code generated by a suitable compiler.
	 */
	BiaMachineCode(std::pair<const uint8_t*, size_t> p_machineCode);
	/**
	 * Move-Constructor.
	 * 
	 * @remarks	The moved object will be invalidated after the operation.
	 * 
	 * @param	[in,out]	p_move	Defines the object that should be moved.
	 */
	BiaMachineCode(BiaMachineCode && p_move);
	BiaMachineCode(const BiaMachineCode&) = delete;
	~BiaMachineCode();

	/**
	 * Executes the machine code.
	 * 
	 * @since	3.42.93.562
	 * @date	14-Dec-2017
	 */
	void Execute() const;
	/**
	 * Checks wheter the parameter passed to the constructor was valid or not.
	 * 
	 * @remarks	This function does not check if the machine code itself is valid.
	 * 
	 * @since	3.42.93.562
	 * @date	14-Dec-2017
	 * 
	 * @return	true if the machine code is valid, otherwise false.
	 */
	bool IsValid();

private:
	typedef void(*entry_point)();

	entry_point m_run;	/**	If non-null, defines the entry point of the machine code.	*/

	size_t m_iSize;	/**	Defines the size of the entry point.	*/
};

}
}