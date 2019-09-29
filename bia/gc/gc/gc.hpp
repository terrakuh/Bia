#pragma once

#include "memory_allocator.hpp"
#include "object.hpp"
#include "object_ptr.hpp"
#include "root.hpp"
#include "std_memory_allocator.hpp"
#include <cstdio>
#include <array>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception/implementation_error.hpp>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_set>
#include <thread/data/synchronized_stack.hpp>
#include <thread/shared_lock.hpp>
#include <thread/shared_spin_mutex.hpp>
#include <thread/spin_mutex.hpp>
#include <thread/thread_pool.hpp>
#include <util/type_traits/conjunction.hpp>
#include <utility>
#include <vector>

namespace bia {
namespace gc {

/*
 A garbage collector highly optimized for the BVM. Using this collector for custom task is not recommended.

 @see memory_allocator
*/
class gc
{
public:
	template<std::size_t Roots>
	class token
	{
	public:
		token(const token& copy) = delete;
		token(token&& move) noexcept : g(move.g), roots(std::move(move.roots))
		{
			move.g = nullptr;
		}
		/*
		 Destructor.
		*/
		~token() noexcept
		{
			// deregister from gc
			if (g) {
				active_gc_instance = nullptr;

				// free roots
				for (auto& root : roots) {
					g->free_root(root);
				}
			}
		}
		/*
		 Sets the destination pointer to the source pointer. If the gc is active, additional work is done.

		 @param[out] dest defines the destination
		 @param src defines the source
		*/
		void set_object_ptr(object_ptr& dest, object_ptr& src)
		{
			thread::shared_lock<decltype(g->mutex)> lock(g->mutex);

			if (g->is_active) {
				g->missed_objects.push(dest.get());
			}

			dest.set(src.get());
		}
		/*
		 Sets the destination pointer to the source pointer. If the gc is active, additional work is done.

		 @param[out] dest defines the destination
		 @param src defines the source
		*/
		void set_object_ptr(object_ptr& dest, void* src)
		{
			thread::shared_lock<decltype(g->mutex)> lock(g->mutex);

			if (g->is_active) {
				g->missed_objects.push(dest.get());
			}

			dest.set(src);
		}
		void set(std::size_t root_index, std::size_t index, void* src)
		{
			set_object_ptr(roots[root_index][index], src);
		}
		root& root_at(std::size_t index) noexcept
		{
			return roots[index];
		}

	private:
		friend gc;

		/* the parent gc */
		gc* g;
		/* the roots */
		std::array<root, Roots> roots;

		template<typename... Counts>
		token(gc* g, Counts... counts) noexcept : g(g), roots{ g->create_root(counts)... }
		{
			static_assert(sizeof...(Counts) == Roots, "Size of Counts and Roots mismatch.");
		}
	};

	/*
	 Constructor.

	 @param allocator provides the required memory for the allocations
	 @param thread_pool (optional) launches the gc thread for concurrent
	 collection; this pool should be exclusively used by this collector
	*/
	gc(std::unique_ptr<memory_allocator> allocator, std::unique_ptr<bia::thread::thread_pool> thread_pool)
		: mem_allocator(std::move(allocator)),
		  missed_objects(std::vector<void*, std_memory_allocator<void*>>(mem_allocator.get()))
	{
		// Start gc main thread
		/*if (thread_pool && thread_pool->max_pool_size() > 0 && thread_pool->task_count() == 0) {
			thread_pool->execute(std::bind(&gc::main_thread, this));
		}*/
	}
	void run_synchronously()
	{
		main_thread();
	}
	/*
	 Registeres the current thread. If this thread has already been registered, an exception is thrown.

	 @param counts is how many roots and with how much space they should be created
	 @returns a token which deregisteres this thread upon destruction
	 @tparam Counts must be integers describing how large each root should be
	*/
	template<typename... Counts>
	typename std::enable_if<bia::util::type_traits::conjunction<std::is_unsigned<Counts>::value...>::value, token<sizeof...(Counts)>>::type register_thread(Counts... counts)
	{
		// There is already a registered instance
		if (active_gc_instance) {
			throw exception::implementation_error(u"active gc instance detected");
		}

		active_gc_instance = this;

		return { this, counts... };
	}
	void* allocate(std::size_t size, bool zero = false)
	{
		auto ptr = allocate_impl(size, true);

		if (zero) {
			std::memset(ptr, 0, size);
		}

		return ptr;
	}
	/*
	 Allocates and constructs the given type with its arguments.

	 @param args are forwarded to the constructor of *T*
	 @returns the newly constructed object as a @ref object_ptr
	 @throws allocate_impl()
	 @tparam T is the type that should be created
	 @tparam Args are the argument types for the constructor
	*/
	template<typename T, typename... Args>
	typename std::enable_if<std::is_base_of<object, T>::value, T*>::type construct(Args&&... args)
	{
		static_assert(alignof(T) <= memory_allocator::alignment,
					  "the alignement cannot be greater than the max alignment "
					  "of the memory allocator");
		static_assert(alignof(object_info) <= memory_allocator::previous_data_alignment,
					  "the required alignment of object info is not met");

		return new (allocate_impl(sizeof(T), false)) T(std::forward<Args>(args)...);
	}
	/*
	 Returns the memory allocator of this garbage collector. Memory allocated
	 through this allocator will not be garbage collected.

	 @returns the internally used memory allocator
	*/
	memory_allocator* allocator() noexcept
	{
		return mem_allocator.get();
	}
	/*
	 Returns the active gc of the current thread.

	 @returns the current gc if set, otherwise `nullptr`
	*/
	static gc* active_gc() noexcept;

private:
	/* the currently active instance */
	static thread_local gc* active_gc_instance;
	/* the memory allocator */
	std::unique_ptr<memory_allocator> mem_allocator;

