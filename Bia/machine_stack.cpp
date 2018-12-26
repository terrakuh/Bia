#include "machine_stack.hpp"
#include "undefined_member.hpp"
#include "max_member_size.hpp"


namespace bia
{
namespace machine
{

machine_stack::machine_stack(memory::allocator * _allocator, size_t _size)
{
	auto _allocation = _allocator->allocate(_size);
	
	this->_allocator = _allocator;
	_buffer = static_cast<int8_t*>(_allocation.first);
	_max_size = _allocation.second;
	_cursor = 0;
}

machine_stack::machine_stack(machine_stack && _move) noexcept
{
	_allocator = _move._allocator;
	_buffer = _move._buffer;
	_max_size = _move._max_size;
	_cursor = _move._cursor;

	_move._buffer = nullptr;
	_move._max_size = 0;
	_move._cursor = 0;
}

machine_stack::~machine_stack()
{
	if (_buffer) {
		_allocator->deallocate(memory::universal_allocation(_buffer, _max_size));
	}
}

void machine_stack::pop(uint32_t _member_count)
{
	if (_cursor < _member_count * framework::max_member_size) {
		throw 1;
	}

	_cursor -= _member_count * framework::max_member_size;

	auto _ptr = _buffer + _cursor;

	// Destroy members
	while (_member_count--) {
		reinterpret_cast<framework::member*>(_ptr)->~member();
		_ptr += framework::max_member_size;
	}
}

void machine_stack::recreate(uint32_t _member_count)
{
	if (_cursor < _member_count * framework::max_member_size) {
		throw 1;
	}

	auto _ptr = _buffer + _cursor - _member_count * framework::max_member_size;

	// Recreate members
	while (_member_count--) {
		reinterpret_cast<framework::member*>(_ptr)->undefine();
		_ptr += framework::max_member_size;
	}
}

void machine_stack::push(framework::member ** _destination, uint32_t _member_count)
{
	auto _ptr = _buffer + _cursor;

	if (_cursor + _member_count * framework::max_member_size > _max_size) {
		throw 1;
	}

	_cursor += _member_count * framework::max_member_size;

	// Create members
	while (_member_count--) {
		*_destination = new(_ptr) framework::undefined_member();
		printf("created: %p -> %p\n", _ptr, _destination++);
		_ptr += framework::max_member_size;
	}
}

}
}