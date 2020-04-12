#ifndef BIA_COMPILER_VARIABLE_MANAGER_HPP_
#define BIA_COMPILER_VARIABLE_MANAGER_HPP_

#include <resource/view.hpp>
#include <map>
#include <utility>
#include <vector>

namespace bia {
namespace compiler {

class variable_manager
{
public:
	typedef std::uint32_t index_type;
	typedef std::uint32_t scope_index_type;

	/**
	 * Returns the index of the scope and the variable. If the variable does not yet exist, it will be created
	 * on the newest stack.
	 *
	 * @pre at least one active scope
	 *
	 * @param identifier the name of the variable
	 * @returns the index of the scope and the variable
	 */
	std::pair<scope_index_type, index_type> index_of(resource::view identifier);
	/**
	 * Returns the index of the latest scope.
	 *
	 * @pre at least one active scope
	 *
	 * @returns the scope index
	 */
	scope_index_type latest_scope() const;
	/**
	 * Returns the index of the latest variable.
	 * 
	 * @pre at least one variable and a scope
	 * 
	 * @returns the variable index
	*/
	index_type latest_variable() const;
	void open_scope();
	void close_scope();

private:
	struct scope
	{
		std::map<resource::view, index_type> variables;
		index_type index = 0;
	};

	std::vector<scope> _scopes;
};

} // namespace compiler
} // namespace bia

#endif