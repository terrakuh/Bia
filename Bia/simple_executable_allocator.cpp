#include "simple_executable_allocator.hpp"
#include "config.hpp"
#include "exception.hpp"

#if defined(BIA_OS_WINDOWS)
#include <windows.h>
#elif defined(BIA_OS_LINUX)
#include <sys/mman.h>
#endif


namespace bia
{
namespace machine
{
namespace memory
{

void simple_executable_allocator::deallocate(universal_allocation _allocation)
{
#if defined(BIA_OS_WINDOWS)
	VirtualFree(_allocation.first, 0, MEM_RELEASE);
#elif defined(BIA_OS_LINUX)
	munmap(_allocation.first, _allocation.second);
#endif
}

void simple_executable_allocator::deallocate_blocks(universal_allocation _blocks)
{
#if defined(BIA_OS_WINDOWS)
	VirtualFree(_blocks.first, 0, MEM_RELEASE);
#elif defined(BIA_OS_LINUX)
	munmap(_blocks.first, block_size * _blocks.second);
#endif
}

void simple_executable_allocator::protect(universal_allocation _allocation, int _protection)
{
#if defined(BIA_OS_WINDOWS)
	// Set protections flags
	DWORD _old = 0;
	DWORD _new = 0;

	if (_protection & PF_READ_WRITE) {
		_new |= PAGE_READWRITE;
	}

	if (_protection & PF_EXECUTE) {
		_new |= PAGE_EXECUTE;
	}

	if (!VirtualProtect(_allocation.first, _allocation.second, _new, &_old)) {
		throw exception::memory_error(BIA_EM_FAILED_MEMORY_PROTECTION);
	}
#elif defined(BIA_OS_LINUX)
	int _new = 0;

	if (_protection & PF_READ_WRITE) {
		_new |= PROT_READ | PROT_WRITE;
	}

	if (_protection & PF_EXECUTE) {
		_new |= PROT_EXEC;
	}

	if (mprotect(_allocation.first, _allocation.second, _new)) {
		throw exception::memory_error(BIA_EM_FAILED_MEMORY_PROTECTION);
	}
#endif
}

simple_executable_allocator::universal_allocation simple_executable_allocator::commit(universal_allocation _allocation, size_t _size)
{
	return _allocation;
}

simple_executable_allocator::universal_allocation simple_executable_allocator::allocate(size_t _size)
{
#if defined(BIA_OS_WINDOWS)
	auto _ptr = VirtualAlloc(nullptr, _size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!_ptr) {
#elif defined(BIA_OS_LINUX)
	auto _ptr = mmap(nullptr, _size, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (_ptr == reinterpret_cast<void*>(-1)) {
#endif
		throw exception::memory_error(BIA_EM_FAILED_ALLOCATION);
	}

	return { _ptr, _size };
}

simple_executable_allocator::universal_allocation simple_executable_allocator::allocate_blocks(size_t _count)
{
	return { allocate(get_block_size() * _count).first, _count };
}

simple_executable_allocator::universal_allocation simple_executable_allocator::prepare(size_t _size)
{
	return allocate(_size);
}

}
}
}