	thread::spin_mutex allocated_mutex;
	/* a list of all gc monitored objects allocated before the gc was active */
	std::unordered_set<object_info*, std::hash<object_info*>, std::equal_to<object_info*>> allocated;

	thread::spin_mutex roots_mutex;
	/* a list of all created roots */
	std::unordered_set<root, std::hash<root>, std::equal_to<root>> roots;

	/* a stack which stores all (potentially) missed objects while the gc was
	 concurrently collecting */
	thread::data::synchronized_stack<std::vector<void*, std_memory_allocator<void*>>, thread::spin_mutex>
		missed_objects;

	/* locks shared when allocating memory */
	thread::shared_spin_mutex mutex;
	/* the current gc mark; this is only set by the main gc thread and is locked by gc_mutex */
	bool current_mark =false;
	/* whether the gc is currently in the marking phase */
	bool is_active = false;

	void* allocate_impl(std::size_t size, bool leaf)
	{
		auto ptr  = mem_allocator->checked_allocate(size + sizeof(object_info), sizeof(object_info));
		auto info = new (ptr) object_info();

		info->leaf = leaf;

		mutex.lock_shared();
		info->marked.store(current_mark, std::memory_order_relaxed);
		mutex.unlock_shared();

		std::unique_lock<decltype(allocated_mutex)> lock(allocated_mutex);

		allocated.insert(info);

		return static_cast<int8_t*>(ptr) + sizeof(object_info);
	}
	root create_root(std::size_t ptr_count)
	{
		auto ptr = static_cast<object_ptr*>(mem_allocator->checked_allocate(ptr_count * sizeof(object_ptr), 0));

		// Construct the pointer objects
		for (std::size_t i = 0; i < ptr_count; ++i) {
			new (ptr + i) object_ptr();
		}

		std::unique_lock<decltype(roots_mutex)> lock(roots_mutex);

		roots.insert({ ptr, ptr_count });

		return { ptr, ptr_count };
	}
	void free_root(root obj)
	{
		{
			std::unique_lock<decltype(roots_mutex)> lock(roots_mutex);

			roots.erase(obj);
		}

		// Destroy the pointer objects
		for (std::size_t i = 0; i < obj.size(); ++i) {
			obj.ptrs[i].~object_ptr();
		}

		mem_allocator->deallocate(obj.ptrs, 0);
	}
	void main_thread()
	{
		mutex.lock();
		is_active	= true;
		current_mark = !current_mark;
		mutex.unlock();

		// mark
		{
			std::unique_lock<decltype(roots_mutex)> lock(roots_mutex);

			for (auto r : roots) {
				for (std::size_t i = r.size(); i-- > 0;) {
					object::gc_mark(r[i], current_mark);
				}
			}
		}

		// finished
		mutex.lock();
		is_active = false;
		mutex.unlock();

		// mark missed
		void* missed = nullptr;

		while (missed_objects.pop(missed)) {
			object::gc_mark(missed, current_mark);
		}
printf("Size of roots: %zi\n", roots.size());
printf("Size of allocated: %zi\n", allocated.size());
		// sweep
		{
			std::unique_lock<decltype(allocated_mutex)> lock(allocated_mutex);

			for (auto i = allocated.begin(); i != allocated.end(); ) {
				auto info = *i;

				if (info->marked.load(std::memory_order_consume) != current_mark) {
					if (!info->leaf) {
						reinterpret_cast<object*>(info + 1)->~object();
					}

					printf("GCing %p\n", info + 1);

					mem_allocator->deallocate(info, sizeof(object_info));
					i = allocated.erase(i);
				} else {
					++i;
					printf("Not marked %p\n", info + 1);
				}
			}
		}
	}
};

} // namespace gc
} // namespace bia