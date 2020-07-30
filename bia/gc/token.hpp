#ifndef BIA_GC_TOKEN_HPP_
#define BIA_GC_TOKEN_HPP_

#include "gc.hpp"
#include "gcable.hpp"
#include "object/base.hpp"
#include "object/header.hpp"
#include "object/pointer.hpp"
#include "root.hpp"
#include "stack_view.hpp"

#include <bia/util/gsl.hpp>
#include <type_traits>

namespace bia {
namespace gc {

class gc;

/**
 * A token generated by registering the current thread in the gc index. This token manages the gc root.
 */
class token : public object::base
{
public:
	/**
	 * Destructor.
	 */
	~token() noexcept
	{
		_deregister();
	}
	/**
	 * Sets `dest` to `src`.
	 *
	 * @param[in,out] dest defines the destination
	 * @param src defines the source
	 */
	template<typename T>
	void set(object::pointer<T>& dest, object::pointer<T>& src) noexcept
	{
		set(dest, src.get());
	}
	template<typename T>
	void set(object::pointer<T>& dest, const object::immutable_pointer<T>& src) noexcept
	{
		set(dest, src.get());
	}
	template<typename T>
	void set(object::pointer<T>& dest, gcable<T>&& src)
	{
		set(dest, src.peek());

		if (src.valid()) {
			src.start_monitor();
		}
	}
	/**
	 * Sets the destination pointer to the source pointer. If the gc is active, additional work is done.
	 *
	 * @param[out] dest defines the destination
	 * @param src defines the source
	 * @tparam Destination the destination type
	 * @tparam Source the source type
	 */
	template<typename Destination, typename Source>
	typename std::enable_if<std::is_base_of<Destination, Source>::value>::type
	    set(object::pointer<Destination>& dest, Source* src) noexcept
	{
		// gc is active and dest could be possibly missed
		if (const auto ptr = dest.get()) {
			const auto header = reinterpret_cast<object::header*>(ptr) - 1;

			while (true) {
				auto current_index = _gc->_miss_index.load(std::memory_order_consume);

				if (header->miss_index.exchange(current_index, std::memory_order_release) <= current_index) {
					break;
				}
			}
		}

		dest.set(src);
	}
	/**
	 * Returns the internal root object.
	 *
	 * @returns a reference to the root
	 */
	class stack_view stack_view() noexcept
	{
		return { _base, _size };
	}

protected:
	void gc_mark_children(bool mark) const noexcept override
	{
		for (auto i = _base, c = _base + _size; i != c; ++i) {
			if (const auto ptr = i->get()) {
				object::gc_mark(ptr, mark);
			}
		}
	}
	void register_gcables(gc& gc) const noexcept override
	{}

private:
	friend gc;

	/** the parent gc */
	gc* _gc;
	stack_view::element_type* _base;
	std::size_t _size;

	/**
	 * Constructor.
	 *
	 * @pre no active garbage collector
	 *
	 * @param[in] gc the parent garbage collector
	 * @param stack_size the stack element count
	 * @throw see memory::allocator::checked_allocate()
	 */
	token(gc* gc, std::size_t stack_size)
	{
		BIA_EXPECTS(!gc::_active_gc_instance);

		// activate this gc
		gc::_active_gc_instance = gc;
		_gc                     = gc;
		_base                   = static_cast<stack_view::element_type*>(
            gc->_allocator->checked_allocate(stack_size * sizeof(stack_view::element_type)).get());
		_size = stack_size;

		// initialize all elements
		for (auto i = _size; i--;) {
			new (_base + i) stack_view::element_type{ nullptr };
		}

		_gc->_roots.add(this);
	}
	void _deregister()
	{
		if (_gc) {
			gc::_active_gc_instance = nullptr;
			_gc->deregister_root(this);

			for (auto i = _size; i--;) {
				_base[i].~pointer();
			}

			_gc->_allocator->deallocate(_base);
		}
	}
};

} // namespace gc
} // namespace bia

#endif
