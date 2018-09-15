#pragma once

#include "config.hpp"
#include "module_library.hpp"
#include "allocator.hpp"


namespace bia
{
namespace machine
{
namespace modular
{

class module_loader
{
public:
	BIA_EXPORT module_loader(memory::allocator * _allocator);
	BIA_EXPORT ~module_loader();
	
	BIA_EXPORT void unload_module(module_library * _module);
	BIA_EXPORT module_library * load_bll(const char * _filepath, const char * _name);

private:
	struct impl;

	memory::allocator * _allocator;
	memory::allocation<impl> _impl;
};

}
}
}
