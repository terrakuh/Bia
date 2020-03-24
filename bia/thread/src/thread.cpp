#include "thread/thread.hpp"

#include <exception/unsupported_error.hpp>
#include <exception>
#include <log/log.hpp>
#include <util/gsl.hpp>

#if BIA_THREAD_BACKEND_STD
#	include <thread>
#elif BIA_THREAD_BACKEND_NONE
#	if defined(BIA_THREAD_SLEEP_INCLUDE)
#		include BIA_THREAD_SLEEP_INCLUDE
#	endif
#endif

namespace bia {
namespace thread {

struct thread::impl
{
	bool valid;
#if BIA_THREAD_BACKEND_STD
	std::thread thread_backend;
	std::exception_ptr ex;
#endif

	impl(std::function<void()>&& target)
	{
		if (target) {
			valid = true;

#if BIA_THREAD_BACKEND_STD
			thread_backend = std::thread([target, this] {
				try {
					target();
				} catch (...) {
					ex = std::current_exception();
				}
			});

			BIA_LOG(INFO, "new thread launched");
#elif BIA_THREAD_BACKEND_NONE
			BIA_LOG(CRITICAL, "tried to launch thread; operation unsupported");

			BIA_THROW(exception::unsupported_error, "threading is not supported");
#endif
		} else {
			valid = false;
		}
	}
	~impl()
	{
#if BIA_THREAD_BACKEND_STD
		if (thread_backend.joinable()) {
			thread_backend.join();
		}
#endif
	}
};

thread::thread(std::function<void()> target) : _pimpl(new impl(std::move(target)))
{}

thread::~thread()
{}

void thread::yield()
{
#if BIA_THREAD_BACKEND_STD
	std::this_thread::yield();
#endif
}

void thread::sleep(std::chrono::milliseconds duration)
{
#if BIA_THREAD_BACKEND_STD
	std::this_thread::sleep_for(duration);
#elif BIA_THREAD_BACKEND_NONE
	BIA_THREAD_SLEEP_FUNCTION(duration);
#endif
}

void thread::join()
{
	BIA_EXPECTS(valid());

#if BIA_THREAD_BACKEND_STD
	_pimpl->thread_backend.join();

	if (_pimpl->ex) {
		std::rethrow_exception(_pimpl->ex);
	}
#endif
}

void thread::detach()
{
	BIA_EXPECTS(valid());

#if BIA_THREAD_BACKEND_STD
	_pimpl->thread_backend.detach();
#endif
}

bool thread::valid() const noexcept
{
#if BIA_THREAD_BACKEND_STD
	return _pimpl->valid && _pimpl->thread_backend.joinable();
#elif BIA_THREAD_BACKEND_NONE
	return false;
#endif
}

bool thread::supported() noexcept
{
	return BIA_THREAD_SUPPORTED;
}

} // namespace thread
} // namespace bia