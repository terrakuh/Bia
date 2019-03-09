#include "scope_exit.hpp"

#include <utility>


namespace bia
{
namespace utility
{

scope_exit::scope_exit(function_t && _exit) noexcept : _exit(std::move(_exit))
{
}

scope_exit::~scope_exit()
{
	if (_exit) {
		_exit();
	}
}

}
}