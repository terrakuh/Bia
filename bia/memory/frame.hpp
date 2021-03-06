#ifndef BIA_MEMORY_FRAME_HPP_
#define BIA_MEMORY_FRAME_HPP_

#include "stack.hpp"

#include <bia/internal/type/framer.hpp>
#include <bia/memory/gc/gc.hpp>
#include <bia/util/type_traits/is_frameable.hpp>
#include <cstdint>

namespace bia {
namespace memory {

class Frame
{
public:
	Frame(Stack& stack, gc::GC& gc, std::size_t offset) noexcept : _stack{ stack }, _gc{ gc }, _offset{ offset }
	{}
	Frame(Frame& parent, std::size_t offset) noexcept
	    : _stack{ parent._stack }, _gc{ parent._gc }, _offset{ offset - parent._offset }
	{}
	template<typename Type>
	void store(std::int32_t offset, const Type& value, bool mark = false)
	{
		static_assert(util::type_traits::Is_frameable<Type>::value, "Type is not frameable");
		using Framer = internal::type::Framer<Type>;

		std::lock_guard<std::mutex> _{ _stack._mutex };
		const auto position = _stack._memory.begin() + offset + _offset;
		if (reinterpret_cast<std::intptr_t>(position) % Framer::alignment()) {
			BIA_THROW(error::Code::bad_stack_alignment);
		} else if (position < _stack._memory.begin() || position + Framer::size() > _stack._memory.end()) {
			BIA_THROW(error::Code::out_of_stack);
		}
		Framer::frame(_gc, { position, Framer::size() }, value);
		if (mark) {
			_stack._meta[(offset + _offset) / sizeof(void*) / 8] |= 1 << (offset + _offset) / sizeof(void*) % 8;
		} else {
			_stack._meta[(offset + _offset) / sizeof(void*) / 8] &= ~(1 << (offset + _offset) / sizeof(void*) % 8);
		}
	}
	template<typename Type>
	Type load(std::int32_t offset) const
	{
		static_assert(util::type_traits::Is_frameable<Type>::value, "Type is not frameable");
		using Framer = internal::type::Framer<Type>;

		std::lock_guard<std::mutex> _{ _stack._mutex };
		const auto position = _stack._memory.begin() + offset + _offset;
		if (reinterpret_cast<std::intptr_t>(position) % Framer::alignment()) {
			BIA_THROW(error::Code::bad_stack_alignment);
		} else if (position < _stack._memory.begin() || position + Framer::size() > _stack._memory.end()) {
			BIA_THROW(error::Code::out_of_stack);
		}
		return Framer::unframe({ position, Framer::size() });
	}

private:
	Stack& _stack;
	gc::GC& _gc;
	std::size_t _offset;
};

} // namespace memory
} // namespace bia

#endif
