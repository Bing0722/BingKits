#pragma once

#include <Bkits/Common-inl.h>
#include <ostream>

// clang-format off
namespace BKits
{
namespace Base
{

template <typename Char>
std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& os,
                                     const detail::basic_string_view<Char>& sv) {
    return os.write(sv.data(), sv.size());
}

using string_view = detail::basic_string_view<char>;

} // namespace Base
} // namespace BKits
// clang-format on