#ifndef BIA_ERROR_SOURCE_LOCATION_HPP_
#define BIA_ERROR_SOURCE_LOCATION_HPP_

#include <cstdint>
#include <ostream>
#include <type_traits>

#define BIA_CURRENT_SOURCE_LOCATION bia::error::source_location(__LINE__, 0, __FILE__, __func__)

namespace bia {
namespace error {

class source_location
{
public:
	source_location() = default;
	source_location(std::uint_least32_t line, std::uint_least32_t column, const char* file_name,
	                const char* function_name) noexcept
	{
		_line          = line;
		_column        = column;
		_file_name     = file_name;
		_function_name = function_name;
	}
	std::uint_least32_t line() const noexcept
	{
		return _line;
	}
	std::uint_least32_t column() const noexcept
	{
		return _column;
	}
	const char* file_name() const noexcept
	{
		return _file_name;
	}
	const char* function_name() const noexcept
	{
		return _function_name;
	}

private:
	std::uint_least32_t _line;
	std::uint_least32_t _column;
	const char* _file_name;
	const char* _function_name;
};

} // namespace error
} // namespace bia

template<typename Ostream>
inline typename std::enable_if<std::is_base_of<std::ostream, Ostream>::value, Ostream&>::type
    operator<<(Ostream& stream, const bia::error::source_location& source_location)
{
	stream << source_location.file_name() << ":" << source_location.line() << " ["
	       << source_location.function_name() << "]";
	return stream;
}

#endif
