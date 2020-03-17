#include "tokenizer/resource/memory/streambuf.hpp"

#include "tokenizer/resource/manager.hpp"

#include <exception/memory_error.hpp>
#include <utility>

namespace bia {
namespace tokenizer {
namespace resource {
namespace memory {

streambuf::streambuf(streambuf&& move) noexcept
{
	std::swap(_manager, move._manager);
	std::swap(_size, move._size);
}

streambuf::~streambuf()
{
	if (valid() && pptr()) {
		close(true);

		_manager->_state.cursor = reinterpret_cast<util::byte*>(pptr()) + _size->size;
		_manager->_buf_active    = false;

		_update_size(nullptr);

		//todo if current size was not used remove more flag
		//if (_size->size)
	}
}

class memory streambuf::close(bool discard)
{
	BIA_EXPECTS(valid());

	if (discard) {
		return {};
	}
}

bool streambuf::valid() const noexcept
{
	return _manager;
}

streambuf::int_type streambuf::sync()
{
	// get next page
	if (pptr() == epptr()) {
		try {
			auto page = _manager->_next_page();

			_update_size(new (page.first) size{});
			setp(reinterpret_cast<char*>(_size + 1), reinterpret_cast<char*>(page.second));
		} catch (const exception::bia_error&) {
			return -1;
		}
	}

	return 0;
}

streambuf::int_type streambuf::overflow(int_type c)
{
	if (sync() == 0) {
		if (c != traits_type::eof()) {
			sputc(traits_type::to_char_type(c));

			return c;
		}

		return c;
	}

	return traits_type::eof();
}

streambuf::streambuf(util::not_null<manager*> manager)
{
	_manager = manager.get();

	if (sync() != 0) {
		BIA_THROW(exception::memory_error, "cannot create page");
	}
}

void streambuf::_update_size(size* s) noexcept
{
	if (_last_size) {
		_last_size->more = 1;
	}

	_last_size = _size;
	_size      = s;
}

} // namespace memory
} // namespace resource
} // namespace tokenizer
} // namespace bia