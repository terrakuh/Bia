#ifndef BIA_TOKENIZER_RESOURCE_iterator_HPP_
#define BIA_TOKENIZER_RESOURCE_iterator_HPP_

#include "../size.hpp"
#include "page.hpp"

#include <cstdint>
#include <iterator>
#include <memory>
#include <util/gsl.hpp>

namespace bia {
namespace tokenizer {
namespace resource {
namespace memory {

class iterator
{
public:
	typedef std::random_access_iterator_tag iterator_category;
	typedef util::byte value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const value_type* pointer;
	typedef value_type reference;

	int compare(const iterator& other) const noexcept
	{
		return 0;
	}
	bool operator==(const iterator& other) const noexcept
	{
		return compare(other) == 0;
	}
	bool operator!=(const iterator& other) const noexcept
	{
		return compare(other) != 0;
	}
	bool operator<(const iterator& other) const noexcept
	{
		return compare(other) < 0;
	}
	bool operator>(const iterator& other) const noexcept
	{
		return compare(other) > 0;
	}
	bool operator<=(const iterator& other) const noexcept
	{
		return compare(other) <= 0;
	}
	bool operator>=(const iterator& other) const noexcept
	{
		return compare(other) >= 0;
	}
	iterator& operator++()
	{
		return *this;
	}
	iterator operator++(int)
	{
		auto copy = *this;

		this->operator++();

		return copy;
	}
	iterator& operator--()
	{
		return *this;
	}
	iterator operator--(int)
	{
		auto copy = *this;

		this->operator--();

		return copy;
	}
	iterator& operator+=(difference_type diff)
	{
		return *this;
	}
	iterator& operator-=(difference_type diff)
	{
		return *this;
	}
	iterator& operator+(difference_type diff)
	{
		return *this;
	}
	iterator& operator-(difference_type diff)
	{
		return *this;
	}
	friend iterator& operator+(difference_type diff, iterator& right);
	friend iterator& operator-(difference_type diff, iterator& right);
	reference operator[](difference_type diff)
	{
		return this->operator*();
	}
	reference operator*() const
	{
		return static_cast<value_type>(0);
	}

private:
	std::shared_ptr<page_container_type> _pages;
	const util::byte* _current;
	std::size_t _page_index;
};

} // namespace memory
} // namespace resource
} // namespace tokenizer
} // namespace bia

#endif