#ifndef BIA_MEMBER_NATIVE_LIST_HPP_
#define BIA_MEMBER_NATIVE_LIST_HPP_

#include "../member.hpp"

#include <bia/thread/lock/spin_mutex.hpp>
#include <vector>

namespace bia {
namespace member {
namespace native {

class list : public member
{
public:
	typedef std::vector<member*> list_type;

	list(list_type data) noexcept;
	flag_type flags() const override;
	test_type test(test_operator op, const member& right) const override;
	gc::gcable<member> copy() const override;
	gc::gcable<member> invoke(parameters_type params, invoke_context& context) override;
	gc::gcable<member> operation(const member& right, infix_operator op) override;
	gc::gcable<member> self_operation(self_operator op) override;
	gc::gcable<member> get(const native::string& name) override;
	float_type as_float() const noexcept override;
	int_type as_int() const noexcept override;
	bool as_data(const std::type_info& type, void* output) override;
	bool as_data(const std::type_info& type, void* output) const override;
	void gc_mark_children(bool mark) const noexcept override;

protected:
	void register_gcables(gc::gc& gc) const noexcept override;

private:
	list_type _data;
	mutable thread::lock::spin_mutex _mutex;

	void _push(connector::parameters_type params);
	void _pop(connector::parameters_type params);
	void _reserve(std::size_t size);
	void _shrink_to_fit();
	void _clear();
	void _insert(connector::parameters_type params);
	void _erase(connector::parameters_type params);
	void _reverse();
};

} // namespace native
} // namespace member
} // namespace bia

#endif
