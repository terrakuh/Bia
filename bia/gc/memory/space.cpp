#include "space.hpp"

#include <algorithm>

using namespace bia;
using namespace bia::gc::memory;

space::space(util::not_null<std::shared_ptr<allocator>> allocator, size_type page_size) noexcept
    : _allocator(std::move(allocator.get())),
      _pages(new page_container_type(std_allocator<util::byte*>(_allocator)), [this] {
		  auto allocator = _allocator;

	      return [allocator](page_container_type* ptr) {
		      for (auto page : *ptr) {
				  allocator->deallocate(page);
		      }

			  delete ptr;
	      };
      }())
{
	_page_size = page_size;
}

void space::truncate(size_type s)
{
	BIA_EXPECTS(s <= size());

	_index  = s / _page_size;
	_offset = s % _page_size;
}

bool space::valid() const noexcept
{
	return _allocator != nullptr;
}

space::size_type space::size() const
{
	BIA_EXPECTS(valid());

	return _offset + _index * _page_size;
}

space::size_type space::capacity() const
{
	BIA_EXPECTS(valid());

	return _pages->size() * _page_size;
}

util::not_null<util::span<util::byte*>> space::next_region(size_type size)
{
	BIA_EXPECTS(valid());

	// go to next page
	if (_offset == _page_size) {
		_offset = 0;

		++_index;
	}

	// allocate page
	if (_pages->size() <= _index) {
		_pages->push_back(static_cast<util::byte*>(_allocator->checked_allocate(_page_size).get()));
	}

	const auto cursor = (*_pages)[_index] + _offset;

	size = std::min(_page_size - _offset, size);
	_offset += size;

	return util::span<util::byte*>(cursor, size);
}

iterator space::cursor(size_type pos) const
{
	if (pos == cpos) {
		pos = size();
	}

	BIA_EXPECTS(valid() && pos <= size());

	return { _pages, _page_size, pos / _page_size, pos % _page_size };
}
