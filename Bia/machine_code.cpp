#include "machine_code.hpp"

#include <cstring>


namespace bia
{
namespace machine
{

machine_code::machine_code(std::pair<const uint8_t*, size_t> _machine_code, machine_schein _machine_schein) : _machine_schein(std::move(_machine_schein))
{
	// Allocate
	_entry_point = this->_machine_schein.executable_allocator()->allocate_executable(_machine_code.second);

	// Copy code
	memcpy(_entry_point.first, _machine_code.first, _machine_code.second);
	this->_machine_schein.executable_allocator()->protect_executable(_entry_point, memory::executable_allocator::PF_EXECUTE);
}

machine_code::machine_code(machine_code && _rvalue) noexcept : _machine_schein(std::move(_rvalue._machine_schein))
{
	_entry_point = _rvalue._entry_point;
	_rvalue._entry_point.clear();
}

machine_code::~machine_code()
{
	clear();
}

void machine_code::execute() const
{
	if (is_executable()) {
		reinterpret_cast<entry_point>(_entry_point.first)();
	}
}

void machine_code::clear()
{
	if (_entry_point.first) {
		_machine_schein.executable_allocator()->deallocate_executable(_entry_point);
		_entry_point.clear();
	}
}

bool machine_code::is_executable() const noexcept
{
	return _entry_point.first && _entry_point.second;
}

machine_code & machine_code::operator=(machine_code && _rvalue)
{
	clear();

	// Move
	_machine_schein = std::move(_rvalue._machine_schein);
	_entry_point = _rvalue._entry_point;
	_rvalue._entry_point.clear();

	return *this;
}

}
}